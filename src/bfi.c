#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#define FREE(x) do { if (x) free(x); } while(0)

FILE* file;
size_t max = 8192;
int idx, depth, args;

// 0-255
unsigned char *tape; 
char *code, balance, current, *pointer;

void *malloc_s(size_t size) {
	void* ptr = malloc(size);
	if (!ptr) {
		exit(EXIT_FAILURE);
	}

	return ptr;
}

void *calloc_s(size_t num, size_t size) {
	void* ptr = calloc(num, size);
	if (!ptr) {
		exit(EXIT_FAILURE);
	}

	return ptr;
}

void *realloc_s(void* ptr, size_t size) {
	void* nptr = realloc(ptr, size);
	if (!nptr) {
		exit(EXIT_FAILURE);
	}

	return nptr;
}

void mem_check(int i) {
	size_t st_idx = (size_t)i;

	if (st_idx >= max) {
		size_t new_max = max;
		while (st_idx >= new_max) {
			new_max += 1024;
		}

		unsigned char* new_tape = realloc_s(tape, new_max);
		memset(new_tape + max, 0, new_max - max);
		tape = new_tape;

		char* new_code = realloc_s(code, new_max);
		code = new_code;

		max = new_max;
	}
}

void interpret(char* c) {
	char* start;
	depth++;

	while (*c) {
		switch (current = 1, *c++) {
		case '<':
			if (idx <= 0) {
				fprintf(stderr, "Error: Pointer below 0\n");
				exit(EXIT_FAILURE);
			}

			idx--;
			break;
		case '>':
			idx++;
			mem_check(idx);

			break;
		case '+':
			tape[idx]++;
			break;
		case '-':
			tape[idx]--;
			break;
		case '.':
			putchar(tape[idx]);
			fflush(stdout);
			break;
		case ',':
			// im not even kidding this is the old implementation
			// tape[idx] = getchar();
			// fflush(stdout);
			// break;

			if (isatty(STDIN_FILENO)) {
				printf(">>> ");
				fflush(stdout);
			}

			int ch = getchar();

			if (ch == EOF) {
				// treat EOF as 0
				tape[idx] = 0;
			} else {
				tape[idx] = (unsigned char)ch;
			}

			break;
		case '[':
			for (balance = 1, start = c; balance && *c; c++) {
				balance += *c == '[';
				balance -= *c == ']';
			}

			if (!balance) {
				c[-1] = 0;

				while (tape[idx]) {
					interpret(start);
				}
				
				c[-1] = ']';
				break;
			}
		case ']':
			fprintf(stderr, "Error: Unbalanced brackets\n");
			exit(EXIT_FAILURE);
		default:
			continue;
		}

		if (idx < 0) {
			fprintf(stderr, "Error: Negative index\n");
			exit(EXIT_FAILURE);
		}

		mem_check(idx);
	}

	depth--;
}

void clean(void) {
	if (file) fclose(file);
	FREE(tape);
	FREE(code);
}


int main(int argc, char* argv[]) {
	// this is amazing
	atexit(clean);

	struct stat st;
	args = argc;

	tape = calloc_s(max, sizeof(char));
	code = malloc_s(max);

	const char* file_path = argv[1];
	if (file_path && stat(file_path, &st) != 0) {
		fprintf(stderr, "Error: Failed to access '%s' : %s\n", file_path, strerror(errno));
		return EXIT_FAILURE;
	}

	file = fopen(file_path, "r");
	if (!file) {
		fprintf(stderr, "Error: Failed to read '%s' : %s\n", file_path, strerror(errno));
		return EXIT_FAILURE;
	}

	pointer = code;
	int tmp;

	while ((tmp = getc(file)) != EOF) {
		// if you get this error then wow
		if ((size_t)(pointer - code) >= max - 1) {
			fprintf(stderr, "Error: Program too big\n");
			exit(EXIT_FAILURE);
		}
		balance = (char)tmp;
		*pointer++ = (char)balance;
	}

	*pointer = 0;
	interpret(code);

	return 0;
}