#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

typedef uint16_t mem_t[2 << 15];
typedef uint16_t (*alu_operation_t)(uint16_t, uint16_t);

#define ARR_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

enum op_code{
  op_noop,
  op_load,
  op_or,
  op_xor,
  op_and,
  op_not,
  op_mov,
  op_plus,
  op_minus,
};
enum jc{
  jc_jmp,
  jc_eq,
  jc_lt,
  jc_gt,
  jc_le,
  jc_ge,
};
enum reg{
  reg_noreg,
  reg_a,
  reg_d,
  reg__a,
  reg_pc,
};
struct Instruction{
  enum op_code op;
  enum jc jc;
  enum reg src[2];
  enum reg dest[3];
  bool is_const_load;
  uint16_t constant;
};
struct cpu_state{
  uint16_t a;
  uint16_t d;
  uint16_t _a;
  uint16_t pc;
  uint16_t R;
};
void unreachable(){
  printf("unreachable");
}
void todo(){
  printf("todo");
}
void dump_instruction(struct Instruction const* ins){
  printf("jc: %d; ", ins->jc);
  printf("op: %d; ", ins->op);
  printf("src0: %d. ", ins->src[0]);
  printf("src1: %d. ", ins->src[1]);
  printf("dest0: %d. ", ins->dest[0]);
  printf("dest1: %d. ", ins->dest[1]);
  printf("dest2: %d\n", ins->dest[2]);
}

void dump_cpu_state(struct cpu_state const* cpu_state){
  printf("a: %d\n", cpu_state->a);
  printf("d: %d\n", cpu_state->d);
  printf("_a: %d\n", cpu_state->_a);
  printf("pc: %d\n", cpu_state->pc);
  printf("R: %d\n", cpu_state->R);
}

uint16_t add(uint16_t v1, uint16_t v2){ return v1 + v2; }
uint16_t sub(uint16_t v1, uint16_t v2){ return v1 - v2; }
uint16_t or(uint16_t v1, uint16_t v2){ return v1 | v2; }
uint16_t xor(uint16_t v1, uint16_t v2){ return v1 ^ v2; }
uint16_t and(uint16_t v1, uint16_t v2){ return v1 & v2; }
uint16_t inv(uint16_t v1, uint16_t v2){ return ~v1; }

void load_value_in_reg(
  struct cpu_state* cpu_state,
  mem_t mem,
  enum reg const* dest,
  uint16_t value_to_load
){
  switch(*dest){
    case reg_a: cpu_state->a = value_to_load; break;
    case reg_d: cpu_state->d = value_to_load; break;
    case reg_pc: cpu_state->pc = value_to_load; break;
    case reg__a: mem[cpu_state->a] = value_to_load; break;
    case reg_noreg: break;
  }
}
//Returns R that needs to be loaded
uint16_t get_value_at_reg(
  struct cpu_state const* cpu_state,
  mem_t mem,
  enum reg const* src
){
  switch(*src){
    case reg_a: return cpu_state->a;
    case reg_d: return cpu_state->d;
    case reg_pc: return cpu_state->pc;
    case reg__a: return mem[cpu_state->a];
    case reg_noreg: return 0;
  }
}
void load_result_in_R(struct cpu_state* cpu_state, uint16_t result){
  cpu_state->R = result;
}
void load_value_in_multiple_reg(
  struct cpu_state* cpu_state,
  mem_t mem,
  enum reg const dest[],
  uint16_t value_to_load
){
  for(uint16_t i = 0; i < ARR_COUNT(&dest); i ++){
    printf("INFO: Loading %d in register %d\n", value_to_load, dest[i]);
    load_value_in_reg(cpu_state, mem, &dest[i], value_to_load);
  }
}
void execute_alu_instruction(
  struct cpu_state *cpu_state,
  struct Instruction const* ins,
  mem_t mem,
  enum reg const* src,
  alu_operation_t op
){
  uint16_t v1 = get_value_at_reg(cpu_state, mem, &src[0]);
  uint16_t v2 = get_value_at_reg(cpu_state, mem, &src[1]);
  printf("Executing ALU operation on %d and %d\n", v1, v2);
  uint16_t result = op(v1, v2);
  load_value_in_multiple_reg(cpu_state, mem, ins->dest, result);
  load_result_in_R(cpu_state, result);
}

void execute_instruction(
    struct Instruction const* ins,
    struct cpu_state* cpu_state,
    mem_t mem
){
  printf("INFO: executing instruction\n");
  dump_instruction(ins);
  if(ins->is_const_load){
  printf("INFO: loading constant %d in reg_a\n", ins->constant);
    for(int i = 0; i < ARR_COUNT(ins->dest); i ++){
      switch(ins->dest[i]){
        case reg_a: cpu_state->a = ins->constant; break;
        case reg_d: cpu_state->d = ins->constant; break;
        case reg__a: cpu_state->_a = ins->constant; break;
        case reg_pc:  break;
        case reg_noreg: break;

        default: unreachable();
      }
    }
    load_result_in_R(cpu_state, ins->constant);
    return;
  }
  switch(ins->op){
    case op_load: {
                    todo();
                    break;
    }
    case op_plus: execute_alu_instruction(cpu_state, ins, mem, ins->src, add); break; 
    case op_minus: execute_alu_instruction(cpu_state, ins, mem, ins->src, sub); break; 
    case op_or: execute_alu_instruction(cpu_state, ins, mem, ins->src, or); break; 
    case op_xor: execute_alu_instruction(cpu_state, ins, mem, ins->src, xor); break; 
    case op_and: execute_alu_instruction(cpu_state, ins, mem, ins->src, and); break; 
    case op_not: execute_alu_instruction(cpu_state, ins, mem, ins->src, inv); break; 
    case op_mov: {
      uint16_t value_to_load = get_value_at_reg(cpu_state, mem, &(ins->src[0]));
      enum reg _dest = { reg_a };
      load_value_in_multiple_reg(cpu_state, mem, ins->dest , value_to_load);
      load_result_in_R(cpu_state, value_to_load);
      break;
    };
    default: unreachable();
  }
}

int main(void){
  
  struct cpu_state cpu_state = {};
  mem_t mem = {};

  struct Instruction ins = { .op=op_load, .is_const_load=1, .constant=34, .dest={reg_a} };
  execute_instruction(&ins, &cpu_state, mem);

  struct Instruction ins1 = { .op=op_mov, .src={reg_a}, .dest={reg_d} };
  execute_instruction(&ins1, &cpu_state, mem);

  struct Instruction ins2 = { .op=op_load, .is_const_load=1, .constant=35, .dest={reg_a} };
  execute_instruction(&ins2, &cpu_state, mem);

  struct Instruction ins3 = { .op=op_plus, .src={reg_a, reg_d}, .dest={reg_d, reg_a} };
  execute_instruction(&ins3, &cpu_state, mem);

  dump_cpu_state(&cpu_state);
  return 0;
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

void (struct cpu_state* cpu_state, mem_t mem, uint16_t inst) {
  if (~inst & INST) {
    state->a = inst;
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

  jmp = todo();
}
