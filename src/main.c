/* main.c - source file for the implementation of the SAP1 virtual machine.

   The SAP1 VM emulates the computer with the same name devised by Albert P.
   Malvino in the book Digital Computer Electronics and further implemented by
   Ben Eater in his YouTube series "Building an 8-bit Breadboard Computer".

   TODO: - Save the VM state to file.
         - Load the VM state from file.
         - Save a VM program to file.
         - Load a VM program from file.
         - Write a simple assembler for VM programs.
         - Rewrite the ex_add and ex_sub functions to mimic the way the
             physical machine handles the ADD and SUB instructions.
         - Add a graphical user interface.  */

#include <stdio.h>

#include "sap1.h"

int
main(void)
{
    struct CPU cpu;
    int run = 0;

    /* A 333ms delay results in an approximately 3Hz clock pulse, which is
       the slowest clock speed of my physical SAP1 machine.  The fastest
       speed of my physical machine is about 503Hz, which would be an
       approximately 2ms delay.  */
    SetClock(333);

    InitializeCPU();
    InitializeRAM();
    SampleProgram();

    while (!run)
        {
            cpu = GetCPU();

            fprintf(stdout, "%X\n", cpu.O_REG);
            run = ExecuteProgram();
        }

    if (run == -1)
        return run;

    return 0;
}
