#ifndef _CPU_H_
#define _CPU_H_
#define MEM_SIZE 256

// Holds all information about the CPU
struct cpu
{
  // PC
  unsigned char PC;
  unsigned char SP;
  //  unsigned char IR;
  //  unsigned char MAR;
  //  unsigned char MDR;
  unsigned char FL;

  // registers (array)
  unsigned char registers[8];
  // ram (array)
  unsigned char ram[MEM_SIZE];
};

// ALU operations
enum alu_op
{
  ALU_MUL,
  ALU_ADD,
  ALU_CMP
};

// Instructions

// These use binary literals. If these aren't available with your compiler, hex
// literals should be used.

#define LDI 0b10000010
#define HLT 0b00000001
#define PRN 0b01000111
#define PUSH 0b01000101
#define POP 0b01000110
#define CALL 0b01010000
#define RET 0b00010001
#define ADD 0b10100000
#define MUL 0b10100010
#define JMP 0b01010100
#define CMP 0b10100111
#define JEQ 0b01010101
#define JNE 0b01010110

// Function declarations

extern void cpu_load(struct cpu *cpu, int argc, char **argv);
extern void cpu_init(struct cpu *cpu);
extern void cpu_run(struct cpu *cpu);

#endif
