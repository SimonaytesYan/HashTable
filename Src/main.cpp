#include <string.h>
#include <time.h>

#include "Libs/HashTable/HashTable.h"
#include "Libs/Utility/Utility.h"

//#define DEBUG

//==========================================CONSTANTS================================================

const size_t kNumberFlags = 2;
const char   kFlags[2][3] = {"-h", "-s"};

const size_t kSrc32HashMask    = 0xEDB88320;

const size_t kTableSize        = 997;
const size_t kNumberHashFunc   = 6;
const size_t kNumberSpeedTests = 100000000;

//======================================FUNCTION PROTOTYPES==========================================

size_t HashFunction1(const Element_t object);
size_t HashFunction2(const Element_t object);
size_t HashFunction3(const Element_t object);
size_t HashFunction4(const Element_t object);
size_t HashFunction5(const Element_t object);
size_t HashFunction6(const Element_t object);

void   TestHashFunctions(const char* test_data_file, const char* result_file);
void   SpeedTest(const char* test_data_file);

//extern "C" inline int comparator(Element_t a, Element_t b);
static inline int comparator(Element_t a, Element_t b);

//======================================FUNCTION IMPLEMENTATIONS=====================================

int main(int argc, char* argv[])
{
    bool flags[kNumberFlags] = {};

    for (int i = 1; i < argc; i++)
    {
        for (int j = 0; j < kNumberFlags; j++)
        {
            if (strcmp(argv[i], kFlags[j]))
            {
                flags[j] = true;
                break;
            }
        }
    }

    if (flags[0])
        TestHashFunctions("Data/WarAndPeace.txt", "Data/CmpHashFunctions1.csv");
    if (flags[1])
        SpeedTest("Data/WarAndPeace.txt");
}

//===================HASH FUNCTION IMPLEMENTATION=======================
//! this hash function always returns 1
size_t HashFunction1(const Element_t object)
{
    return 1;
}

//! this hash function returns ascii code first symbol
size_t HashFunction2(const Element_t object)
{
    return ((unsigned char*)object)[0];
}

//! this hash function return length of the string
size_t HashFunction3(const Element_t object)
{
    return strlen(object);
}

//! calculate hash by formula:
//! hash[i] = (cyclic bitwise right shift h[i-1]) xor object[i] 
size_t HashFunction4(const Element_t object)
{
    size_t i    = 0;
    size_t hash = object[i];
    while (object[i] != '\0')
    {
        hash = (((hash & 1) << 63) + (hash >> 1)) ^ object[i];
        i++;
    }

    return hash;    
}

//! calculate hash by formula:
//! hash[i] = (cyclic bitwise left shift h[i-1]) xor object[i] 
size_t HashFunction5(const Element_t object)
{
    size_t i    = 0;
    size_t hash = object[i];
    while (object[i] != '\0')
    {
        hash = (((hash & (1ULL << 63ULL)) >> 63ULL) | (hash << 1ULL)) ^ (size_t)object[i];
        i++;
    }
    
    return hash;
}
/*__asm__(
    ".intel_syntax noprefix\n\t"

    "movsx rax, BYTE PTR [rdi]\n"       //rax = object[0]
    "test  al, al\n"                    //  
    "je _ret\n"                         //if (rax == 0) return rax

    "_loop:\n"
    "   rol rax, 1\n"
    "   xor rax, rdx\n"

    "   inc rdi\n"                     //rdi++
    "   movzx rdx, BYTE PTR [rdi]\n"
    "   test dl, dl\n"                   //
    "   jne _loop\n"                     //while (dl != 0) return rax

    "   _ret:\n"
    "   ret\n"

    ".att_syntax prefix\n"
);*/

//! src32 hash
size_t HashFunction6(const Element_t object) 
{
    unsigned int  result = 0xFFFFFFFF;
    unsigned int  mask   = 0;
    size_t        i      = 0;

    while (object[i] != 0)
    {
        result = result ^ (unsigned char)object[i];

        for (int j = 7; j >= 0; j--)
        {
            mask   = -(result &  1);
            result =  (result >> 1) ^ (kSrc32HashMask & mask);
        }
        i++;
    }

    return ~result;
}

//==========================TEST FUNCTION IMPLEMENTATION==========================
void TestHashFunctions(const char* test_data_file, const char* result_file)
{
    HashTable_t hash_tables[kNumberHashFunc] = {};
    HashTableCtor(&hash_tables[0], kTableSize, HashFunction1, comparator);
    HashTableCtor(&hash_tables[1], kTableSize, HashFunction2, comparator);
    HashTableCtor(&hash_tables[2], kTableSize, HashFunction3, comparator);
    HashTableCtor(&hash_tables[3], kTableSize, HashFunction4, comparator);
    HashTableCtor(&hash_tables[4], kTableSize, HashFunction5, comparator);
    HashTableCtor(&hash_tables[5], kTableSize, HashFunction6, comparator);

    FILE* test_data     = fopen(test_data_file, "r");
    size_t number_words = 0;
    char** words_arr    = GetWordsFromFile(test_data, &number_words);
    fclose(test_data);

    #ifdef DEBUG
        printf("n_words = %d\n", number_words);
        int k = 0;
        scanf("%c", &k);
    #endif
    for (size_t table_i = 0; table_i < kNumberHashFunc; table_i++)
    {
        #ifdef DEBUG
            size_t sum         = 0;
            size_t n_dif_words = 0;
        #endif
        for(size_t word_i = 0; word_i < number_words; word_i++)
        {
            if (words_arr[word_i] == nullptr)
                continue;

            if (!HashTableFind(&hash_tables[table_i], words_arr[word_i]))
            {
                HashTableInsert(&hash_tables[table_i], words_arr[word_i]);

                #ifdef DEBUG
                    n_dif_words++;
                    sum += strlen(words_arr[word_i]);
                    printf("len = %d\n", strlen(words_arr[word_i]));
                    printf("%s\n", words_arr[word_i]);
                #endif
            }
        }

        #ifdef DEBUG
            printf("n_words      = %d\n",  number_words);
            printf("n_diff_words = %d\n",  n_dif_words);
            printf("avr_len = %lg\n", sum / (double)n_dif_words);
            int k = 0;
            scanf("%c", &k);
        #endif
    }

    //==============SAVE STATS=======================
    FILE* result_fp = fopen(result_file, "w");

    for (int i = 0; i < kNumberHashFunc; i++)
    {
        for (int j = 0; j < hash_tables[i].table_size; j++)
            fprintf(result_fp, "%ld;", hash_tables[i].lists[j].size);
        fprintf(result_fp, "\n");
    }

    fclose(result_fp);
    //===============================================

    for (size_t i = 0; i < kNumberHashFunc; i++)
        HashTableDtor(&hash_tables[i]);
    
    for (size_t i = 0; i < number_words; i++)
        free(words_arr[i]);
    free(words_arr);
}

void SpeedTest(const char* test_data_file)
{
    HashTable_t hash_table = {};
    HashTableCtor(&hash_table, kTableSize, HashFunction5, comparator);

    FILE* test_data     = fopen(test_data_file, "r");
    size_t number_words = 0;
    char** words_arr    = GetWordsFromFile(test_data, &number_words);
    fclose(test_data);

    for(size_t i = 0; i < number_words / 2; i++)
        HashTableInsert(&hash_table, words_arr[i]);

    for (size_t i = 0; i < kNumberSpeedTests; i++)
        HashTableFind(&hash_table, words_arr[i % number_words]);

    HashTableDtor(&hash_table);

    for (size_t i = 0; i < number_words; i++)
        free(words_arr[i]);
    free(words_arr);
}

static inline int comparator(Element_t a, Element_t b)
{
    return strcmp(a, b);
    
    /*__asm__(
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

        ".att_syntax prefix\n"
    );*/
}