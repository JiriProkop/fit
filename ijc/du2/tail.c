// tail.c
// Řešení IJC-DU2, 19.4.2022
// Autor: Jiří Prokop, FIT
// Přeloženo: gcc 9.4.0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define LINE_LIMIT 4095

unsigned int lineC = 10;

void line_count_check(char* argv[])
{
    if (strcmp("-n", argv[1]) == 0)
    {
        if ((lineC = (unsigned int) atoi(argv[2])) == 0)
        {
            fprintf(stderr, "invalid argument or 0 after '-n'!\n");
            exit (-1);                    
        }
    }
    else
    {
        fprintf(stderr, "invalid argument!\n");
        exit(-1);
    }
}

// reorganizes the line_buffer so we can keep only lineC lines in it
void line_add(char* line_buffer[])
{
    char* tmp1;
    char* tmp2;
    for(unsigned int i = 0; i < lineC; i++)
    {
        if (i == (lineC - 1))
        {
            line_buffer[0] = line_buffer[i];
            line_buffer[i] = tmp1;
        }
        else
        {
            if (i != 0)
            {
                tmp2 = line_buffer[i];
                line_buffer[i] = tmp1;
                tmp1 = tmp2;
            }
            else
            {
                tmp1 = line_buffer[i];
            }
        }
    }
}

void lineRead(char fileName[], char* line_buffer[], int filePos)
{
    FILE* fp;
    if (filePos > 0)
    {
        fp = fopen(fileName, "r");
        if(fp == NULL)
        {
            fprintf(stderr, "couldn't open the file!\n");
            for(unsigned int i = 0; i < lineC; i++)
            {
                free(line_buffer[i]);
            }
            exit(-1);
        }
    }
    else
    {
        fp = stdin;
    }
    
    int c;
    int char_count = 0;
    bool flag = 1;
    while((c = fgetc(fp)) != EOF)
    {
        if(c == '\n')
        {
            line_buffer[0][char_count] = '\0';
            char_count = 0;
            line_add(line_buffer);
            continue;
        }
        if(char_count == (LINE_LIMIT - 1))
        {
            if(flag)
            {
                fprintf(stderr, "line is bigger than line limit(%d)!\n", LINE_LIMIT);
            }
            line_buffer[0][char_count] = '\0';
            char_count = 0;
            while(c != EOF || c != '\n') {c = fgetc(fp);}
            line_add(line_buffer);
            continue;
        }
        line_buffer[0][char_count] = c;
        char_count++;
    }
    if (filePos > 0)
    {
        fclose(fp);
    }
}

int main(int argc, char* argv[])
{
    int filePos = -1; // -1 = input is read from stdin
                      // otherwise position of filename in argv

    switch (argc)
    {
        case 1:
            break;
        case 2: 
            filePos = 1;
            break;
        case 3:
            line_count_check(argv);
            break;
        case 4:
            line_count_check(argv);
            filePos = 3;
            break;
        default:
            fprintf(stderr, "invalid argument!\n");
            return -1;
            break;
    }

    char *line_buffer[lineC];
    for (unsigned int i = 0; i < lineC; i++)
    {
        line_buffer[i] = calloc(LINE_LIMIT, sizeof(char));
        if (line_buffer[i] == NULL)
        {
            for ( ; i > 0; i--)
            {
                free(line_buffer[i - 1]);
            }
            fprintf(stderr, "malloc error!\n");
            exit(-1);
        }
    }

    lineRead(argv[filePos], line_buffer, filePos);

    for(unsigned int i = lineC; i > 0; i--)
    {
        printf("%s\n", line_buffer[i-1]);
    }

    for(unsigned int i = 0; i < lineC; i++)
    {
        free(line_buffer[i]);
    }

    return 0;
}