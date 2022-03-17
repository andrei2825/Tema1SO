#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Structura de pereche in care voi tine o cheie si o valoare
typedef struct {
	char *symbol;
	char *mapping;
} pair;

// Functie care verifica daca o cheie se afla in hashMap
pair *hashSearch(pair **hashMap, int size, char *symbol)
{
	int i;
	for (i = 0; i < size; i++)
		if (strcmp(hashMap[i]->symbol, symbol) == 0)
			return hashMap[i];
	return NULL;
}


// Functie care verifica daca un fisier exista
int fileCheck(const char *file)
{
	FILE *f;

	f = fopen(file, "r");
	if (f) {
		fclose(f);
		return 1;
	}
	return 0;
}

// Functie care preia un string, un key si un value si inlocuieste
// in acel string aparitiile key-ului cu valoarea
char *strreplace(char *s, const char *s1, const char *s2)
{
	char *p;
	char *col1;
	char *col2;
	size_t len1;
	size_t len2;
//      pentru cazurile in care key-ul se afla intre "", acesta nu este
//      modificat
	p = strstr(s, s1);
	col1 = strchr(s, '"');
	if (col1 != NULL) {
		col2 = strchr(col1 + 1, '"');
		if (col2 != NULL) {
			if (p != NULL) {
				if (strlen(col1) > strlen(p) &&
				    strlen(p) > strlen(col2)) {
					p = strstr(p + 1, s1);
				}
			}
		}
	}
	if (p != NULL) {
		len1 = strlen(s1);
		len2 = strlen(s2);
		if (len1 != len2)
			memmove(p + len2, p + len1, strlen(p + len1) + 1);
		memcpy(p, s2, len2);
	}
	return s;
}

int main(int argc, char **argv)
{
	FILE *inFile;
	FILE *oFile;
	int index = 0;
	int i;
	int j;
	int tmpCond;
	pair **hashTable = malloc(1000 * sizeof(pair));

	if (hashTable == NULL)
		exit(12);
	char *dir = malloc(1000);

	if (dir == NULL)
		exit(12);
	char *outfile = malloc(1000);

	if (outfile == NULL)
		exit(12);
	char input[256];
	char tmp1[256];
	char tmp2[256];
	char cond[256];
	char tokenD[256];
	char fileName[256];
	char *fielToken;
	char *multiLine;
	char tmpMulti[256];
	char *tmpTok;
	char multilineValue[256] = "";
	char *tok;
	long ifPointer = -1;
	pair *p;
//      cazul in care se primeste input de la stdin
	if (argc == 1) {
		fgets(input, 255, stdin);
		printf("%s", input);
	} else {
//              parcurg fiecare argument primit
		for (i = 1; i < argc; i++) {
//                      verific daca esste un path catre un fisier de intrare,
//                      un fisier de iesire sau o comanda de tip -D
			if (argv[i][0] == '_' &&
			    argv[i][strlen(argv[i]) - 1] == 'n') {
				inFile = fopen(argv[i], "r");
				fgets(input, 255, (FILE *)inFile);
//                              inlocuiest orice valoare definita daca exista
				for (j = 0; j < index; j++) {
					strreplace(input, hashTable[j]->symbol,
						   hashTable[j]->mapping);
				}
				strcpy(tmp1, input);
				tok = strtok(tmp1, " ");
//                              verific daca prima linie din fisier este o comanda
				if (strcmp(tok, "#define") == 0) {
					ifPointer = -1;
//                                      separ linia in key si valoare
					p = malloc(sizeof(pair));
					if (p == NULL)
						exit(12);
					p->symbol = malloc(256);
					if (p->symbol == NULL)
						exit(12);
					strcpy(p->symbol, strtok(NULL, " "));
					p->mapping = malloc(256);
					if (p->mapping == NULL)
						exit(12);
//                                      caz define multiline
					multiLine = strtok(NULL, "\n");
					if (multiLine[strlen(multiLine) - 1] ==
					    '\\') {
						strncat(multilineValue,
							multiLine,
							strlen(multiLine) - 1);
						fgets(input, 255,
						      (FILE *)inFile);
						strcpy(tmpMulti, input);
						multiLine =
						    strtok(tmpMulti, "\t");
						while (
						    multiLine[strlen(
								  multiLine) -
							      2] == '\\') {
							strncat(
							    multilineValue,
							    multiLine,
							    strlen(multiLine) -
								2);
							fgets(input, 255,
							      (FILE *)inFile);
							strcpy(tmpMulti, input);
							multiLine = strtok(
							    tmpMulti, "\t");
						}
						strncat(multilineValue,
							multiLine,
							strlen(multiLine) - 1);
						strcpy(p->mapping,
						       multilineValue);
					} else {
						strcpy(p->mapping, multiLine);
					}
//                                      adaug cheia si valoarea in hashMap
					if (hashSearch(hashTable, index,
						       p->symbol) == NULL) {
						hashTable[index++] = p;
					}
				} else if (strcmp(tok, "#undef") == 0) {
					ifPointer = -1;
//                                      eliberez memoria si sterg din hashMap elementul definit
					free(hashTable[index - 1]->mapping);
					free(hashTable[index - 1]->symbol);
					free(hashTable[index - 1]);
					index--;
				} else if (strcmp(tok, "#if") == 0) {
					ifPointer = -1;
					strcpy(cond, strtok(NULL, " "));
//                                      separ si verific conditia
					if (atoi(cond)) {
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							printf("%s", input);
//                                                      ifPointer va fi pozitia anterioara a cursorului
//                                                      inainte de citirea din fisier. cand nu este nevoie de 
//                                                      acesta, va fi setyat la -1
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					} else {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					}
				} else if (strcmp(input, "#else\n") == 0) {
					ifPointer = -1;
					if (!atoi(cond)) {
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							printf("%s", input);
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					} else {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					}
				} else if (strcmp(tok, "#elif") == 0) {
					ifPointer = -1;
					strcpy(cond, strtok(NULL, " "));
					if (atoi(cond)) {
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							printf("%s", input);
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					} else {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					}
				} else if (strcmp(input, "#endif\n") == 0) {
					ifPointer = -1;
				} else if (strcmp(tok, "#ifdef") == 0) {
					ifPointer = -1;
					strcpy(cond, strtok(NULL, "\n "));
					tmpCond = 0;
					for (j = 0; j < index; j++) {
						if (strcmp(
							hashTable[j]->mapping,
							cond) == 0) {
							tmpCond = 1;
							break;
						}
					}
					if (tmpCond) {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							printf("%s", input);
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					} else {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					}
				} else if (strcmp(tok, "#ifndef") == 0) {
					ifPointer = -1;
					strcpy(cond, strtok(NULL, "\n "));
					tmpCond = 0;
					for (j = 0; j < index; j++) {
						if (strcmp(
							hashTable[j]->mapping,
							cond) == 0) {
							tmpCond = 1;
							break;
						}
					}
					if (!tmpCond) {
						ifPointer = ftell(inFile);
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							printf("%s", input);
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					} else {
						ifPointer = -1;
						fgets(input, 256,
						      (FILE *)inFile);
						while (input[0] != '#') {
							ifPointer =
							    ftell(inFile);
							fgets(input, 256,
							      (FILE *)inFile);
						}
					}
				} else if (strcmp(tok, "#include") == 0) {
//                                      verific daca exista un include si extrag numele fisierului inclus
					ifPointer = -1;
					fielToken = strtok(NULL, "\n ");
					memcpy(fileName, fielToken + 1,
					       strlen(fielToken) - 2);
//                                      daca fisierul nu exista returnez o eroare
					if (!fileCheck(fileName)) {
						fprintf(stderr, "Invalid file");
						exit(1);
					}
				} else {
					ifPointer = -1;
					if (strcmp(input, "\n") != 0)
						printf("%s", input);
				}
//                              repet procesul de mai sus
				while (1) {
					if (ifPointer != -1) {
						fseek(inFile, ifPointer,
						      SEEK_SET);
					}
//                                      ma opresc cand ajung la ned of file sau cand primesc o eroare
					if (!fgets(input, 256,
						   (FILE *)inFile) ||
					    feof(inFile) || ferror(inFile)) {
						break;
					}
					for (j = 0; j < index; j++) {
						strreplace(
						    input, hashTable[j]->symbol,
						    hashTable[j]->mapping);
					}
					strcpy(tmp1, input);
					strcpy(tmp2, input);
					tok = strtok(tmp1, " ");
					if (strcmp(tok, "#define") == 0) {
						ifPointer = -1;
						p = malloc(sizeof(pair));
						if (p == NULL)
							exit(12);
						p->symbol = malloc(256);
						if (p->symbol == NULL)
							exit(12);
						strcpy(p->symbol,
						       strtok(NULL, " "));
						p->mapping = malloc(256);
						if (p->mapping == NULL)
							exit(12);
						strcpy(p->mapping,
						       strtok(NULL, "\n"));
						if (hashSearch(hashTable, index,
							       p->symbol) ==
						    NULL) {
							hashTable[index++] = p;
						}
					} else if (strcmp(tok, "#undef") == 0) {
						ifPointer = -1;
						free(hashTable[index - 1]
							 ->mapping);
						free(hashTable[index - 1]
							 ->symbol);
						free(hashTable[index - 1]);
						index--;
					} else if (strcmp(tok, "#if") == 0) {
						strcpy(cond, strtok(NULL, " "));
						if (atoi(cond)) {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								printf("%s",
								       input);
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						} else {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						}
					} else if (strcmp(input, "#else\n") ==
						   0) {
						if (!atoi(cond)) {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								printf("%s",
								       input);
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						} else {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						}
					} else if (strcmp(tok, "#elif") == 0) {
						strcpy(cond, strtok(NULL, " "));
						if (atoi(cond)) {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								printf("%s",
								       input);
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						} else {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						}
					} else if (strcmp(input, "#endif\n") ==
						   0) {
						ifPointer = -1;
					} else if (strcmp(tok, "#ifdef") == 0) {
						strcpy(cond,
						       strtok(NULL, "\n "));
						tmpCond = 0;
						for (j = 0; j < index; j++) {
							if (strcmp(
								hashTable[j]
								    ->mapping,
								cond) == 0) {
								tmpCond = 1;
								break;
							}
						}
						if (tmpCond) {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								printf("%s",
								       input);
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						} else {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						}
					} else if (strcmp(tok, "#ifndef") ==
						   0) {
						strcpy(cond,
						       strtok(NULL, "\n "));
						tmpCond = 0;
						for (j = 0; j < index; j++) {
							if (strcmp(
								hashTable[j]
								    ->mapping,
								cond) == 0) {
								tmpCond = 1;
								break;
							}
						}
						if (!tmpCond) {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								printf("%s",
								       input);
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						} else {
							fgets(input, 256,
							      (FILE *)inFile);
							while (input[0] !=
							       '#') {
								ifPointer =
								    ftell(
									inFile);
								fgets(
								    input, 256,
								    (FILE *)
									inFile);
							}
						}
					} else {
						ifPointer = -1;
						if (strcmp(input, "\n") != 0)
							printf("%s", input);
					}
				}
				fclose(inFile);
			} else if (argv[i][0] == '_' &&
				   argv[i][strlen(argv[i]) - 1] == 't') {
//                              in cazul unui fisier de output, voi scrie ce primesc la intrare, in fisierul de iesire
				inFile = fopen(argv[i - 1], "r");
				oFile = fopen(argv[i], "w+");
				fgets(input, 255, (FILE *)inFile);
				fputs(input, oFile);
				fclose(inFile);
				fclose(oFile);
			} else if (argv[i][0] == '-') {
//                              verific ce comanda primesc ca parametru
				if (argv[i][1] == 'D') {
//                                      verific daca comanda este sau nu atasata de argumentele sale
					if (strlen(argv[i]) == 2) {
						strcpy(tokenD, argv[i + 1]);
						p = malloc(sizeof(pair));
						if (p == NULL)
							exit(12);
						p->symbol = malloc(256);
						if (p->symbol == NULL)
							exit(12);
						strcpy(p->symbol,
						       strtok(tokenD, "="));
						p->mapping = malloc(256);
						if (p->mapping == NULL)
							exit(12);
						tmpTok = strtok(NULL, "=");
						if (tmpTok != NULL) {
							strcpy(p->mapping,
							       tmpTok);
						} else {
							strcpy(p->mapping,
							       p->symbol);
						}
						if (hashSearch(hashTable, index,
							       p->symbol) ==
						    NULL) {
							hashTable[index++] = p;
						}
						i++;
					} else {
						strncpy(tokenD, &argv[i][2],
							255);
						p = malloc(sizeof(pair));
						if (p == NULL)
							exit(12);
						p->symbol = malloc(256);
						if (p->symbol == NULL)
							exit(12);
						strcpy(p->symbol,
						       strtok(tokenD, "= "));
						p->mapping = malloc(256);
						if (p->mapping == NULL)
							exit(12);
						tmpTok = strtok(NULL, "=");
						if (tmpTok != NULL) {
							strcpy(p->mapping,
							       tmpTok);
						} else {
							strcpy(p->mapping,
							       "NULL");
						}
						if (hashSearch(hashTable, index,
							       p->symbol) ==
						    NULL) {
							hashTable[index++] = p;
						}
					}
				}
			} else {
//                              caz de input invalid
				fprintf(stderr, "Invalid input");
				exit(1);
			}
		}
	}

//      eliberez memoria si golesc hashMapul
	for (i = 0; i < index; i++) {
		free(hashTable[i]->symbol);
		free(hashTable[i]->mapping);
		free(hashTable[i]);
	}

//      eliberez restul de memorie
	free(outfile);
	free(dir);
	free(hashTable);
	return 0;
}
