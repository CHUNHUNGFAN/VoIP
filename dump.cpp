#define _CRT_SECURE_NO_WARNINGS
#define BUFSIZE 4
#include <fstream>
#include <iostream>
#include <cmath>
using std::cin;
using std::cout;
using std::endl;

int print_hex(unsigned char buffer[BUFSIZE], size_t n)
{
	static size_t offset = 0;
	size_t i;
	unsigned char c;
	printf("%06lx  ", offset);
	offset += 16;

	for (i = 0; i<n; i++) {
		printf("%02x ", buffer[i]);
		if (i == 7)
			printf("  ");
	}
	if (n < BUFSIZE)
	for (i = n; i<BUFSIZE; i++)
		printf("   ");
	if (n < 8)
		printf("  ");

	for (i = 0; i<n; i++) {
		c = buffer[i];
		if (32 <= c && c <= 126){
			printf("%c", c);
		}
		else
			printf(".");
	}
	printf("\n");
	return 0;    // total bytes dumped
}

int main(int argc, char* argv[])
{
	unsigned char buffer[BUFSIZE];
	size_t length, i, nRecord;

	char fn[20] = "a.out";
	if (argc == 2)
		strcpy(fn, argv[1]);

	FILE *fp = fopen(fn, "rb");
	fseek(fp, 0L, SEEK_END);
	length = ftell(fp);
	rewind(fp);
	nRecord = length / BUFSIZE;
	if (length % BUFSIZE != 0)
		++nRecord;
	for (i = 0; i<18; i++) {
		fread(buffer, BUFSIZE, 1, fp);
		if (i < nRecord - 1)
			print_hex(buffer, BUFSIZE);
		else // last record
			print_hex(buffer, length % BUFSIZE);
	}
	fclose(fp);
	return 0;
}
