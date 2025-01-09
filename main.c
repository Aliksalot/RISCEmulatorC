#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

typedef uint16_t mem_t[2 << 15];

#define ARR_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

struct cpu_state{
  uint16_t a;
  uint16_t d;
  uint16_t _a;
  uint16_t pc;
  uint16_t R;
  
};
void dump_cpu_state(struct cpu_state const* cpu_state){
  printf("a: %d\n", cpu_state->a);
  printf("d: %d\n", cpu_state->d);
  printf("_a: %d\n", cpu_state->_a);
  printf("pc: %d\n", cpu_state->pc);
  printf("R: %d\n", cpu_state->R);
}
void unreachable(){
  printf("unreachable");
}
void todo(){
  printf("todo");
}

#define INST (1 << 15)

#define SRC_A_S (1 << 12)

#define DEST_A (1 << 10)
#define DEST_D (1 << 9)
#define DEST_S (1 << 8)

#define OP_FLAG_U (1 << 7)
#define OP_FLAG_P0 (1 << 6)
#define OP_FLAG_P1 (1 << 5)
#define OP_FLAG_ZX (1 << 4)
#define OP_FLAG_SW (1 << 3)

void f(struct cpu_state* cpu_state, mem_t mem, uint16_t inst) {

  if (~inst & INST) {
    cpu_state->a = inst;
    return;
  }

  uint16_t v1 = cpu_state->d;
  uint16_t v2 = cpu_state->a;
  if(inst & SRC_A_S) v2 = mem[cpu_state->a];

  if(inst & OP_FLAG_SW){
    uint16_t temp = v1;
    v1 = v2;
    v2 = temp;
  }

  if(inst & OP_FLAG_ZX){
    v2 = 0;
  }

  uint16_t result;

  if(inst & OP_FLAG_U){
    //AU
    if(inst & OP_FLAG_P0) v2 = 1;
    if(inst & OP_FLAG_P1){
      result = v1 - v2;
    }else{
      result = v1 + v2;
    }
  }else{
    //LU
    if((inst & OP_FLAG_P0) && (inst & OP_FLAG_P1)) result = ~v1;
    if(!(inst & OP_FLAG_P0) && (inst & OP_FLAG_P1)) result = v1 ^ v2;
    if((inst & OP_FLAG_P0) && !(inst & OP_FLAG_P1)) result = v1 | v2;
    if(!(inst & OP_FLAG_P0) && !(inst & OP_FLAG_P1)) result = v1 & v2;
  }

  if (inst & DEST_A) cpu_state->a = result;
  if (inst & DEST_D) cpu_state->d = result;
  if (inst & DEST_S) mem[cpu_state->a] = result;

  cpu_state->R = result;

  //jmp
  todo();
}

int main(void){

  struct cpu_state cpu_state = {};
  mem_t mem = {};

  f(&cpu_state, mem, 0b0111111111111111);
  f(&cpu_state, mem, 0b1 | DEST_D | OP_FLAG_SW | OP_FLAG_ZX);

  dump_cpu_state(&cpu_state);
  return 0;
}
