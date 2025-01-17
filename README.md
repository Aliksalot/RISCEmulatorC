# RISCEmulatorC
Emulating the [nandgame processor](https://nandgame.com/) with its simplistic instruction set in C.
Gonna modify it a bit ofc.

# What is
- It's a 16 bit processor
- 16 bit Data BUS
- It has 2 16-bit A and D, as well as a program counter

## Implementation is not the best
- First time i do something like this. Tokenization is pretty not good; Its not case sensitive at least
- It works with uint16s, not with int16

## Instructions

The instruction encoding is almost 1:1 as the CPU you create in the [nandgame](https://nandgame.com/).

### ```DUMPC``` and ```DUMPD```

One observation you can make is that the 15th and 14th bit are being unused.
I made them so that:

#### if 15 is high

Processor goes into dump mode and dumps A reg into std out as a DIGIT

If 14 is high as well it tries to dump it as a char from ASCII

## Here are all the Instructions we can use

### Memory

```LOAD <uint16_t>``` - Loads number in A reg

```MOV <src-reg> <A/*A/D dest-regs>``` - Copies value from src-reg to all registers in dest. Example: ```MOV A A D *A```

### Dump

```DUMPC``` - explained above

```DUMPD``` - explained above

### Jumps

#### Jumps look at result from operation of <u>Current instruction</u>. So if you do 
```
ADD A D D
JGE
```
#### It wont look at result from first instruction, but of result of instruction with no params which i think is bitwise ```AND``` of A and D.

#### To have expeted behavior you can do ```MOV D D; JGE```

```JMP``` - Unconditional

```JGE``` - Jump greater or equal

```JGT``` - Jump greater

```JEQ``` - Jump equal

### ALU instructions

#### Starting here you can include a optional jump condition after each instruction as follows:

```<INST> <ARGS>[; <Jump condition>]``` with example: ```ADD A D D; JGE```

#### Instructions

```ADD <src-reg1> <src-reg2> <A/*A/D dest-regs>``` - sums up ```src-reg1``` and ```src-reg2``` and copies the result in ```dest-regs```

```SUB <src-reg1> <src-reg2> <A/*A/D dest-regs>``` - subtracts from ```src-reg1``` ```src-reg2``` and copies the result in ```dest-regs```

```AND <src-reg1> <src-reg2> <A/*A/D dest-regs>``` - performs bitwise and on ```src-reg1``` and ```src-reg2``` and copies the result in ```dest-regs```

```OR <src-reg1> <src-reg2> <A/*A/D dest-regs>``` - performs bitwise and on ```src-reg1``` or ```src-reg2``` and copies the result in ```dest-regs```

```INV <src-reg> <A/*A/D dest-regs>``` - performs bitwise inversion on ```src-reg``` and copies the result in ```dest-regs```

## Compile

Use whatever compiler you want on ```main.c```

## Usage - Assemble

You can assemble using ```./<program> asm <file-path> [dest-path]```

## Usage - Emulate

You can emulate the then assembled file by doing ```./<progra> emulate <file-path>```

### You can do ```./<program> help``` To see the manual






