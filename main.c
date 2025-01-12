#include<stdio.h>
#include<assert.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<strings.h>
#include<math.h>

typedef uint16_t mem_t[2 << 15];

#define ARR_COUNT(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX_LINE_SIZE 100
#define MAX_FILE_SIZE 2048
#define MAX_TOKEN_COUNT 10
#define MAX_TOKEN_SIZE 10

enum op{
  op_add,
  op_sub,
  op_mov,
  op_and,
  op_or,
  op_xor,
  op_inv,
};

struct cpu_state{
  uint16_t a;
  uint16_t d;
  uint16_t _a;
  uint16_t pc;
  uint16_t R;
};

bool is_meaningful_char(char _char){
  return _char == 59 || _char == 44;
}

bool is_numeric_str(char* str, int charc){
  for(int i = 0; i < charc; i ++){
    if((str[i] < 47 || str[i] > 57) && str[i] != 42 && str[i] != 45){
      return false;
    }
  }
  return true;
}

bool is_alpha_str(char* str, int charc){
  for(int i = 0; i < charc; i ++){
    if(str[i] < 65 || str[i] > 122 || (str[i] > 90 && str[i] < 97)){
      //printf("[DEBUG][is_alpha_str] Returning false for %s\n charc: %d", str, charc);
      return false;
    }
  }
  return true;
}
void dump_cpu_state(struct cpu_state const* cpu_state){
  printf("a: %d\n", cpu_state->a);
  printf("d: %d\n", cpu_state->d);
  printf("_a: %d\n", cpu_state->_a);
  printf("pc: %d\n", cpu_state->pc);
  printf("R: %d\n", cpu_state->R);
}
void unreachable(){
  printf("unreachable\n");
}
void todo(){
  printf("todo\n");
}

void print_tokens(char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE]){
  for(int i = 0; i < MAX_TOKEN_COUNT; i ++){
    if(strlen(tokens[i]) <= 0){
      return;
    }
    printf("[INFO][print_tokens] TOKEN[%d]: %s\n", i, tokens[i]);
  }
}

void convertToBinary16(uint16_t num, char out[16]){
  for(int i = 15; i >= 0; i --){
    out[15 - i] = (num >> i) % 2;
  }
}

void printToBinary16(uint16_t num){
  printf("0b");
  for(int i = 15; i >= 0; i --){
    printf("%d", (num >> i) % 2);
  }
  printf("\n");
}

#define ALU_INST (1 << 15)

#define SRC_A_S (1 << 12)

#define DEST_A (1 << 10)
#define DEST_D (1 << 9)
#define DEST_S (1 << 8)

#define OP_FLAG_U (1 << 7)
#define OP_FLAG_P0 (1 << 6)
#define OP_FLAG_P1 (1 << 5)
#define OP_FLAG_ZX (1 << 4)
#define OP_FLAG_SW (1 << 3)

#define JMP_LE (1 << 2)
#define JMP_EQ (1 << 1)
#define JMP_GR (1 << 0)

void exec_instruction(struct cpu_state* cpu_state, mem_t mem, uint16_t inst) {

  printf("[INFO][exec_instruction]:");
  printToBinary16(inst);
  if (~inst & ALU_INST) {
    printf("[INFO][exec_instruction]: executing data instruction\n");
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
    printf("[INFO][exec_instruction] Arithmetic unit\n");
    if(inst & OP_FLAG_P0) v2 = 1;
    if(inst & OP_FLAG_P1){
      result = v1 - v2;
    }else{
      result = v1 + v2;
    }
  }else{
    //LU
    printf("[INFO][exec_instruction] Logic unit\n");
    printf("[INFO][exec_instruction] (inst & OP_FLAG_P0): %d (inst & OP_FLAG_P0): %d", (inst & OP_FLAG_P0), (inst & OP_FLAG_P1) );
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

//Gets line, returns TOKENS
void parse_line(char line[MAX_LINE_SIZE], char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE]){
  int token_ind = -1;
  int in_token_ind = 0;

  for(int i = 0; i < MAX_LINE_SIZE; i ++){
    if(is_alpha_str(&(line[i]), 1) || is_numeric_str(&(line[i]), 1) || is_meaningful_char(line[i])){
      if(line[i] == ';'){
        token_ind ++;
        in_token_ind = 0;
        tokens[token_ind][in_token_ind] = line[i];
      }
      if(i > 0 && (is_alpha_str(&(line[i - 1]), 1) || is_numeric_str(&(line[i - 1]), 1))){
        tokens[token_ind][in_token_ind] = line[i];
        in_token_ind ++;
      }else{
        token_ind ++;
        tokens[token_ind][in_token_ind] = line[i];
        in_token_ind = 1;
      }
    }else{
      in_token_ind = 0;
    }
  }
}

void assemble_line(char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE], char ins_out[16]){

  print_tokens(tokens);

  if(!strcasecmp(tokens[0], "LOAD")){
    int number_to_load = atoi(tokens[1]);
    printf("[INFO][assemble_line] number_to_load: %d\n", number_to_load);
    ins_out[0] = '0';
    for(int i = 0; i <= 14; i ++){
      ins_out[i + 1] = (number_to_load >> (15 - i)) % 2 == 0 ? '0' : '1';
    }
    return;
  }else{

    uint16_t inst = ALU_INST;
    uint8_t dest_start = 0;
    
    //Assign the 5 ALU bits
    if(!strcasecmp(tokens[0], "MOV")){
      char* src = tokens[1];
      if(is_numeric_str(src, strlen(src))){
        int src_num = atoi(src);
        if(src_num < -1 || src_num > 1){
          printf("[ERROR][assemble_line] in MOV src should be REG or -1, 0, 1");
          exit(1);
        }else if(src_num == 0){
          inst = ALU_INST;
        }else if(src_num == 1){ inst = inst | OP_FLAG_ZX | OP_FLAG_U | OP_FLAG_P0;
        }else if(src_num == -1){ inst = inst | OP_FLAG_ZX | OP_FLAG_U | OP_FLAG_P0 | OP_FLAG_P1;
        }else unreachable();
      }else{
        if(src[0] == 'A'){ inst = inst | OP_FLAG_SW | OP_FLAG_ZX;
        }else if(src[0] == 'D'){ inst = inst | OP_FLAG_ZX;
        }else if(src[0] == '*'){ inst = inst | OP_FLAG_ZX | OP_FLAG_SW | SRC_A_S;
        }else unreachable();
      }
      dest_start = 2;
    }else if(!strcasecmp(tokens[0], "ADD")){
      todo();
    }else if(!strcasecmp(tokens[0], "SUB")){
      todo();
    }else if(!strcasecmp(tokens[0], "XOR")){
      todo();
    }else if(!strcasecmp(tokens[0], "AND")){
      todo();
    }else if(!strcasecmp(tokens[0], "OR")){
      todo();
    }else if(!strcasecmp(tokens[0], "INV")){
      todo();
    }else{
      unreachable();
    }

    //Assign the destination bits and JC
    bool has_jump = false;
    for(int i = dest_start; i < MAX_TOKEN_COUNT; i ++){
      if(tokens[i][0] == 'A'){
        inst = inst | DEST_A;
      }else if(tokens[i][0] == 'D'){
        inst = inst | DEST_D;
      }else if(tokens[i][0] == '*'){
        inst = inst | DEST_S;
      }else if(tokens[i][0] == ';'){
        //Next token must be a jump
        if(i + 1 >= MAX_TOKEN_COUNT){
          printf("[ERROR][assemble_line][setting jump condition] Incorrect ussage: after ; you must specify Jump condition");
          exit(1);
        }

        char* jc = tokens[i + 1];
        if(!strcasecmp(jc, "JMP")){ inst = inst | JMP_EQ | JMP_LE | JMP_GR;
        }else if(!strcasecmp(jc, "JEQ")){ inst = inst | JMP_EQ;
        }else if(!strcasecmp(jc, "JLE")){ inst = inst | JMP_EQ | JMP_LE;
        }else if(!strcasecmp(jc, "JGE")){ inst = inst | JMP_EQ | JMP_GR;
        }else if(!strcasecmp(jc, "JLT")){ inst = inst | JMP_LE;
        }else if(!strcasecmp(jc, "JGT")){ inst = inst | JMP_GR;
        }else{
          printf("[ERROR][assemble_line][setting jump condition] Incorrect ussage: after ; you must specify a valid Jump condition, current: %s\n", jc);
          exit(1);
        }
      }
    }
    printToBinary16(inst);
  }
}

void assemble(char* file_path, char* out_path){
  FILE *fptr;
  FILE *outfptr;
  
  outfptr = fopen(out_path, "w");
  fptr = fopen(file_path, "r");
  char assm[MAX_FILE_SIZE][MAX_LINE_SIZE];

  int line_count = 0;
  while(fgets(assm[line_count], MAX_LINE_SIZE, fptr)){

    char tokens[MAX_TOKEN_COUNT][MAX_TOKEN_SIZE] = {};
    parse_line(assm[line_count], tokens);
   
    char out[16] = {};
    assemble_line(tokens, out);

    printf("[INFO][assemble] assembled line: %.*s\n", 16, out);
    fprintf(outfptr, "%.*s\n", 16, out);

    line_count ++;
  }
  line_count --;

  printf("Line count: %d\n", line_count);
}

int main(void){

  struct cpu_state cpu_state = {};
  mem_t mem = {};

  /*printToBinary16(ALU_INST);
  exec_instruction(&cpu_state, mem, 0b0001000000000000);
  exec_instruction(&cpu_state, mem, ALU_INST | DEST_D | OP_FLAG_SW | OP_FLAG_ZX | OP_FLAG_U);
  exec_instruction(&cpu_state, mem, ALU_INST | DEST_A | O_FLAG_U);
  */

  //dump_cpu_state(&cpu_state);
  //
  
  assemble("prog.asm", "out");

  return 0;
}
