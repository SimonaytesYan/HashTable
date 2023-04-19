#ifndef SYM_HASH_TABLE
#define SYM_HASH_TABLE

typedef char* Element_t; 

#include "../List.h"

struct HashTable_t
{
    size_t  (*hash_function)(Element_t object)         = nullptr;  //<by object returns a hash
    int     (*comparator)   (Element_t a, Element_t b) = nullptr;  //<return 0 if objects are the same and non-zero otherwise

    List_t* lists      = nullptr;                                  //<array of lists to store elements  
    size_t  table_size = 0;                                        //<number of lists in the array of lists
};

void HashTableCtor(HashTable_t* hash_table, size_t table_size, 
                   size_t (*hash_function)(const Element_t object), 
                   int    (*comparator)   (const Element_t a, const Element_t b));
void HashTableDtor  (HashTable_t* hash_table);

void HashTableInsert(HashTable_t* hash_table, Element_t element);
void HashTableRemove(HashTable_t* hash_table, Element_t element);
bool HashTableFind  (HashTable_t* hash_table, Element_t element);

#endif