#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "operations.h"
#include "stack.h"
#include "lines.h"

#define MAX_LABEL_LENGTH 64

//helper
bool bfrcmp(char *buffer, char *instruction) 
{
	return strncmp(buffer, instruction, strlen(instruction)) == 0;
}

//add to stack
void push(int value)
{
	if (stack_top >= STACK_SIZE - 1) {
		fprintf(stderr, "Stack overflow\n");
		exit(1);
	}
	stack[++stack_top] = value;
}

//remove from stack
int pull()
{
	if (stack_top < 0) {
		fprintf(stderr, "Stack underflow\n");
		exit(1);
	}
	return stack[stack_top--];
}

//pull and add twice
void dup()
{
	int a = pull();
	push(a);
	push(a);
}

//pull two and push sum
void add()
{
	int a = pull();
	int b = pull();
	push(a+b);
}

//pull two and push difference
void sub()
{
	int a = pull();
	int b = pull();
	push(b-a);
}

//pull two and push product
void mul()
{
	int a = pull();
	int b = pull();
	push(a*b);
}

//pull two and push quotient?
void dvd()
{
	int a = pull();
	int b = pull();
	if (a == 0) {
		fprintf(stderr, "Division by zero\n");
		exit(1);
	}
	push(b/a);
}

//pull top and print
void say()
{
	printf("%d\n", pull());
}

//end
void end()
{
	exit(0);
}

void jump_if_zero(FILE *fptr, char *buffer, int line)
{
	char jump_label[MAX_LABEL_LENGTH];
	if (sscanf(buffer, "JUMP_IF_ZERO %s", jump_label) != 1) {
		fprintf(stderr, "Bad jump label at line %d", line);
		exit(1);
	}
	int top = pull();
	if (top != 0) {
		push(top);
		return;
	}
	for (size_t i = 0; i < lines.count; ++i) {
		if (!bfrcmp(lines.data[i].label, jump_label)) continue;
		fseek(fptr, lines.data[i].byte_offset, SEEK_SET);
	}
}

void jump(FILE *fptr, char *buffer, int line)
{
	char jump_label[MAX_LABEL_LENGTH];
	if (sscanf(buffer, "JUMP %s", jump_label) != 1) {
		fprintf(stderr, "Bad jump label at line %d", line);
		exit(1);
	}
	for (size_t i = 0; i < lines.count; ++i) {
		if (!bfrcmp(lines.data[i].label, jump_label)) continue;
		fseek(fptr, lines.data[i].byte_offset, SEEK_SET);
	}
}

void store(unsigned int address)
{
	ram[address] = pull();
}

void load(unsigned int address)
{
	push(ram[address]);
}

void swap()
{
	int a = pull();
	int b = pull();
	push(a);
	push(b);
}

void mod()
{
	int a = pull();
	int b = pull();
	if (a == 0) {
		fprintf(stderr, "Division by zero\n");
		exit(1);
	}
	push (b%a);
}
