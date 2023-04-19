#include <string.h>

#include "Libs/HashTable/HashTable.h"

const size_t MAX_WORD_LENGTH = 1000;
const size_t TABLE_SIZE      = 255;

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

int comparator(Element_t a, Element_t b)
{
    return strcmp(a, b);
}

void TestHashFunctions(const char* test_data_file, const char* result_file)
{
    FILE* test_data = fopen(test_data_file, "r");
    FILE* result_fp = fopen(result_file,    "w");

    HashTable_t hash_tables[6] = {};

    HashTableCtor(&hash_tables[0], TABLE_SIZE, HashFunction1, comparator);
    HashTableCtor(&hash_tables[1], TABLE_SIZE, HashFunction2, comparator);
    HashTableCtor(&hash_tables[2], TABLE_SIZE, HashFunction3, comparator);
    HashTableCtor(&hash_tables[3], TABLE_SIZE, HashFunction4, comparator);
    HashTableCtor(&hash_tables[4], TABLE_SIZE, HashFunction5, comparator);
    HashTableCtor(&hash_tables[5], TABLE_SIZE, HashFunction6, comparator);

    char word[MAX_WORD_LENGTH] = {0};
    while (fscanf(test_data, "%s", word) == 1)
    {
        for (int i = 0; i < 6; i++)
        {
            char* new_word = (char*)calloc(sizeof(char), strlen(word) + 1);
            memcpy(new_word, word, sizeof(char) * strlen(word));
            HashTableInsert(&hash_tables[i], new_word);
        }
    }
    fclose(test_data);

    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < hash_tables[i].table_size; j++)
            fprintf(result_fp, "%d;", hash_tables[i].lists[j].size);
        fprintf(result_fp, "\n");
    }
    

    for (size_t i = 0; i < 6; i++)
        HashTableDtor(&hash_tables[i]);

    fclose(result_fp);
}

int main()
{
    TestHashFunctions("Data/English.txt", "Data/CmpHashFunctions.csv");
}