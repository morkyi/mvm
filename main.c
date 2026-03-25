#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "dynamic_arrays.h"
#include "operations.h"
#include "stack.h"
#include "lines.h"

#define MAX_LINE_LENGTH  128
#define MAX_LABEL_LENGTH 64

bool debug = false;
bool b_show_machine = false;
bool b_pause_output = false;

void find_labels(FILE *fptr)
{
	int line_number = 0;
	long current_pos;
	if (debug) printf("[DEBUG] Parsing for labels\n");
	char buffer[MAX_LINE_LENGTH];
	char name[MAX_LABEL_LENGTH]; 
	while (fgets(buffer, MAX_LINE_LENGTH, fptr)) {
		++line_number;
		buffer[strcspn(buffer, "\n")] = '\0';
		buffer[strcspn(buffer, ";")] = '\0';
		
		if (!bfrcmp(buffer, "LABEL")) continue;
		if (sscanf(buffer, "LABEL %s", name) != 1) {
			fprintf(stderr, "Invalid label at line %d\n",
    				line_number);
			exit(1);
		}
		Line line = {strdup(name), ftell(fptr)};
		lines_append(lines, line);
		if (!debug) continue; 
		printf("[DEBUG] LABEL %s @ line %u\n", 
	 		line.label, line_number);
	}
}

int get_push_number(char *buffer)
{
	int number;
	sscanf(buffer, "PUSH %d", &number);
	return number;
}

int get_store_number(char *buffer)
{
	unsigned int number;
	sscanf(buffer, "STORE %d", &number);
	if (debug) printf("[DEBUG] \"%s\" storing to 0x%d\n", buffer, number);
	return number;
}

int get_load_number(char *buffer)
{
	unsigned int number;
	sscanf(buffer, "LOAD %d", &number);
	if (debug) printf("[DEBUG] \"%s\" loading from 0x%d\n", 
		   				buffer, number);
	return number;
}

void zero_initialize_ram()
{
	for (size_t i = 0; i < RAM_SIZE; ++i) 
		ram[i] = 0;
}

void show_machine()
{
	printf(" ------- \n");
	printf("| STACK |\n");
	printf(" ------- \n");
	for (size_t i = 0; i < STACK_SIZE; ++i)
		printf("%d, ", stack[i]);
	printf("\n");
	printf("\n");
	printf(" ----- \n");
	printf("| RAM |\n");
	printf(" ----- \n");
	for (size_t i = 0; i < RAM_SIZE; ++i) 
		printf("0x%d = %d, ", i, ram[i]);
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
	if (!b_pause_output) printf("\033[2J\033[H");
}

void pause_output()
{
	getchar();
}

int main(int argc, char **argv)
{
	zero_initialize_ram();
	char *filename = NULL;

	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "-file") == 0)
			if (i+1 < argc) filename = argv[i+1];
		if (strcmp(argv[i], "-debug") == 0)
			debug = true;
		if (strcmp(argv[i], "-show_machine") == 0)
			b_show_machine = true;
		if (strcmp(argv[i], "-pause_output") == 0)
			b_pause_output = true;
	}

	if (filename == NULL) {
		fprintf(stderr, "No file specified, use -file <filename>\n");
		return 1;
	}

	FILE *fptr;
	fptr = fopen(filename, "r");

	if (fptr == NULL) {
		printf("Failed to open file");
		return 1;
	}

	char buffer[MAX_LINE_LENGTH];

	find_labels(fptr);
	rewind(fptr);

	if (debug) printf("[DEBUG] Now reading file\n"); 
	
	int line = 0;
	while (fgets(buffer, MAX_LINE_LENGTH, fptr)) {
		++line;
		buffer[strcspn(buffer, "\n")] = '\0';
		buffer[strcspn(buffer, ";")] = '\0';
		// strip leading whitespace
		while (buffer[0] == ' ' || buffer[0] == '\t')
    			memmove(buffer, buffer+1, strlen(buffer));
		// skip empty lines and comments
		if (buffer[0] == '\0') continue;  
		
		if (debug) printf("[DEBUG] \"%s\"\n", buffer);
		if (bfrcmp(buffer, "PUSH")) push(get_push_number(buffer));
		else if (bfrcmp(buffer, "PULL")) 
			pull();
		else if (bfrcmp(buffer, "DUP")) 
			dup();
		else if (bfrcmp(buffer, "ADD")) 
			add();
		else if (bfrcmp(buffer, "SUB")) 
			sub();
		else if (bfrcmp(buffer, "MUL")) 
			mul();
		else if (bfrcmp(buffer, "DVD")) 
			dvd();
		else if (bfrcmp(buffer, "SAY")) 
			say();
		else if (bfrcmp(buffer, "END")) 
			end();
		else if (bfrcmp(buffer, "JUMP_IF_ZERO")) 
			jump_if_zero(fptr, buffer, line);
		else if (bfrcmp(buffer, "JUMP")) 
			jump(fptr, buffer, line); 
		else if (bfrcmp(buffer, "STORE")) 
			store(get_store_number(buffer)); 
		else if (bfrcmp(buffer, "LOAD")) 
			load(get_load_number(buffer));  
		else if (bfrcmp(buffer, "SWAP")) 
			swap();
		else if (bfrcmp(buffer, "MOD")) 
			mod();
		/*to prevent unknown instruction errors*/	
		else if (bfrcmp(buffer, "LABEL")) goto skip_error;

		else fprintf(stderr, 
	       		"Unknown instruction at line %d: %s\n", line, buffer);         
		skip_error:
		if (b_show_machine) show_machine(); 
		if (b_pause_output) pause_output();
	}

	fclose(fptr);
	lines_destroy(lines);
}
