#include "HashTable.h"

void HashTableDtor  (HashTable_t* hash_table)
{
    for (int i = 0; i < hash_table->table_size; i++)
    {
        int index = 0;
        ListIterate(&hash_table->lists[i], &index);
        while (index != 0)
        {
            Element_t element_i = hash_table->lists[i].data[index].val;
            free(hash_table->lists[i].data[index].val);

            ListIterate(&hash_table->lists[i], &index);
        }

        ListDtor(&hash_table->lists[i]);
    }

    free(hash_table->lists);
}

void HashTableCtor(HashTable_t* hash_table, size_t table_size, 
                   size_t (*hash_function)(const Element_t object), 
                   int    (*comparator)   (const Element_t a, const Element_t b))
{
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
    size_t hash = (hash_table->hash_function(element)) % hash_table->table_size;
    ListInsert(&(hash_table->lists[hash]), element, 0, nullptr);
}

void HashTableRemove(HashTable_t* hash_table, Element_t element)
{
    size_t hash = (hash_table->hash_function(element)) % hash_table->table_size;

    int index = 0;
    ListIterate(&hash_table->lists[hash], &index);
    while (index != 0)
    {
        Element_t element_i = hash_table->lists[hash].data[index].val;
        if (!hash_table->comparator(element_i, element))
        {
            ListRemove(&hash_table->lists[hash], index);
            return;
        }

        ListIterate(&hash_table->lists[hash], &index);
    }
}

bool HashTableFind(HashTable_t* hash_table, Element_t element)
{
    size_t hash = (hash_table->hash_function(element)) % hash_table->table_size;

    int index = 0;
    ListIterate(&hash_table->lists[hash], &index);
    while (index != 0)
    {
        Element_t element_i = hash_table->lists[hash].data[index].val;
        if (!hash_table->comparator(element_i, element))
            return true;

        ListIterate(&hash_table->lists[hash], &index);
    }

    return false;
}