# Polynom-SharedMemory

Shared Memory

Authored by Liran Libster


==Description==

this program separate a big polynom into small polynoms and let shared memory count each one, then summaries all the results.

Functions:

buildArray - this function receives a polynom that the user inputs and the number of expressions in the string, builds a 2D array of separated polynoms and return the array. 

countExps - this function counts how many expressions in the string.

freeArray - this function runs in all cells of the 2D array and free the allocated memory.

expressionSolution - this function receives an expression(cell) from the 2d array that the buildArray function built (each cell contains an expression), then calculates the expression.

==Program files==

main.c


==How to compile==

compile: gcc main.c -o main
run: ./main

==Input==

"POLYNOM, VALUE" 

example: 2*x^3+3*x^2+x+2, 2

example 2: 3, 4

example 3: x^2  , 1

to exit type: done

==Output==

example 1: 32

example 2: 3

example 3: wrong input, fix spaces!
