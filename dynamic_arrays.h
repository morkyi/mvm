#ifndef DYNAMIC_ARRAYS_H
#define DYNAMIC_ARRAYS_H
#include <stdlib.h>

typedef struct {
	char *label;
	long  byte_offset;
} Line;

typedef struct {
	Line  *data;
	size_t capacity;
	size_t count;
} Lines;

#define lines_append(lines, param)\
do {\
	if (lines.count >= lines.capacity) {\
		if (lines.capacity == 0) lines.capacity = 256;\
		else lines.capacity *= 2;\
		lines.data = realloc(lines.data, lines.capacity*sizeof(*lines.data));\
	}\
	lines.data[lines.count++] = param;\
} while (0)

#define lines_destroy(lines)\
do {\
	if (lines.data == NULL)\
		break;\
	free(lines.data);\
} while (0)

#endif
