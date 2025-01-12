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
void dump_cpu_state(struct cpu_state const* cpu_state, mem_t mem){
  printf("a: %d\n", cpu_state->a);
  printf("d: %d\n", cpu_state->d);
  printf("_a: %d\n", mem[cpu_state->a]);
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
    out[15 - i] = ((num >> i) % 2 == 0) ? '0' : '1';
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
#define OP_DUMP (1 << 14)

#define SRC_A_S (1 << 12)

#define OP_FLAG_U (1 << 10)
#define OP_FLAG_P0 (1 << 9)
#define OP_FLAG_P1 (1 << 8)
#define OP_FLAG_ZX (1 << 7)
#define OP_FLAG_SW (1 << 6)

#define DEST_A (1 << 5)
#define DEST_D (1 << 4)
#define DEST_S (1 << 3)

#define JMP_LE (1 << 2)
#define JMP_EQ (1 << 1)
#define JMP_GR (1 << 0)

void exec_instruction(struct cpu_state* cpu_state, mem_t mem, uint16_t inst) {

  //printf("[INFO][exec_instruction]:");
  //printToBinary16(inst);
  if ( inst & OP_DUMP){
    dump_cpu_state(cpu_state, mem);
    return;
  }
  if (~inst & ALU_INST) {
    //printf("[INFO][exec_instruction]: executing data instruction\n");
    cpu_state->a = inst;
    return;
  }

  uint16_t v1 = cpu_state->d;
  uint16_t v2 = cpu_state->a;
  if(inst & SRC_A_S) v2 = mem[cpu_state->a];

  if(inst & OP_FLAG_SW){
    //printf("[INFO][exec_instruction]: Perfoming swap\n");
    uint16_t temp = v1;
    v1 = v2;
    v2 = temp;
  }

  if(inst & OP_FLAG_ZX){
    //printf("[INFO][exec_instruction]: Perfoming zero\n");
    v1 = 0;
  }

  uint16_t result;

  //printf("[INFO][exec_instruction] U_FLAG=%d\n", inst & OP_FLAG_U);
  if(inst & OP_FLAG_U){
    //AU
    //printf("[INFO][exec_instruction] Arithmetic unit\n");
    if(inst & OP_FLAG_P0) v2 = 1;
    if(inst & OP_FLAG_P1){
      result = v1 - v2;
    }else{
      result = v1 + v2;
    }
  }else{
    //LU
    //printf("[INFO][exec_instruction] Logic unit\n");
    //printf("[INFO][exec_instruction] (inst & OP_FLAG_P0): %d (inst & OP_FLAG_P0): %d\n", (inst & OP_FLAG_P0), (inst & OP_FLAG_P1) );
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
      ins_out[i + 1] = (number_to_load >> (14 - i)) % 2 == 0 ? '0' : '1';
    }
    return;
  }else if(!strcasecmp(tokens[0], "DUMP")){
    uint16_t inst = OP_DUMP;
    convertToBinary16(inst, ins_out);
    return;
  }else{

    uint16_t inst = ALU_INST;
    uint8_t dest_start = 0;
    
    //Assign the 5 ALU bits
    if(!strcasecmp(tokens[0], "MOV")){
      printf("DOING A MOV\n");
      char* src = tokens[1];
            if(src[0] == 'A'){ inst = inst | OP_FLAG_ZX | OP_FLAG_U;; printf("SRC A\n");
      }else if(src[0] == 'D'){ inst = inst | OP_FLAG_ZX | OP_FLAG_SW | OP_FLAG_U; printf("SRC D\n");
      }else if(src[0] == '*'){ inst = inst | OP_FLAG_ZX | OP_FLAG_U  | SRC_A_S;; printf("SRC *A\n");
      }else if(src[0] == '1'){ inst = inst | OP_FLAG_U  | OP_FLAG_P0 | OP_FLAG_ZX;; printf("SRC 1\n");
      }else if(src[0] == '-'){ inst = inst | OP_FLAG_U  | OP_FLAG_P0 | OP_FLAG_P1 | OP_FLAG_ZX;; printf("SRC -1\n");
      }else if(src[0] == '0'){ inst = inst | OP_FLAG_ZX;printf("SRC 0\n");
      }else {
        unreachable();
        printf("Couldnt interpret MOV src\n");
      }

      dest_start = 2;
    }else if(!strcasecmp(tokens[0], "ADD")){
      printf("DOING A ADD\n");
      if(
        ((tokens[1][0] == 'A' || tokens[1][0] == '*') && tokens[2][0] == 'D') ||
        ((tokens[2][0] == 'A' || tokens[2][0] == '*') && tokens[1][0] == 'D')
      ){
        if(tokens[1][0] == '*' || tokens[2][0] == '*') inst = inst | SRC_A_S;
        inst = inst | OP_FLAG_U;
      }else if(
        ((tokens[1][0] == 'A' || tokens[1][0] == '*') && tokens[2][0] == '1') ||
        ((tokens[2][0] == 'A' || tokens[2][0] == '*') && tokens[1][0] == '1')
      ){
        if(tokens[1][0] == '*' || tokens[2][0] == '*') inst = inst | SRC_A_S;
        inst = inst | OP_FLAG_U | OP_FLAG_P0 | OP_FLAG_SW;
      }else if(
        (tokens[1][0] == 'D' && tokens[2][0] == '1') ||
        (tokens[2][0] == 'D' && tokens[1][0] == '1')
      ){
        inst = inst | OP_FLAG_U | OP_FLAG_P0;
      }else{ printf("Invalid addition"); exit(1); }
      dest_start = 3;
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
      if(tokens[i][0] == 'A'){ inst = inst | DEST_A;
      }else if(tokens[i][0]== 'D'){ inst = inst | DEST_D;
      }else if(tokens[i][0] == '*'){ inst = inst | DEST_S;
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

    
    convertToBinary16(inst, ins_out);
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

uint16_t inst_to_uint16(char inst[16]){
  uint16_t result = 0;
  
  for(int i = 15; i >= 0; i --){
    result = result | (inst[i] == '1' ? (1 << (15-i)) : 0);
  }
  return result;
}


void run_program_from_file(char const* file_path){
  FILE* fptr;
  struct cpu_state cpu_state = {};
  mem_t mem = {};

  char program[MAX_FILE_SIZE][MAX_LINE_SIZE] = {};
  fptr = fopen(file_path, "r");
  int linec = 0;
  while(fgets(program[linec], MAX_LINE_SIZE, fptr)){
    //printf("%s\n",program[linec]);
    exec_instruction(&cpu_state, mem, inst_to_uint16(program[linec]));
    linec ++;
  }
 
  
}

void usage(){
  printf("Usage: \n");
  printf("    emulate [file_path] - Acccepts a assembled instruction file to emalate\n");
  printf("    asm [file_path] <output_path> - Acccepts a .asm file to assemble\n");
  printf("    help - Displays usage menu\n");
  printf("    asm-man - Displays manual for how does the assembly for this CPU work\n");
}

int main(int argc, char* argv[]){

  if(argc < 2){
    printf("Please provide arguments\n");
    usage();
  }else{
    if(!strcmp(argv[1],"emulate")){
      if(argc < 3){
        printf("Please provide file path!\n");
        exit(1);
      }
      char* fpath = argv[2];
      run_program_from_file(fpath);
    }else if(!strcmp(argv[1],"asm")){
      if(argc < 3){
        printf("Please provide file path!\n");
        exit(1);
      }

      char* fpath = argv[2];
      if(argc == 4){
        char* outpath = argv[2];
        assemble(fpath, outpath);
      }else{
        assemble(fpath, "out");
      }


    }else if(!strcmp(argv[1],"help")){
      usage();
    }else if(!strcmp(argv[1],"asm-man")){
      printf("Not implemented");
    }else{ printf("Unknown argument %s\n", argv[1]); usage(); }
  }

  return 0;
}
