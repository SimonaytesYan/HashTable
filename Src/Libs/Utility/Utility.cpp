#include "Utility.h"

#include <stdlib.h>
#include <string.h>

//==========================================CONSTANTS================================================

const size_t MAX_WORD_LENGTH    = 256;

//======================================FUNCTION PROTOTYPES==========================================

static size_t CalcNumberStrings(FILE* fp);

//======================================FUNCTION IMPLEMENTATIONS=====================================

size_t CalcNumberStrings(FILE* fp)
{
    size_t number_strings = 0;
    long int start_file_pos = ftell(fp);
    int      c              = 0;
    while ((c = getc(fp)) != EOF)
    {
        if (c == '\n')
            number_strings++;
    }
    number_strings++;

    fseek(fp, start_file_pos, SEEK_SET);
    return number_strings;
}

char** GetWordsFromFile(FILE* fp, size_t* array_size)
{
    *array_size = CalcNumberStrings(fp);
    
    char** new_array = (char**)calloc(sizeof(char*), *array_size);
    
    char word[MAX_WORD_LENGTH] = {0};
    size_t index               = 0;
    while (fscanf(fp, "%s", word) == 1)
    {
        size_t word_len = strlen(word);

        new_array[index] = (char*)calloc(sizeof(char), word_len + 1);
        memcpy(new_array[index], word, sizeof(char) * word_len);
        index++;
    }
    
    return new_array;
}