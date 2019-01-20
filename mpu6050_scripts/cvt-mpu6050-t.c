#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
	if (argc != 3) return 1;
	unsigned H = strtol(argv[1], NULL, 0);
	unsigned L = strtol(argv[2], NULL, 0);
	int W = (signed short)((H << 8) | (L & 0xFF));
	float T = W * (1.0/340.0) + 36.53;
	printf("%0.2f\n", T);
	return 0;
}

