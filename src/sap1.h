/* sap1.h - main header file for the SAP1 virtual machine.

   Primary source for definitions and prototypes for use throughout the
   implementation of the SAP1 VM.  */

#ifndef DCP_SAP_H
#define DCP_SAP_H

#include <stdio.h>


#define MEMCHECK(A) if (!(A)) {\
    fprintf(stderr, "Memory Error.\n"); goto error; }

#define ASM(I, O) ((I) << 4) | (O)

/* The maximum program length of the physical SAP1 machine is 16 bytes.  */
#define PROG 16

/* Instructions NI0 through NI4 are currently not implemented and will result
   in an error if used.  They are included as a filler for now, and may be
   implemented at a later date.  */
enum INSTRUCTIONS
{
    NOP, LDA, ADD, SUB, STA, LDI, JMP, JC,
    JZ, NI0, NI1, NI2, NI3, NI4, OUT, HLT
};

/* The SAP1 only has two flags, the carry flag (CF), and the zero flag(ZF).
   They are each a 1-bit value stored in the flags register.  */
struct FLAGS
{
    unsigned char CF : 1;
    unsigned char ZF : 1;
};

/* The instruction register is an 8-bit register that holds the instructions
   in the upper 4 bits and the operands in the lower 4 bits.  */
struct INS_REG
{
    unsigned char INS : 4;
    unsigned char OPR : 4;
};

struct CPU
{
    unsigned char M_REG : 4;
    unsigned char P_CNT : 4;
    unsigned char A_REG, B_REG, O_REG;
    struct INS_REG I_REG;
    struct FLAGS F_REG;
    unsigned short BUS : 9;
};

struct VM
{
    struct CPU *cpu;
    unsigned int ram[PROG];
};

void SetClock(int);
void InitializeCPU(void);
struct CPU GetCPU(void);
struct VM *GetVMState(void);
void CleanVM(struct VM *);
void InitializeRAM(void);
void SampleProgram(void);
void LoadProgram(unsigned char[], int);
void SaveProgram(unsigned char[], int);
int ExecuteProgram(void);

#endif /* DCP_SAP_H */
