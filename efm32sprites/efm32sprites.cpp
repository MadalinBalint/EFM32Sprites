// efm32sprites.cpp : Defines the entry point for the console application.
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

int fsize(char *filename)
{
	FILE *file;
	errno_t err;
	int size;

	err = fopen_s(&file, filename, "rb");
	if (err == 0) {
		int prev = ftell(file);
		fseek(file, 0L, SEEK_END);
		size = ftell(file);
		fseek(file, prev, SEEK_SET); //go back to where we were
		fclose(file);
		return size;
	}

	return 0;
}

char *uppercase(char *&s) {
	int size = strlen(s);
	char *t = (char *)malloc(size + 1);
	for (int i = 0; i < size; i++)
		t[i] = toupper(s[i]);
	t[size] = 0;

	return t;
}

int main(int argc, char *argv[])
{
	if (argc != 5) {
		printf("EFM32SPRITES rawimage name width height\n");
		getchar();
		exit(0);
	}

	char *filename = argv[1];
	char *name = argv[2];
	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	 

	if ((filename == NULL) || (name == NULL)) {
		printf("Eroare la parametrii rawimage sau name\n");
		getchar();
		exit(0);
	}

	if ((width <= 0) || (height <= 0)) {
		printf("Eroare la parametrii width sau height\n");
		getchar();
		exit(0);
	}

	printf("Procesam fisierul '%s' cu rezolutia de %dx%d pixeli\n", filename, width, height);

	int size = fsize(filename);

	if (size <= 0) {
		printf("Fisierul %s are o dimensiune gresita\n", filename);
		getchar();
		exit(0);
	}

	FILE *file, *fileh, *filec;
	errno_t err;
	char *buffer = (char *)malloc(width * height);

	int powers[8] = { 1, 2, 4, 8, 16, 32, 64, 128 };

	char fisier_header[128], fisier_c[128];
	strcpy_s(fisier_header, name);
	strcat_s(fisier_header, ".h");

	strcpy_s(fisier_c, name);
	strcat_s(fisier_c, ".c");

	err = fopen_s(&fileh, fisier_header, "w");
	if (err != 0) {
		printf("Eroare la deschiderea fisierului .H %s\n", fisier_header);
		getchar();
		exit(0);
	}

	err = fopen_s(&filec, fisier_c, "w");
	if (err != 0) {
		printf("Eroare la deschiderea fisierului .C %s\n", fisier_c);
		getchar();
		exit(0);
	}

	int x = 0;
	err = fopen_s(&file, filename, "rb");
	if (err == 0) {
		fread(buffer, 1, size, file);
		fclose(file);		

		int b;
		
		/* Scriem fisierul .C */
		fprintf_s(filec, "#include <stdint.h>\n\n");
		fprintf_s(filec, "const uint8_t %s[%d] = { ", name, width * height / 8);

		for (int i = 0; i < width * height; i += 8) {
				int p = 0;
				for (int k = 0; k < 8; k++) {
					b = buffer[i + k];
					if (b != 0) b = 1;
					p += b * powers[k];
				}
				fprintf_s(filec, "%d", p);
				if (i < width * height - 8) fprintf_s(filec, ", ");
				x++;
			
		}
		fprintf_s(filec, " };\n");

		/* Scriem fisierul .H */
		fprintf_s(fileh, "#ifndef %s\n", uppercase(name));
		fprintf_s(fileh, "#define %s\n\n", uppercase(name));

		fprintf_s(fileh, "#include <stdint.h>\n\n");
		fprintf_s(fileh, "#ifdef __cplusplus\nextern \"C\" {\n#endif\n\n");

		fprintf_s(fileh, "#define %s_XSIZE %d\n", uppercase(name), width);
		fprintf_s(fileh, "#define %s_YSIZE %d\n", uppercase(name), height);
		fprintf_s(fileh, "extern const uint8_t %s[%d]; \n", name, width * height / 8);

		fprintf_s(fileh, "\n#ifdef __cplusplus\n}\n#endif\n\n");
		fprintf_s(fileh, "#endif\n");
	} else {
		printf("Eroare la deschiderea fisierului %s\n", filename);
		getchar();
		exit(0);
	  }

	fclose(fileh);
	fclose(filec);

	printf("Pixels: %d\n", x);

	getchar();

    return 0;
}