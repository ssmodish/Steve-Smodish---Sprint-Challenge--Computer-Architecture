#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_LEN 6

unsigned char cpu_ram_read(struct cpu *cpu, unsigned char MAR)
{
  return cpu->ram[MAR];
}

void cpu_ram_write(struct cpu *cpu, unsigned char MAR, unsigned char MDR)
{
  cpu->ram[MAR] = MDR;
  return;
}

void trace(struct cpu *cpu)
{
  printf("PC:%02X | ", cpu->PC);

  printf("%02X %02X %02X |",
         cpu_ram_read(cpu, cpu->PC),
         cpu_ram_read(cpu, cpu->PC + 1),
         cpu_ram_read(cpu, cpu->PC + 2));

  for (int i = 0; i < 8; i++)
  {
    printf("R[%d]:%02X | ", i, cpu->registers[i]);
  }

  printf("FL:%02X", cpu->FL);

  printf("\n");
}
/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, int argc, char **argv)
{

  if (argc != 2)
  {
    printf("Correct usage: ./files file_name.extension\n");
    return;
  }

  FILE *fp;
  char line[MEM_SIZE];

  fp = fopen(argv[1], "r");

  if (fp == NULL)
  {
    printf("Error opening file.\n");
    return;
  }

  int address = 0;

  while (fgets(line, MEM_SIZE, fp) != NULL)
  {
    char *endptr;

    unsigned char val = strtoul(line, &endptr, 2);

    if (line == endptr)
    {
      //         printf("skipping: %s", line);
      continue;
    }

    cpu->ram[address++] = val;
  }
  // fclose(fp);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    cpu->registers[0] = cpu->registers[regA] * cpu->registers[regB];
    break;

  case ALU_ADD:
    cpu->registers[0] = cpu->registers[regA] + cpu->registers[regB];
    break;

  case ALU_CMP:
    /* FL bits: 00000LGE */
    if (cpu->registers[regA] == cpu->registers[regB])
    {
      cpu->FL = 0b00000001;
    }
    else if (cpu->registers[regA] > cpu->registers[regB])
    {
      cpu->FL = 0b00000010;
    }
    else if (cpu->registers[regA] < cpu->registers[regB])
    {
      cpu->FL = 0b00000100;
    }
    printf("ALU_CMP\n");
    trace(cpu);

    break;
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  int running = 1; // True until we get a HLT instruction

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    unsigned char IR = cpu_ram_read(cpu, cpu->PC);
    // 2. Figure out how many operands this next instruction requires
    // 3. Get the appropriate value(s) of the operands following this instruction
    unsigned char operand0 = cpu_ram_read(cpu, cpu->PC + 1);
    unsigned char operand1 = cpu_ram_read(cpu, cpu->PC + 2);

    switch (IR)
    {
    case MUL:
      alu(cpu, ALU_MUL, operand0, operand1);
      cpu->PC += 3;
      break;

    case ADD:
      alu(cpu, ALU_ADD, operand0, operand1);
      cpu->PC += 3;
      break;

    case CMP:
      alu(cpu, ALU_CMP, operand0, operand1);
      cpu->PC += 3;
      break;

    case LDI:
      cpu->registers[operand0] = operand1;
      cpu->PC += 3;
      break;

    case PRN:
      printf("%d\n", cpu->registers[operand0]);
      cpu->PC += 2;
      break;

    case HLT:
      running = 0;
      break;

    case PUSH:
      cpu->registers[7] -= 1;
      cpu->ram[cpu->registers[7]] = cpu->registers[cpu->ram[cpu->PC + 1]];
      cpu->PC += 2;
      break;

    case POP:
      cpu->registers[cpu->ram[cpu->PC + 1]] = cpu->ram[cpu->registers[7]];
      cpu->registers[7] += 1;
      cpu->PC += 2;
      break;

    case CALL:
      cpu->registers[7]--;
      cpu->ram[cpu->registers[7]] = cpu->PC + 2;
      cpu->PC = cpu->registers[cpu->ram[cpu->PC + 1]];
      break;

    case RET:
      cpu->PC = cpu->ram[cpu->registers[7]];
      cpu->registers[7]++;
      break;

    case JMP:
      cpu->PC = cpu->registers[cpu->PC + 1];
      printf("JMP\n");
      trace(cpu);
      break;

    case JEQ:
      if (cpu->FL == 0b00000001)
      {
        cpu->PC = cpu->registers[cpu->PC + 1];
      }
      else
      {
        cpu->PC += 2;
      }
      printf("JEQ\n");
      trace(cpu);
      break;

    case JNE:
      if (cpu->FL != 0b00000001)
      {
        cpu->PC = cpu->registers[cpu->PC + 1];
      }
      else
      {
        cpu->PC += 2;
      }
      printf("JNE\n");

      trace(cpu);
      break;

    default:
      printf("Unknown command 0x%02X at 0x%02X\n", IR, cpu->PC);
      exit(1);
    }
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  // initialize PC to 0
  cpu->PC = 0; // Program Counter
  cpu->FL = 0;
  cpu->SP = 0xF4;

  memset(cpu->registers, 0x00, 7 * sizeof(unsigned char));
  cpu->registers[7] = 0xF4;

  memset(cpu->ram, 0x00, 256 * sizeof(unsigned char));
}
