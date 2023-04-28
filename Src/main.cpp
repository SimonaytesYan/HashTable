#include <assert.h>
#include <emmintrin.h>
#include <immintrin.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "Libs/HashTable/HashTable.h"
#include "Libs/Utility/Utility.h"

//#define DEBUG

//==========================================CONSTANTS================================================

const size_t kNumberFlags = 2;
const char   kFlags[2][3] = {"-h", 
                             "-s"};

const size_t kSrc32HashMask    = 0xEDB88320;

const size_t kTableSize        = 997;
const size_t kNumberHashFunc   = 6;
const size_t kNumberSpeedTests = 40000000;

//======================================FUNCTION PROTOTYPES==========================================

size_t HashFunction1(const Element_t object);
size_t HashFunction2(const Element_t object);
size_t HashFunction3(const Element_t object);
size_t HashFunction4(const Element_t object);
size_t HashFunction5(const Element_t object);
size_t HashFunction6(const Element_t object);

void   TestHashFunctions(const char* test_data_file, const char* result_file);
void   SpeedTest(const char* test_data_file);

extern "C" size_t hash_crc32(const Element_t a);
static inline int comparator(const Element_t a, const Element_t b);

//======================================FUNCTION IMPLEMENTATIONS=====================================

int main(int argc, char* argv[])
{
    bool flags[kNumberFlags] = {};

    for (int i = 1; i < argc; i++)
    {
        for (int j = 0; j < kNumberFlags; j++)
        {
            if (!strcmp(argv[i], kFlags[j]))
            {
                flags[j] = true;
                break;
            }
        }
    }

    if (flags[0])
        TestHashFunctions("Data/WarAndPeace.txt", "Data/CmpHashFunctions2.csv");
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
    size_t hash = object[0];
    while (object[i] != '\0')
    {
        hash = (((hash & (1ULL << 63ULL)) >> 63ULL) | (hash << 1ULL)) ^ (size_t)object[i];
        i++;
    }
    
    return hash;
}

//! CRC32 hash
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
    assert(test_data_file != nullptr);
    assert(result_file    != nullptr);
    
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
    assert(test_data_file != nullptr);
    
    HashTable_t hash_table = {};
    HashTableCtor(&hash_table, kTableSize, hash_crc32, comparator);

    FILE* test_data     = fopen(test_data_file, "r");
    size_t number_words = 0;
    char** words_arr    = GetWordsFromFile(test_data, &number_words);
    fclose(test_data);

    for(size_t i = 0; i < number_words / 2; i++)
    {
        if (words_arr[i] != nullptr)
            HashTableInsert(&hash_table, words_arr[i]);
    }

    for (size_t i = 0; i < kNumberSpeedTests; i++)
    {
        if (words_arr[i % number_words] != nullptr)
            HashTableFind(&hash_table, words_arr[i % number_words]);
    }

    HashTableDtor(&hash_table);

    for (size_t i = 0; i < number_words; i++)
        free(words_arr[i]);
    free(words_arr);
}

static inline int comparator(const Element_t a, const Element_t b)
{
    __asm__(
        ".intel_syntax noprefix\n\t"
        "mov     rax, QWORD PTR [rdi]\n"
        "mov     rdx, QWORD PTR [rsi]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 8]\n"
        "mov     rdx, QWORD PTR [rsi + 8]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 16]\n"
        "mov     rdx, QWORD PTR [rsi + 16]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 24]\n"
        "mov     rdx, QWORD PTR [rsi + 24]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 32]\n"
        "mov     rdx, QWORD PTR [rsi + 32]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 40]\n"
        "mov     rdx, QWORD PTR [rsi + 40]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 48]\n"
        "mov     rdx, QWORD PTR [rsi + 48]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 54]\n"
        "mov     rdx, QWORD PTR [rsi + 54]\n"
        "cmp     rax, rdx\n"
        "jne     _ret\n"

        "mov     rax, QWORD PTR [rdi + 60]\n"
        "mov     rdx, QWORD PTR [rsi + 60]\n"
        
        "_ret:\n\t"
        "sub eax, ecx\n\t"
        "ret\n\t"

        ".att_syntax prefix\n"
    );
}

/*
__m512i a_512 = _mm512_set_epi8(a[63], a[62], a[61], a[60], a[59], a[58], a[57], a[56],
                                    a[55], a[54], a[53], a[52], a[51], a[50], a[49], a[48],
                                    a[47], a[46], a[45], a[44], a[43], a[42], a[41], a[40],
                                    a[39], a[38], a[37], a[36], a[35], a[34], a[33], a[32],
                                    a[31], a[30], a[29], a[28], a[27], a[26], a[25], a[24],
                                    a[23], a[22], a[21], a[20], a[19], a[18], a[17], a[16],
                                    a[15], a[14], a[13], a[12], a[11], a[10], a[9],  a[8],
                                    a[7],  a[6],  a[5],  a[4],  a[3],  a[2],  a[1],  a[0]);
    
    __m512i b_512 = _mm512_set_epi8(b[63], b[62], b[61], b[60], b[59], b[58], b[57], b[56],
                                    b[55], b[54], b[53], b[52], b[51], b[50], b[49], b[48],
                                    b[47], b[46], b[45], b[44], b[43], b[42], b[41], b[40],
                                    b[39], b[38], b[37], b[36], b[35], b[34], b[33], b[32],
                                    b[31], b[30], b[29], b[28], b[27], b[26], b[25], b[24],
                                    b[23], b[22], b[21], b[20], b[19], b[18], b[17], b[16],
                                    b[15], b[14], b[13], b[12], b[11], b[10], b[9],  b[8],
                                    b[7],  b[6],  b[5],  b[4],  b[3],  b[2],  b[1],  b[0]);

    return _mm512_cmp_epi64_mask(a_512, b_512, _CMP_NEQ_UQ);*/

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