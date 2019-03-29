/* sap1.c - source file for the body of the SAP1 virtual machine.  */

#include <stdlib.h>
#include <time.h>

#include "sap1.h"

/* Simple macros to keep me sane and prevent typing so much.  */
#define CPU(A) working_cpu.A
#define WAIT(A) cpu_wait(A)

/* The working CPU variable is the structure that holds all of the registers
   for the execution of CPU instructions.  */
static struct CPU working_cpu;
/* The virtual machine's RAM variable.  */
static unsigned char RAM[PROG];
/* The clock speed in milliseconds; defaults to a 200ms delay for a 5Hz
   clock pulse.  */
static int CLK = 200;

static void cpu_wait(int);

/* These prototypes are for the various functions that emulate the
   instructions given to the CPU.  */
static int ex_nop(void);
static int ex_lda(void);
static int ex_add(void);
static int ex_sub(void);
static int ex_sta(void);
static int ex_ldi(void);
static int ex_jmp(void);
static int ex_jc(void);
static int ex_jz(void);
static int ex_out(void);
static int ex_hlt(void);

/* Sets the clock speed in milliseconds.  */
void
SetClock(milli)
    int milli;
{
    CLK = milli;
}

/* Returns the current CPU structure.  */
struct CPU
GetCPU(void)
{
    return working_cpu;
}

/* Returns a pointer to space allocated to be a VM state.  This is explicitly
   for saving states to file.  */
struct VM *
GetVMState(void)
{
    int i = 0;

    struct VM *vm = calloc(1, sizeof(struct VM));
    MEMCHECK(vm);

    struct CPU *cpu = calloc(1, sizeof(struct CPU));
    MEMCHECK(cpu);

    cpu->M_REG = CPU(M_REG);
    cpu->P_CNT = CPU(P_CNT);
    cpu->A_REG = CPU(A_REG);
    cpu->B_REG = CPU(B_REG);
    cpu->O_REG = CPU(O_REG);

    cpu->F_REG.CF = CPU(F_REG.CF);
    cpu->F_REG.ZF = CPU(F_REG.ZF);

    cpu->I_REG.INS = CPU(I_REG.INS);
    cpu->I_REG.OPR = CPU(I_REG.OPR);

    cpu->BUS = CPU(BUS);

    vm->cpu = cpu;

    for (i = 0; i < PROG; i++)
        vm->ram[i] = RAM[i];

    return vm;

error:
    if (vm)
        free(vm);

    return NULL;
}

/* Cleans up the VM by freeing all allocated memory.  This must be called to
   clean up memory after every time GetVMState is used.  */
void
CleanVM(vm)
    struct VM *vm;
{
    if (vm->cpu)
        free(vm->cpu);

    free(vm);
}

/* Initializes all of the CPU's registers and other elements to zero.  */
void
InitializeCPU(void)
{
    CPU(A_REG) = 0x00;
    CPU(B_REG) = 0x00;
    CPU(O_REG) = 0x00;

    CPU(I_REG.INS) = 0x0;
    CPU(I_REG.OPR) = 0x0;

    CPU(F_REG.CF) = 0;
    CPU(F_REG.ZF) = 0;

    CPU(M_REG) = 0x0;
    CPU(P_CNT) = 0x0;

    CPU(BUS) = 0x00;
}

/* Initializes all the values in RAM to zero.  */
void
InitializeRAM(void)
{
    int i = 0;

    for (i = 0; i < PROG; i++)
        RAM[i] = 0;
}

/* Builds a sample program for CPU execution.  */
void
SampleProgram(void)
{
    RAM[0x0] = ASM(LDA, 0xF);
    RAM[0x1] = ASM(ADD, 0xE);
    RAM[0x2] = ASM(STA, 0xF);
    RAM[0x3] = ASM(OUT, 0x0);
    RAM[0x4] = ASM(JZ,  0x6);
    RAM[0x5] = ASM(JMP, 0x0);
    RAM[0x6] = ASM(HLT, 0x0);
    RAM[0x7] = 0x00;
    RAM[0x8] = 0x00;
    RAM[0x9] = 0x00;
    RAM[0xA] = 0x00;
    RAM[0xB] = 0x00;
    RAM[0xC] = 0x00;
    RAM[0xD] = 0x00;
    RAM[0xE] = 0x01;
    RAM[0xF] = 0xFE;
}

/* Loads the program in the program array into the RAM variable.  If the
   given length of the program is longer than the maximum allowed program
   length (PROG), then only the maximum amount of commands are added to the
   RAM variable.  */
void
LoadProgram(program, length)
    unsigned char program[];
    int length;
{
    int max = 0, i = 0;

    if (length < PROG)
        max = length;
    else
        max = PROG;

    for (i = 0; i < max; i++)
        RAM[i] = program[i];
}

/* Saves the program in the RAM variable to the program array.  If the
   given length of the program is longer than the maximum allowed program
   length (PROG), then only the maximum amount of commands are added to the
   program array.  */
void
SaveProgram(program, length)
    unsigned char program[];
    int length;
{
    int max = 0, i = 0;

    if (length < PROG)
        max = length;
    else
        max = PROG;

    for (i = 0; i < max; i++)
        program[i] = RAM[i];
}

/* Executes the program that is currently resident in the RAM variable.  This
   function returns a 0 if the program is still executing, returns a 1 if the
   program is completed (HLT received), or returns a -1 if the given
   instruction is not an available option (currently there are four
   instructions that have not been implemented, so if any of these four are
   passed it will return a -1).  */
int
ExecuteProgram(void)
{
    CPU(BUS) = CPU(P_CNT);
    CPU(M_REG) = CPU(BUS);

    WAIT(CLK);

    CPU(BUS) = RAM[CPU(M_REG)];
    CPU(I_REG.INS) = CPU(BUS) >> 4;
    CPU(I_REG.OPR) = CPU(BUS) & 0xF;
    CPU(P_CNT)++;

    WAIT(CLK);

    if (CPU(I_REG.INS) == NOP)
        return ex_nop();

    if (CPU(I_REG.INS) == LDA)
        return ex_lda();

    if (CPU(I_REG.INS) == ADD)
        return ex_add();

    if (CPU(I_REG.INS) == SUB)
        return ex_sub();

    if (CPU(I_REG.INS) == STA)
        return ex_sta();

    if (CPU(I_REG.INS) == LDI)
        return ex_ldi();

    if (CPU(I_REG.INS) == JMP)
        return ex_jmp();

    if (CPU(I_REG.INS) == JC)
        return ex_jc();

    if (CPU(I_REG.INS) == JZ)
        return ex_jz();

    if (CPU(I_REG.INS) == OUT)
        return ex_out();

    if (CPU(I_REG.INS) == HLT)
        return ex_hlt();

    return -1;
}

/* Simple delay function that waits the given amount of milliseconds.  */
static void
cpu_wait(milli)
    int milli;
{
    long int pause;
    clock_t delay;

    pause = milli * (CLOCKS_PER_SEC / 1000);
    delay = clock();

    while (clock() - delay < pause)
        ;
}

/* The NOP instruction does nothing for the remainder of the current CPU
   cycle.  This is currently handled by delaying for the time it takes three
   microcode commands to execute.  This function returns a 0 if
   successful.  */
static int
ex_nop(void)
{
    int i = 0;

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The LDA instruction loads the current value of RAM located at the M_REG
   index into the A_REG.  This function returns a 0 if successful.  */
static int
ex_lda(void)
{
    int i = 0;

    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(M_REG) = CPU(BUS) & 0xF;

    WAIT(CLK);

    CPU(BUS) = RAM[CPU(M_REG)];
    CPU(A_REG) = CPU(BUS) & 0xFF;

    for (i = 0; i < 2; i++)
        WAIT(CLK);

    return 0;
}

/* The ADD instruction loads the current value of RAM located at the M_REG
   index into the B register, then adds the value if the A register to the
   B register and loads the sum into the A register.  The carry and zero
   flags are set by this instruction.  This function returns a 0 if
   successful.  TODO: Rewrite addition to mimic the way the physical machine
   handles it.  */
static int
ex_add(void)
{
    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(M_REG) = CPU(BUS) & 0xF;

    WAIT(CLK);

    CPU(BUS) = RAM[CPU(M_REG)];
    CPU(B_REG) = CPU(BUS) & 0xFF;

    WAIT(CLK);

    CPU(BUS) = CPU(A_REG) + CPU(B_REG);
    CPU(A_REG) = CPU(BUS) & 0xFF;
    CPU(F_REG.CF) = CPU(BUS) >> 8;
    CPU(F_REG.ZF) = (CPU(A_REG) == 0);

    WAIT(CLK);

    return 0;
}

/* The SUB instruction loads the current value of RAM located at the M_REG
   index into the B register, then subtracts the value of the B register from
   the value in the A register and loads the difference into the A register.
   The carry and zero flags are set by this instruction.  This function
   returns a 0 if successful.  TODO: Rewrite subtraction to mimic the way
   the physical machine handles it.  */
static int
ex_sub(void)
{
    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(M_REG) = CPU(BUS) & 0xF;

    WAIT(CLK);

    CPU(BUS) = RAM[CPU(M_REG)];
    CPU(B_REG) = CPU(BUS) & 0xFF;

    WAIT(CLK);

    CPU(BUS) = CPU(A_REG) - CPU(B_REG);
    CPU(A_REG) = CPU(BUS) & 0xFF;
    CPU(F_REG.CF) = CPU(BUS) >> 8;
    CPU(F_REG.ZF) = (CPU(A_REG) == 0);

    WAIT(CLK);

    return 0;
}

/* The STA instruction stores the contents of the A register into RAM at the
   M_REG index.  This function returns a 0 if successful.  */
static int
ex_sta(void)
{
    int i = 0;

    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(M_REG) = CPU(BUS) & 0xF;

    WAIT(CLK);

    CPU(BUS) = CPU(A_REG);
    RAM[CPU(M_REG)] = CPU(BUS) & 0xFF;

    for (i = 0; i < 2; i++)
        WAIT(CLK);

    return 0;
}

/* The LDI instruction loads the contents of the instruction register into the
   A register.  This function returns a value of 0 if successful.  */
static int
ex_ldi(void)
{
    int i = 0;

    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(A_REG) = CPU(BUS) & 0xFF;

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The JMP instruction unconditionally loads the contents of the instruction
   register into the program counter.  This function returns a value of 0 if
   successful.  */
static int
ex_jmp(void)
{
    int i = 0;

    CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
    CPU(P_CNT) = CPU(BUS) & 0xF;

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The JC instruction checks if the carry flag is set and loads the contents
   of the instruction register into the program counter if the carry flag is
   set.  This function returns a 0 if successful.  */
static int
ex_jc(void)
{
    int i = 0;

    if (CPU(F_REG.CF))
        {
            CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
            CPU(P_CNT) = CPU(BUS) & 0xF;
            CPU(F_REG.CF) = 0;
        }

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The JZ instruction checks if the zero flag is set and loads the contents
   of the instruction register into the program counter if the zero flag is
   set.  This function returns a 0 if successful.  */
static int
ex_jz(void)
{
    int i = 0;

    if (CPU(F_REG.ZF))
        {
            CPU(BUS) = (CPU(I_REG.INS) << 4) | CPU(I_REG.OPR);
            CPU(P_CNT) = CPU(BUS) & 0xF;
            CPU(F_REG.ZF) = 0;
        }

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The OUT instruction puts the value in the A register into the output
   register.  The output register is the only register that has a value
   displayed.  This function returns a 0 if successful.  */
static int
ex_out(void)
{
    int i = 0;

    CPU(BUS) = CPU(A_REG);
    CPU(O_REG) = CPU(BUS) & 0xFF;

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 0;
}

/* The HLT instruction halts the CPU from executing any more instructions and
   would also halt the clock from ticking if it were implemented in the same
   way as the physical machine.  This function returns a 1 if successful.  */
static int
ex_hlt(void)
{
    int i = 0;

    for (i = 0; i < 3; i++)
        WAIT(CLK);

    return 1;
}
