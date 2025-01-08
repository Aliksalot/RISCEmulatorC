#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include"./parser.c"



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
  printf("Executing ALU operation on %d and %d", v1, v2);
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

  return 0;
}
