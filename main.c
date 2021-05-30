//polynom - Shared Memory
//author: Liran Libster
//description: this program separate a big polynom into small polynoms and let shared memory count each one, then summaries all the results.

#include <stdio.h>

#include <stdlib.h>

#include <string.h>

#include <sys/types.h>

#include <sys/ipc.h>

#include <sys/shm.h>

#include <unistd.h>

#include <wait.h>

#define MAX 512

void freeArray(char ** array, int numberofWords);

int countExps(char * word);

char ** buildArray(char * word, int numofExps);

int expressionSolution(char * word);

int solution = 0;
int value = 0;

int main() {

    char str[MAX];
    char ** array;

    while (1) {

        solution = 0;
        printf("Please enter polynom and value:\n");
        fgets(str, MAX, stdin);
        str[strlen(str) - 1] = '\0';
        int spaceFlag = 0;
        int i, result;

        for (i = 0; i < strlen(str); i++) {
            if (str[i] == ' ') {
                value = atoi( & str[i + 1]);
            }
        }

        for (i = 0; i < strlen(str); i++) { //loop to check if the polynom entered correctly without additional spaces
            if (str[i] == ',') {
                if (str[i + 2] == ' ') {
                    printf("wrong input, fix spaces!\n");
                    spaceFlag = 1;
                    break;
                }
                break;
            }
            if (str[i] == ' ') {
                printf("wrong input, fix spaces!\n");
                spaceFlag = 1;
                break;
            }
        }

        int numberofExps = countExps(str);
        array = buildArray(str, numberofExps);

        if (strcmp(array[0], "done") == 0 && numberofExps == 1) {
            freeArray(array, numberofExps);
            exit(0);
        }

        key_t key;
        if ((key = ftok("/tmp", 'y')) == -1) {
            perror("ftok() failed");
            exit(EXIT_FAILURE);
        }
        int shm_id;
        shm_id = shmget(key, numberofExps, IPC_CREAT | IPC_EXCL | 0600);
        if (shm_id == -1) {
            perror("shmget failed");
            exit(EXIT_FAILURE);
        }

        int * shm_ptr;
        shm_ptr = (int * ) shmat(shm_id, NULL, 0);
        if (shm_ptr == (int * ) - 1) {
            perror("shmat failed");
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < numberofExps; j++) {

            pid_t child = fork();

            if (child < 0) {
                freeArray(array, numberofExps);
                perror("ERR");
                exit(1);
            }
            if (child == 0) {
                result = expressionSolution(array[j]);
                shm_ptr[j] = result;
                exit(EXIT_SUCCESS);
            }
            wait(NULL);
        }

        for (int res = 0; res < numberofExps; res++) { //combine all the solutions that save in the shared memory array
            solution += shm_ptr[res];
        }
        if (shmctl(shm_id, IPC_RMID, NULL) == -1) {
            perror("shmctl failed");
            exit(EXIT_FAILURE);
        }
        shmdt(shm_ptr);
        if (spaceFlag == 0) {
            printf("%d\n", solution);
        }
        freeArray(array, numberofExps);
        value = 0;
        solution = 0;
    }

}

char ** buildArray(char * word, int numofExps) {

    char ** Arr;
    Arr = (char ** ) malloc((numofExps + 1) * sizeof(char * ));
    if (Arr == NULL) {
        fprintf(stderr, "ERR");
        exit(1);
    }

    int charNum = 0; //count characters
    int arrCounter = 0;

    for (int j = 0; j < strlen(word); j++) {

        if (word[j] != '+') {

            charNum++;

            if (word[j + 1] == '+' || word[j + 1] == '\0') {

                char * newExp = malloc((charNum + 1) * sizeof(char));

                if (newExp == NULL) {
                    freeArray(Arr, numofExps);
                    fprintf(stderr, "ERR");
                    exit(1);
                }

                //insert current expression into newExp
                int counter = 0;
                for (int k = j - charNum + 1; k < (j - charNum + 1) + charNum; k++) {
                    newExp[counter] = word[k];
                    counter++;

                }

                newExp[counter] = '\0';
                Arr[arrCounter] = (char * ) malloc(sizeof(char) * (charNum + 1));
                if (Arr[arrCounter] == NULL) {
                    free(newExp);
                    freeArray(Arr, numofExps);
                    fprintf(stderr, "ERR");
                    exit(1);
                }
                strcpy(Arr[arrCounter], newExp);
                free(newExp);
                arrCounter++;
                charNum = 0;

            }
        }
    }

    return Arr;
}

int countExps(char * word) {

    int counter = 1;
    for (int i = 0; i < strlen(word); i++) {
        if (word[i] == '+') {
            counter++;
        }
    }
    return counter;
}

void freeArray(char ** array, int numberofWords) {

    for (int setFree = 0; setFree < numberofWords; setFree++) {
        free(array[setFree]);
    }
    free(array);

}

int expressionSolution(char * word) {

    char coefficient[MAX];;
    char exponent[MAX];
    int coeff, exp;
    int result = value;
    int flag = 0;
    int xcheck = 0;
    if (word[0] == 'x' && word[1] == '\0' || word[0] == 'x' && word[1] == ',') {
        flag = 1;
    }

    for (int i = 0; i < strlen(word); i++) { //loop that build chars for the coefficient and the exponent.
        if (word[i] == '*') {
            if (word[i + 2] == '^') {
                exponent[0] = word[i + 3];
            } else {
                exponent[0] = 1;
            }
            break;
        }
        if (word[i] == ',') {
            break;
        }
        coefficient[i] = word[i];
    }
    coefficient[strlen(coefficient)] = '\0';
    for (int j = 0; j < strlen(word); j++) {
        if (word[j] == 'x') {
            xcheck = 1;
        }
    }

    if (xcheck == 0) { //if there is no x in the expression add only the coefficient to the solution
        coeff = atoi(coefficient);
        result = coeff;
    } else {
        for (int j = 0; j < strlen(word); j++) {
            if (word[j] == '^') {
                exponent[0] = word[j + 1];
            }
        }

        coeff = atoi(coefficient);
        exp = atoi(exponent);
        if (coeff == 0) {
            coeff = 1;
        }
        if (flag == 0) {
            for (int j = 0; j < exp - 1; j++) {
                result *= value;
            }

            if (exp == 0) {
                result = coeff * value;
            } else {
                result *= coeff;
            }
        } else {
            result += 0;
        }
        solution += result;
    }
    return result;

}