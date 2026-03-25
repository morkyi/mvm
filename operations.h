#ifndef OPERATIONS_H
#define OPERATIONS_H
bool bfrcmp(char *buffer, char *instruction);
//real operations:
void push(int value);
int  pull();
void dup();
void add();
void sub();
void mul();
void dvd();
void say();
void end();
void jump(FILE *fptr, char *buffer, int line);
void jump_if_zero(FILE *fptr, char *buffer, int line);
void store(unsigned int address);
void load(unsigned int address);
void swap();
void mod();
#endif
