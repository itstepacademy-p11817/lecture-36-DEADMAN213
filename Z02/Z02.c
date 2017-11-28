#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <string.h>

#define MAX 256
#define MENUPT 7

enum {
	OPENFL = 1,
	PRINTFLS,
	CREATEMAS,
	PRINTMAS,
	CODEFL,
	DECODEFL,
	EXIT
};

const char* menuPoints[] = {
	"Меню:\n",
	"Открыть файл.\n",
	"Печать файлов.\n",
	"Создать массив.\n",
	"Печать массива.\n",
	"Кодировать файл.\n",
	"Декодировать файл.\n",
	"Выход\n"
};

uint8_t menu() {
	uint8_t ch = 0;
	system("cls");

	printf(menuPoints[0]);
	for (uint8_t i = 1; i <= MENUPT; i++) {
		printf("%hhu - %s", i, menuPoints[i]);
	}
	printf("Ваш выбор: ");
	scanf("%hhu", &ch);

	return ch;
}

void pass() {
	while (getchar() != '\n');
}

char* createLine(FILE* fl) {
	char* line = NULL;
	char ch = 0;
	uint16_t ln = 0;
	uint16_t ind = 0;

	while (!feof(fl)) {
		ch = getc(fl);

		if (ln == ind) {
			ln++;
			line = (char*)realloc(line, (ln *= 2) * sizeof(char));
		}

		if (ch == '\n' || ch == EOF) {
			if (ind == 0 && ch == EOF) {
				free(line);
				line = NULL;
				break;
			}
			else if (ind == 0 && ch == '\n') {
				line = (char*)realloc(line, sizeof(char));
				*line = '\n';
			}

			*(line + ind) = '\0';
			line = (char*)realloc(line, (ind + 1) * sizeof(char));
			break;
		}
		else {
			*(line + ind++) = ch;
		}
	}

	return line;
}

char* initMas() {
	uint8_t* codeMas = (char*)calloc(MAX, sizeof(char));

	printf("Введите пароль: ");
	char* passwrd = createLine(stdin);
	uint8_t passLen = strlen(passwrd);
	uint16_t sum = 0;

	for (uint8_t i = 0; i < passLen; i++) {
		sum += *(passwrd + i);
	}

	*codeMas = sum % MAX;
	for (uint16_t i = 1; i < MAX; i++) {
		codeMas[i] = (char)((codeMas[i - 1] + passwrd[i%passLen]) % MAX);
	}

	return codeMas;
}

void printFile(FILE* fl) {
	char ch = 0;
	int32_t pos = ftell(fl);

	fseek(fl, 0, SEEK_SET);
	while ((ch = getc(fl)) != EOF) {
		putc(ch, stdout);
	}
	putc('\n', stdout);
	fseek(fl, pos, SEEK_SET);
}

void swapMas(char* mas) {
	for (uint16_t i = 0; i < MAX; i++) {
		uint8_t ind = rand() % 256;
		uint8_t temp = mas[ind];
		mas[ind] = mas[i];
		mas[i] = temp;
	}
}

char searchSimbol(char* mas, char s) {
	for (uint16_t i = 0; i < MAX; i++) {
		if (mas[i] == s) {
			return (char)i;
		}
	}
}

int main(void) {
	setlocale(LC_ALL, "Russian");
	srand(time(NULL));

	char* codeMas = NULL;

	FILE* fin = NULL;
	FILE* fout = NULL;

	char* finName = NULL;
	char* foutName = NULL;

	uint8_t ch = 0;

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case OPENFL:
			pass();
			if (fin) {
				fclose(fin);
			}
			if (finName) {
				free(finName);
			}

			printf("Введите имя входного файла: ");
			finName = createLine(stdin);

			if ((fin = fopen(finName, "r")) == NULL) {
				printf("Ошибка при открытии файла.\n");
			}
			else {
				printf("Файл открыт.\n");
			}

			break;

		case PRINTFLS:
			if (fin) {
				fseek(fin, 0, SEEK_SET);
				printf("Входной файл:\n");
				printFile(fin);
			}

			if (fout) {
				fseek(fout, 0, SEEK_SET);
				printf("Выходной файл:\n");
				printFile(fout);
			}

			break;

		case CREATEMAS:
			pass();
			codeMas = initMas();
			break;

		case PRINTMAS:
			for (uint16_t i = 0; i < MAX; i++) {
				printf("%c", codeMas[i]);
			}
			putc('\n', stdout);
			break;

		case CODEFL:
			pass();
			if (fout) {
				fclose(fout);
			}
			if (foutName) {
				free(foutName);
			}

			printf("Введите имя выходного файла: ");
			foutName = createLine(stdin);

			if ((fout = fopen(foutName, "w+")) == NULL) {
				printf("Ошибка при открытии файла.\n");
			}
			else {
				printf("Файл открыт.\n");
			}

			if (fin && fout) {
				fseek(fin, 0, SEEK_SET);
				char ch = 0;

				do {
					if ((ch = getc(fin)) != EOF) {
						putc(codeMas[ch], fout);
						fflush(fin);
					}
				} while (!feof(fin));
			}

			break;

		case DECODEFL:
			if (fout) {
				char ch = 0;
				fseek(fout, 0, SEEK_SET);

				while ((ch = getc(fout)) != EOF) {
					printf("%c", searchSimbol(codeMas, ch));
				}
				putc('\n', stdout);
			}
			break;

		default:
			printf("Неверный ввод.\n");
			pass();
			break;
		}
		printf("Нажмите любую клавишу.\n");
		_getch();
	}

	if (fin) {
		fclose(fin);
	}
	if (fout) {
		fclose(fout);
	}

	if (finName) {
		free(finName);
	}
	if (foutName) {
		free(foutName);
	}

	if (codeMas) {
		free(codeMas);
	}

	return 0;
}