#include "Utility.h"

#include <stdlib.h>
#include <string.h>

//#define DEBUG

//==========================================CONSTANTS================================================

const size_t MAX_WORD_LENGTH    = 512;

//======================================FUNCTION PROTOTYPES==========================================

static size_t CalcNumberWords(FILE* fp);

//======================================FUNCTION IMPLEMENTATIONS=====================================

size_t CalcNumberWords(FILE* fp)
{
    size_t   number_words   = 0;
    long int start_file_pos = ftell(fp);
    bool     is_space       = false;
    int      c              = 0;

    while ((c = getc(fp)) != EOF)
    {
        if (c == ' ' || c == '\n' || c == '\f' || c == '\t' || c == '\v')
        {
            if (!is_space)
            {
                number_words++;
                #ifdef DEBUG
                    printf("<n_word%d>\n", number_words);
                #endif
            }
            is_space = true;
        }
        else
        {
            is_space = false;
        }
        #ifdef DEBUG
            printf("%c", c);
        #endif
    }
    if (!is_space)
        number_words++;

    fseek(fp, start_file_pos, SEEK_SET);
    return number_words;
}

char** GetWordsFromFile(FILE* fp, size_t* array_size)
{
    *array_size = CalcNumberWords(fp);
    #ifdef DEBUG
        printf("n_words = %d\n", *array_size);
        int k = 0;
        scanf("%c", &k);
    #endif
    
    char** new_array = (char**)calloc(sizeof(char*), *array_size);
    
    char word[MAX_WORD_LENGTH] = {0};
    size_t index               = 0;
    while (fscanf(fp, "%s", word) == 1)
    {
        #ifdef DEBUG
            printf("%d: word = %s\n", index, word);
        #endif
        size_t word_len = strlen(word);

        new_array[index] = (char*)calloc(sizeof(char), word_len + 1);
        memcpy(new_array[index], word, sizeof(char) * word_len);
        index++;
    }
    
    return new_array;
}