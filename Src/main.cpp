#include <string.h>

#include "Libs/HashTable/HashTable.h"
#include "Libs/Utility/Utility.h"

//==========================================CONSTANTS================================================

const size_t TABLE_SIZE         = 127;
const size_t NUMBER_HASH_FUNC   = 6;
const size_t NUMBER_SPEED_TESTS = 100000000;

//======================================FUNCTION PROTOTYPES==========================================

size_t HashFunction1(const Element_t element);
size_t HashFunction2(const Element_t element);
size_t HashFunction3(const Element_t element);
size_t HashFunction4(const Element_t element);
size_t HashFunction5(const Element_t element);
size_t HashFunction6(const Element_t element);

void   TestHashFunctions(const char* test_data_file, const char* result_file);
void   SpeedTest(const char* test_data_file);

int    comparator(Element_t a, Element_t b);

//======================================FUNCTION IMPLEMENTATIONS=====================================

int main()
{
    //fprintf(stderr, "%d\n", comparator("jacket", "abandon"));
    //TestHashFunctions("Data/English.txt", "Data/CmpHashFunctions.csv");
    SpeedTest("Data/English.txt");
}

//===================HASH FUNCTION IMPLEMENTATION=======================
//! this hash function always returns 1
size_t HashFunction1(const Element_t element)
{
    return 1;
}

//! this hash function returns ascii code first symbol
size_t HashFunction2(const Element_t element)
{
    return element[0];
}

//! this hash function return length of the string
size_t HashFunction3(const Element_t element)
{
    return strlen(element);
}

//! calculate hash by formula:
//! hash[i] = (cyclic bitwise right shift h[i-1]) xor element[i] 
size_t HashFunction4(const Element_t element)
{
    size_t i    = 0;
    size_t hash = element[i];
    while (element[i] != '\0')
    {
        hash = (((hash & 1) << 31) + (hash >> 1)) ^ element[i];
        i++;
    }

    return hash;    
}

//! calculate hash by formula:
//! hash[i] = (cyclic bitwise left shift h[i-1]) xor element[i] 
size_t HashFunction5(const Element_t element)
{
    size_t i    = 0;
    size_t hash = element[i];
    while (element[i] != '\0')
    {
        hash = (((hash & (1 << 31)) >> 31) + (hash << 1)) ^ element[i];
        i++;
    }
    
    return hash;    
}

//! GNU hash
size_t HashFunction6(const Element_t element)
{
    size_t i    = 0;
    size_t hash = 5381;
    while (element[i] != '\0')
    {
        hash = hash * 33 + element[i];
        i++;
    }

    return hash;
}

//==========================TEST FUNCTION IMPLEMENTATION==========================
void TestHashFunctions(const char* test_data_file, const char* result_file)
{
    HashTable_t hash_tables[NUMBER_HASH_FUNC] = {};
    HashTableCtor(&hash_tables[0], TABLE_SIZE, HashFunction1, comparator);
    HashTableCtor(&hash_tables[1], TABLE_SIZE, HashFunction2, comparator);
    HashTableCtor(&hash_tables[2], TABLE_SIZE, HashFunction3, comparator);
    HashTableCtor(&hash_tables[3], TABLE_SIZE, HashFunction4, comparator);
    HashTableCtor(&hash_tables[4], TABLE_SIZE, HashFunction5, comparator);
    HashTableCtor(&hash_tables[5], TABLE_SIZE, HashFunction6, comparator);

    FILE* test_data     = fopen(test_data_file, "r");
    size_t number_words = 0;
    char** words_arr    = GetWordsFromFile(test_data, &number_words);
    fclose(test_data);

    for (size_t table_i = 0; table_i < NUMBER_HASH_FUNC; table_i++)
        for(size_t word_i = 0; word_i < number_words; word_i++)
            HashTableInsert(&hash_tables[table_i], words_arr[word_i]);

    //==============SAVE STATS=======================
    FILE* result_fp = fopen(result_file, "w");

    for (int i = 0; i < NUMBER_HASH_FUNC; i++)
    {
        for (int j = 0; j < hash_tables[i].table_size; j++)
            fprintf(result_fp, "%d;", hash_tables[i].lists[j].size);
        fprintf(result_fp, "\n");
    }

    fclose(result_fp);
    //===============================================

    for (size_t i = 0; i < NUMBER_HASH_FUNC; i++)
        HashTableDtor(&hash_tables[i]);
    
    for (size_t i = 0; i < number_words; i++)
        free(words_arr[i]);
    free(words_arr);
}

void SpeedTest(const char* test_data_file)
{
    HashTable_t hash_table = {};
    HashTableCtor(&hash_table, TABLE_SIZE, HashFunction5, comparator);

    FILE* test_data     = fopen(test_data_file, "r");
    size_t number_words = 0;
    char** words_arr    = GetWordsFromFile(test_data, &number_words);
    fclose(test_data);

    for(size_t i = 0; i < number_words / 2; i++)
        HashTableInsert(&hash_table, words_arr[i]);

    for (size_t i = 0; i < NUMBER_SPEED_TESTS; i++)
        HashTableFind(&hash_table, words_arr[i % number_words]);

    HashTableDtor(&hash_table);

    for (size_t i = 0; i < number_words; i++)
        free(words_arr[i]);
    free(words_arr);
}

int comparator(Element_t a, Element_t b)
{
    __asm__ volatile
    (
        ".intel_syntax noprefix\n\t"
        "xor eax, eax\n\t"              //eax = 0
        "xor ecx, ecx\n\t"              //ecx = 0

        "movb al, BYTE PTR [rdi]\n\t"   //al = a[0]
        "movb cl, BYTE PTR [rsi]\n\t"   //cl = b[0]
        "cmp al, cl\n\t"                //if (al != cl) goto _ret
        "jne _ret\n\t"                  //

        "xor rdx, rdx\n\t"              //
        "inc rdx\n\t"                   //rdx = 1

        "_loop:\n\t" 
            "or al, cl\n\t"             //
            "je _ret\n\t"               //if (al == cl && al == 0) goto _ret

            "movb al, BYTE PTR [rdi + rdx]\n\t"         //al = a[rdx]
            "inc rdx\n\t"                               //rdx++
            "movb cl, BYTE PTR [rsi - 1 + rdx]\n\t"     //cl = b[rdx-1]

            "cmp al, cl\n\t"            //
            "je _loop\n\t"              //while(al == cl)
        "\n"
        "_ret:\n\t"
        "sub eax, ecx\n\t"
        "ret\n\t"
        ".att_syntax prefix\n\t"
    );
}