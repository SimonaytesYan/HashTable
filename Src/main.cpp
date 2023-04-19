#include <string.h>

#include "Libs/HashTable/HashTable.h"

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
        hash = ((hash & 1) << 31 + hash >> 1) ^ element[i];
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
        hash = ((hash & (1 << 31)) >> 31 + hash << 1) ^ element[i];
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

int main()
{
    HashTable_t hash_table1 = {};
    HashTable_t hash_table2 = {};
    HashTable_t hash_table3 = {};
    HashTable_t hash_table4 = {};
    HashTable_t hash_table5 = {};
    HashTable_t hash_table6 = {};

    HashTableCtor(&hash_table1, 100, HashFunction1, comparator);
    HashTableCtor(&hash_table2, 100, HashFunction2, comparator);
    HashTableCtor(&hash_table3, 100, HashFunction3, comparator);
    HashTableCtor(&hash_table4, 100, HashFunction4, comparator);
    HashTableCtor(&hash_table5, 100, HashFunction5, comparator);
    HashTableCtor(&hash_table6, 100, HashFunction6, comparator);

    
}