#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct 
{
    char* symbol;
    char* mapping;
}pair;

pair* hashSearch (pair** hashMap, int size, char* symbol) {
    for (int i = 0; i < size; i++)
        if (strcmp(hashMap[i]->symbol, symbol) == 0)
            return hashMap[i];
    return NULL;
}


char *strreplace(char *s, const char *s1, const char *s2) {
    char *p = strstr(s, s1);
    if (p != NULL) {
        size_t len1 = strlen(s1);
        size_t len2 = strlen(s2);
        if (len1 != len2)
            memmove(p + len2, p + len1, strlen(p + len1) + 1);
        memcpy(p, s2, len2);
    }
    return s;
}


int main(int argc, char **argv) {
    FILE *inFile;
    FILE *oFile;
    int index = 0;
    pair** hashTable = malloc(1000 * sizeof(pair));
    char* dir = malloc(1000);
    char* outfile = malloc(1000);
    char input[256];
    char tmp1[256];
    char tmp2[256];
    char cond[256];
    char *tok;
    long ifPointer = -1;
    pair* p;
    if(argc == 1) {
        fgets(input, 255, stdin);
        printf("%s", input);
    } else {
        for (int i = 1; i < argc; i++)
        {
            if(argv[i][0] == '_' && argv[i][strlen(argv[i])-1] == 'n') {
                inFile = fopen(argv[i], "r");
                fgets(input, 255, (FILE*)inFile);
                strcpy(tmp1, input);
                tok = strtok(tmp1, " ");
                if(strcmp(tok, "#define") == 0) {
                    p = malloc(sizeof(pair));
                    p->symbol = malloc(256);
                    strcpy(p->symbol,strtok(NULL, " "));
                    p->mapping =malloc(256);
                    char* multiLine;
                    char tmpMulti[256];
                    char multilineValue[256] = "";
                    multiLine = strtok(NULL, "\n");
                    if(multiLine[strlen(multiLine) - 1] == '\\') {
                        strncat(multilineValue, multiLine, strlen(multiLine)-1);
                        fgets(input, 255, (FILE*)inFile);
                        strcpy(tmpMulti, input);
                        multiLine = strtok(tmpMulti, "\t");
                        while(multiLine[strlen(multiLine)-2] == '\\') {
                            strncat(multilineValue, multiLine, strlen(multiLine)-2);
                            fgets(input, 255, (FILE*)inFile);
                            strcpy(tmpMulti, input);
                            multiLine = strtok(tmpMulti, "\t");
                        }
                        strncat(multilineValue, multiLine, strlen(multiLine)-1);
                        strcpy(p->mapping, multilineValue);
                    } else {
                        strcpy(p->mapping, multiLine);
                    }
                    if(hashSearch(hashTable, index, p->symbol) == NULL) {
                        hashTable[index++] = p;
                    }
                } else if(strcmp(tok, "#undef") == 0) {
                    free(hashTable[index-1]->mapping);
                    free(hashTable[index-1]->symbol);
                    free(hashTable[index-1]);
                    index--;
                } else if(strcmp(tok, "#if") == 0) {
                    printf("ok");
                    strcpy(cond,strtok(NULL, " "));
                    // char ifTmp[256];
                    if(atoi(cond)) {
                        fgets(input, 256, (FILE*)inFile);
                        while(input[0] == '#') {
                            printf("%s", input);
                            fgets(input, 256, (FILE*)inFile);
                        }
                    }
                } else {
                    printf("%s", input);
                }
                while (1)
                {
                    if(ifPointer != -1) {
                        fseek(inFile, ifPointer, SEEK_SET);
                    }
                    if(!fgets(input, 256, (FILE*)inFile) || feof(inFile) || ferror(inFile)) {
                        break;
                    }
                    for (int i = 0; i < index; i++)
                    {
                        strreplace(input, hashTable[i]->symbol, hashTable[i]->mapping);
                    }
                    strcpy(tmp1, input);
                    strcpy(tmp2, input);
                    tok = strtok(tmp1, " ");
                    // printf("%s \n", tok);
                    
                    if(strcmp(tok, "#define") == 0) {
                        ifPointer = -1;
                        pair* p = malloc(sizeof(pair));
                        p->symbol = malloc(256);
                        strcpy(p->symbol,strtok(NULL, " "));
                        p->mapping =malloc(256);
                        strcpy(p->mapping, strtok(NULL, "\n"));
                        if(hashSearch(hashTable, index, p->symbol) == NULL) {
                            hashTable[index++] = p;
                        }
                    } else if(strcmp(tok, "#undef") == 0) {
                        ifPointer = -1;
                        free(hashTable[index-1]->mapping);
                        free(hashTable[index-1]->symbol);
                        free(hashTable[index-1]);
                        index--;
                    } else if(strcmp(tok, "#if") == 0) {
                        strcpy(cond,strtok(NULL, " "));
                        if(atoi(cond)) {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                printf("%s", input);
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        } else {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        }
                    } else if(strcmp(input, "#else\n") == 0) {
                        if(!atoi(cond)) {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                printf("%s", input);
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        } else {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        }
                    } else if(strcmp(tok, "#elif") == 0) {
                        strcpy(cond,strtok(NULL, " "));
                        if(atoi(cond)) {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                printf("%s", input);
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        } else {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        }
                    } else if(strcmp(input, "#endif\n") == 0) {
                        ifPointer = -1;
                    } else if(strcmp(tok, "#ifdef") == 0) {
                        strcpy(cond,strtok(NULL, " \n"));
                        int tmpCond = 0;
                        for(int i = 0; i < index; i++) {
                            if(strcmp(hashTable[i]->mapping, cond) == 0) {
                                tmpCond = 1;
                                break;
                            }
                        }
                        if(tmpCond) {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                printf("%s", input);
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        } else {
                            fgets(input, 256, (FILE*)inFile);
                            while(input[0] != '#') {
                                ifPointer = ftell(inFile);
                                fgets(input, 256, (FILE*)inFile);
                            }
                        }
                    } else {
                            ifPointer = -1;
                            printf("%s", input);
                    }
                }       
                fclose(inFile);
            } else if(argv[i][0] == '_' && argv[i][strlen(argv[i])-1] == 't') {
            inFile = fopen(argv[i-1], "r");
            oFile = fopen(argv[i], "w+");
            fgets(input, 255, (FILE*)inFile);
            fputs(input,oFile);
            fclose(inFile);
            fclose(oFile);
            } else if(argv[i][0] == '-') {
                if(argv[i][1] == 'D') {
                    if(strlen(argv[i])== 2) {
                        char tokenD[256];
                        strcpy(tokenD, argv[i+1]);
                        p = malloc(sizeof(pair));
                        p->symbol = malloc(256);
                        strcpy(p->symbol, strtok(tokenD, "="));
                        p->mapping = malloc(256);
                        strcpy(p->mapping,  strtok(NULL, "="));
                        if(hashSearch(hashTable, index, p->symbol) == NULL) {
                            hashTable[index++] = p;
                        }
                        i++;
                    } else {
                        char tokenD[256];
                        strncpy(tokenD, &argv[i][2], 255);
                        p = malloc(sizeof(pair));
                        p->symbol = malloc(256);
                        strcpy(p->symbol, strtok(tokenD, "= "));
                        p->mapping = malloc(256);
                        char *tmpTok = strtok(NULL, "=");
                        if(tmpTok != NULL) {
                            strcpy(p->mapping, tmpTok);
                        } else {
                            strcpy(p->mapping, "NULL");
                        }
                        if(hashSearch(hashTable, index, p->symbol) == NULL) {
                            hashTable[index++] = p;
                        }
                    }
                }
            }
        }
        
    }

    for (int i = 0; i < index; i++)
    {
        free(hashTable[i]->symbol);
        free(hashTable[i]->mapping);
        free(hashTable[i]);
    }

    free(outfile);
    free(dir);
    free(hashTable);
    return 0;
}