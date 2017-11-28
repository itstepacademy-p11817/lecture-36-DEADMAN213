#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <conio.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MENU 12

//------------------------------------------------------------------------------------------------------------------
enum {
	ADD_WORKER = 1,
	SAVE_TO_FILE,
	LOAD_FROM_FILE,
	PRINT_WORKERS,
	EDIT_WORKER,
	FIND_WORKERS,
	FIND_AGE,
	FIND_EXPR,
	FIND_WAGE,
	DELETE_WORKER,
	DELETE_WORKERS,
	EXIT
};

const char* menuPoints[] = {
	"Меню:\n",
	"Добавить работника.\n",
	"Сохранить базу в файл.\n",
	"Загрузить базу из файла.\n",
	"Распечатать базу.\n",
	"Редактировать работника.\n",
	"Найти работника по фамилии.\n",
	"Найти работника по возрасту.\n",
	"Найти работника по опыту.\n",
	"Найти работника по зарплате.\n",
	"Удалить работника.\n",
	"Удалить базу.\n",
	"Выход.\n",
};

typedef struct {
	char* lname;
	char* fname;
	char* position;

	uint8_t age;
	uint8_t expr;
	uint16_t wage;
} Coworker;
//------------------------------------------------------------------------------------------------------------------

uint8_t menu() {
	uint8_t ch = 0;
	system("cls");

	printf(menuPoints[0]);
	for (uint8_t i = 1; i <= MENU; i++) {
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

		if (ch == '\n' || ch == EOF || ch == '\0') {
			if (ind == 0) {
				free(line);
				line = NULL;
				break;
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
//------------------------------------------------------------------------------------------------------------------

Coworker* createWorker() {
	Coworker* wrk = NULL;
	char* line = NULL;

	printf("Введите фамилию работника или нажмите \"Ввод\" для выхода: ");
	while (line = createLine(stdin)) {
		wrk = (Coworker*)calloc(1, sizeof(Coworker));
		wrk->lname = line;

		printf("Введите имя работника: ");
		line = createLine(stdin);
		wrk->fname = line;

		printf("Введите должность работника: ");
		line = createLine(stdin);
		wrk->position = line;

		printf("Введите возраст работника: ");
		scanf("%hhu", &(wrk->age));

		printf("Введите стаж работника: ");
		scanf("%hhu", &(wrk->expr));

		printf("Введите зарплату работника: ");
		scanf("%hu", &(wrk->wage));
	}

	return wrk;
}

void printWorker(Coworker* wrk) {
	printf("\nФамилия: ");
	printf("%s.\n", wrk->lname);

	printf("Имя: ");
	printf("%s.\n", wrk->fname);

	printf("Должность: ");
	printf("%s.\n", wrk->position);

	printf("Возраст: ");
	printf("%hhu.\n", wrk->age);

	printf("Стаж: ");
	printf("%hhu.\n", wrk->expr);

	printf("Зарплата: ");
	printf("%hu.\n", wrk->wage);
}

void deleteWorker(Coworker* wrk) {
	free(wrk->lname);
	free(wrk->fname);
	free(wrk->position);
	free(wrk);
}

Coworker** deleteWorkers(Coworker** wrk, uint8_t num) {
	for (uint8_t i = 0; i < num; i++) {
		deleteWorker(*(wrk + i));
	}
	free(wrk);

	return NULL;
}

int main(void) {
	setlocale(LC_ALL, "Russian");

	Coworker** workers = NULL;
	uint8_t numWork = 0;

	uint8_t ch = 0;

	while ((ch = menu()) != EXIT) {
		switch (ch) {
		case ADD_WORKER: {
			pass();
			Coworker* wrk = NULL;
			while (wrk = createWorker()) {
				workers = (Coworker**)realloc(workers, (numWork + 1) * sizeof(Coworker*));
				*(workers + numWork++) = wrk;
			}
			break;
		}

		case SAVE_TO_FILE: {
			pass();
			FILE* fout = NULL;

			printf("Введите имя файла: ");
			char* fname = createLine(stdin);

			if (fname == NULL || (fout = fopen(fname, "wb")) == NULL) {
				printf("Ошибка при создании файла.\n");
			}
			else {
				fwrite(&numWork, sizeof(numWork), 1, fout);
				for (uint8_t i = 0; i < numWork; i++) {
					fwrite((*(workers + i))->lname, strlen((*(workers + i))->lname) + 1, 1, fout);
					fwrite((*(workers + i))->fname, strlen((*(workers + i))->fname) + 1, 1, fout);
					fwrite((*(workers + i))->position, strlen((*(workers + i))->position) + 1, 1, fout);

					fwrite(&((*(workers + i))->age), sizeof(uint8_t), 1, fout);
					fwrite(&((*(workers + i))->expr), sizeof(uint8_t), 1, fout);
					fwrite(&((*(workers + i))->wage), sizeof(uint16_t), 1, fout);
				}
				fclose(fout);
				free(fname);
			}

			break;
		}

		case LOAD_FROM_FILE: {
			pass();
			FILE* fin = NULL;

			printf("Введите имя файла: ");
			char* fname = createLine(stdin);

			if (fname == NULL || (fin = fopen(fname, "rb")) == NULL) {
				printf("Ошибка при создании файла.\n");
			}
			else {
				fread(&numWork, sizeof(numWork), 1, fin);
				workers = (Coworker**)realloc(workers, numWork * sizeof(Coworker*));
				for (uint8_t i = 0; i < numWork; i++) {
					*(workers + i) = (Coworker*)calloc(1, sizeof(Coworker));
					(*(workers + i))->lname = createLine(fin);
					(*(workers + i))->fname = createLine(fin);
					(*(workers + i))->position = createLine(fin);

					fread(&((*(workers + i))->age), sizeof(uint8_t), 1, fin);
					fread(&((*(workers + i))->expr), sizeof(uint8_t), 1, fin);
					fread(&((*(workers + i))->wage), sizeof(uint16_t), 1, fin);
				}

				fclose(fin);
				free(fname);
			}

			break;
		}

		case PRINT_WORKERS:
			for (uint8_t i = 0; i < numWork; i++) {
				printWorker(*(workers + i));
			}
			break;

		case FIND_WORKERS: {
			pass();
			printf("Введите фамилию: ");
			char* line = createLine(stdin);

			for (uint8_t i = 0; i < numWork; i++) {
				if (!strcmp(line, (*(workers + i))->lname)) {
					printWorker(*(workers + i));
				}
			}

			break;
		}

		case FIND_AGE: {
			pass();

			uint8_t ageBgn = 0;
			uint8_t ageEnd = 0;

			printf("Введите диапазон поиска через пробел: ");
			scanf("%hhu %hhu", &ageBgn, &ageEnd);

			for (uint8_t i = 0; i < numWork; i++) {
				if ((*(workers + i))->age >= ageBgn && (*(workers + i))->age <= ageEnd) {
					printWorker(*(workers + i));
				}
			}

			break;
		}

		case FIND_EXPR: {
			pass();

			uint8_t exprBgn = 0;
			uint8_t exprEnd = 0;

			printf("Введите диапазон поиска через пробел: ");
			scanf("%hhu %hhu", &exprBgn, &exprEnd);

			for (uint8_t i = 0; i < numWork; i++) {
				if ((*(workers + i))->expr >= exprBgn && (*(workers + i))->expr <= exprEnd) {
					printWorker(*(workers + i));
				}
			}

			break;
		}

		case FIND_WAGE: {
			pass();

			uint16_t wageBgn = 0;
			uint16_t wageEnd = 0;

			printf("Введите диапазон поиска через пробел: ");
			scanf("%hu %hu", &wageBgn, &wageEnd);

			for (uint8_t i = 0; i < numWork; i++) {
				if ((*(workers + i))->wage >= wageBgn && (*(workers + i))->wage <= wageEnd) {
					printWorker(*(workers + i));
				}
			}

			break;
		}

		case EDIT_WORKER: {
			pass();
			printf("Введите фамилию: ");
			char* line = createLine(stdin);

			for (uint8_t i = 0; i < numWork; i++) {
				if (!strcmp(line, (*(workers + i))->lname)) {
					printWorker(*(workers + i));
					putc('\n', stdout);

					free(*(workers + i));
					*(workers + i) = createWorker();
				}
			}

			break;
		}

		case DELETE_WORKER: {
			pass();
			printf("Введите фамилию: ");
			char* line = createLine(stdin);

			for (uint8_t i = 0; i < numWork; ) {
				if (!strcmp(line, (*(workers + i))->lname)) {
					for (uint8_t j = i; j < numWork - 1; j++) {
						*(workers + j) = *(workers + j + 1);
					}
					workers = (Coworker**)realloc(workers, --numWork * sizeof(Coworker*));
				}
				else {
					i++;
				}
			}

			break;
		}

		case DELETE_WORKERS:
			workers = deleteWorkers(workers, numWork);
			numWork = 0;
			break;

		default:
			pass();
			printf("Неверный ввод.\n");
			break;
		}

		printf("Нажмите любую клавишу.\n");
		_getch();
	}

	return 0;
}
