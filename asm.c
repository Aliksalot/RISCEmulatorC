#include<stdio.h>

#define ARR_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef uint16_t mem_t[2 << 15];
typedef uint16_t (*alu_operation_t)(uint16_t, uint16_t);

enum op{
  op_noop,
  op_plus,
  op_minus,
  op_or,
  op_xor,
  op_and,
  op_not,
  op_load,
  op_mov,
};
enum reg{
  reg_noreg,
  reg_a,
  reg_d,
  reg_pc,
  reg__a,
};
enum jmp_c{
  jmp_c_no,
  jmp_c_gt,
  jmp_c_lt,
  jmp_c_le,
  jmp_c_ge,
  jmp_c_eq,
  jmp_c_jm,
};
struct Instruction{
  enum jmp_c jc;
  enum op op;
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
  printf("unreachable\n");
}
void todo(){
  printf("todo\n");
}

void usage(){
  printf("Usage:\n");
  printf("    ngasm [file-path] <output-file-path> -> assembles straight to machine code for ngemul\n");
}

int main(int argc, char** argv) {
  
  usage();
  return 0;
}

