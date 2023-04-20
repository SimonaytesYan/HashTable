#include "HashTable.h"

//======================================FUNCTION IMPLEMENTATIONS=====================================

void HashTableDtor (HashTable_t* hash_table)
{
    for (int i = 0; i < hash_table->table_size; i++)
        ListDtor(&hash_table->lists[i]);

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
        
        //fprintf(stderr, "elem_i = <%s>\n", element_i);
        //fprintf(stderr, "elem   = <%s>\n", element);
        int comp_res = hash_table->comparator(element_i, element);
        if (!comp_res)
            return true;
        //fprintf(stderr, "end cmp = %d\n", comp_res);

        ListIterate(&hash_table->lists[hash], &index);
        //fprintf(stderr, "go to next index\n");
    }

    return false;
}