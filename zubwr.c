#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

struct State {
	int16_t a;
	int16_t d;
	uint16_t pc;
};

#define CPU_INST     (1 << 15)
#define CPU_MEM      (1 << 12)
#define CPU_SW       (1 <<  7)
#define CPU_ZX       (1 <<  6)
#define CPU_DEST_A   (1 <<  5)
#define CPU_DEST_D   (1 <<  4)
#define CPU_DEST_MEM (1 <<  3)
#define CPU_JLT      (1 <<  2)
#define CPU_JEQ      (1 <<  1)
#define CPU_JGT      (1 <<  0)
#define CPU_JMP (CPU_JLT | CPU_JEQ | CPU_JGT)
#define CPU_OP_LSB 8

enum {
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_INV,
	OP_ADD,
	OP_SUB,
	OP_INC,
	OP_DEC,
};

void execute(struct State *cpu, int16_t *mem, uint16_t inst) {
	if (~inst & CPU_INST) {
		cpu->a = inst;
		cpu->pc += 1;
		return;
	}

	int16_t x = cpu->d;
	int16_t y = cpu->a;
	if (inst & CPU_MEM) y = mem[cpu->a];
	if (inst & CPU_SW) {
		int16_t tmp = x;
		x = y;
		y = tmp;
	}
	if (inst & CPU_ZX) x = 0;

	int16_t result;
	switch (inst >> CPU_OP_LSB & 7) {
		case OP_AND: result = x & y; break;
		case OP_OR:  result = x | y; break;
		case OP_XOR: result = x ^ y; break;
		case OP_INV: result = ~x;    break;
		case OP_ADD: result = x + y; break;
		case OP_SUB: result = x - y; break;
		case OP_INC: result = x + 1; break;
		case OP_DEC: result = x - 1; break;
	}

	bool jmp = false;
	if (inst & CPU_JLT) jmp = jmp || result  < 0;
	if (inst & CPU_JEQ) jmp = jmp || result == 0;
	if (inst & CPU_JGT) jmp = jmp || result  > 0;

	if (jmp) cpu->pc = cpu->a;
	else     cpu->pc += 1;

	if (inst & CPU_DEST_MEM) mem[cpu->a] = result;
	if (inst & CPU_DEST_A) cpu->a = result;
	if (inst & CPU_DEST_D) cpu->d = result;
}

int main() {
	struct State cpu = {};
	int16_t mem[1 << 15] = {};
	uint16_t program[] = {
		CPU_INST | CPU_DEST_D | OP_INC << CPU_OP_LSB,
		CPU_INST | CPU_JMP,
	};

	for (size_t i = 0; i < 100; i += 1) {
		execute(&cpu, mem, program[cpu.pc]);
		printf("A = %04uhx\tD = %04uhx\tPC=%04uhx\n", cpu.a, cpu.d, cpu.pc);
	}
}
