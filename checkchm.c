#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	FILE* f;
	struct stat fi;
	int buffer;

	if (argc != 2) {
		printf ("Usage: checkchm [file]\n\n");
		return 1;
	}

	stat(argv[1], &fi);

	f = fopen(argv[1], "rb");
	if (!f) {
		return -3;
	}

	fseek(f, 0x68, SEEK_SET);
	fread(&buffer, 4, 1, f);

	if (fi.st_size == buffer) {
		return 0;
	} else {
		return 2;
	}
}
