#define NDEBUG

#include <assert.h>

#include "HashTable.h"

//#define DEBUG

//======================================FUNCTION IMPLEMENTATIONS=====================================

void HashTableDtor (HashTable_t* hash_table)
{
    assert(hash_table    != nullptr);

    for (int i = 0; i < hash_table->table_size; i++)
        ListDtor(&hash_table->lists[i]);

    free(hash_table->lists);
}

void HashTableCtor(HashTable_t* hash_table, size_t table_size, 
                   size_t (*hash_function)(const Element_t object), 
                   int    (*comparator)   (const Element_t a, const Element_t b))
{
    assert(hash_table    != nullptr);
    assert(hash_function != nullptr);
    assert(comparator    != nullptr);

    hash_table->table_size    = table_size;
    hash_table->lists         = (List_t*)calloc(sizeof(List_t), table_size);

    for (int i = 0; i < hash_table->table_size; i++)
    {
        ListCtor(&hash_table->lists[i], 0);
    }

    hash_table->hash_function = hash_function;
    hash_table->comparator    = comparator;
}

void HashTableInsert(HashTable_t* hash_table, Element_t element)
{
    assert(hash_table->hash_function != nullptr);
    assert(hash_table->comparator    != nullptr);
    assert(hash_table->lists         != nullptr);
    assert(hash_table != nullptr);

    size_t hash = (hash_table->hash_function(element)) % hash_table->table_size;

    ListInsert(&(hash_table->lists[hash]), element, 0, nullptr);
}

void HashTableRemove(HashTable_t* hash_table, Element_t element)
{
    assert(hash_table->hash_function != nullptr);
    assert(hash_table->comparator    != nullptr);
    assert(hash_table->lists         != nullptr);
    assert(hash_table != nullptr);

    size_t hash = (hash_table->hash_function(element)) % hash_table->table_size;

    size_t index = 0;

    ListIterate(&hash_table->lists[hash], &index);
    while (index != 0)
    {
        if (!hash_table->comparator(hash_table->lists[hash].data[index].val, element))
        {
            ListRemove(&hash_table->lists[hash], index);
            return;
        }

        ListIterate(&hash_table->lists[hash], &index);
    }
}

bool HashTableFind(HashTable_t* hash_table, Element_t element)
{
    assert(hash_table->hash_function != nullptr);
    assert(hash_table->comparator    != nullptr);
    assert(hash_table->lists         != nullptr);
    assert(hash_table != nullptr);

    size_t hash  = (hash_table->hash_function(element)) % hash_table->table_size;

    return ListFind(&hash_table->lists[hash], element, hash_table->comparator);
}