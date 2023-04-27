#ifndef __SYM_LIST__
#define __SYM_LIST__

#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "Logging/Logging.h"
#include "Swap/Swap.h"
#include "Errors.h"

//==========================================CONSTANTS================================================

static const int       ResizeCoef         = 2;
static const void*     POISON_PTR         = (void*)13;
static const size_t    POISON_SIZE_T      = 0X7FFFFFFF;
static const char      COMAND_PROTOTYPE[] = "Dot GraphicDumps/dump%d -o GraphicDumps/Dump%d.png -T png";
static       int       GRAPHIC_DUMP_CNT   = 0;

//============================================STRUCTS=============================================

typedef struct ListElem 
{
    Element_t val  = {};
    int       next = -1;
    int       prev = -1;
} ListElem;

typedef struct LogInfo 
{
    const char* name     = "(null)";
    const char* function = "(null)";
    const char* file     = "(null)";
    int         line     = POISON_SIZE_T;
    bool        status   = false;
} LogInfo;

typedef struct List_t
{
    ListElem* data      = nullptr;

    size_t    size      = 0;
    size_t    capacity  = 0;
    LogInfo   debug     = {};
    int       free      = -1;
    bool      linerized = false;
}List_t;

//======================================FUNCTION PROTOTYPES==========================================
static int  ListConstructor(List_t* list, int capacity, int line, const char* name, const char* function, const char* file);
static int  ListDtor(List_t* list);
static int  ListCheck(List_t* list);
static void DumpList(List_t* list, const char* function, const char* file, int line);
static void GraphicDump(List_t* list);

static int ListInsert(List_t* list, Element_t value, int after_which, int* index = nullptr);
static int ListRemove(List_t* list, int index);
static int FindFree(List_t* list, int* index);
static int ResizeUp(List_t* list, int new_capacity);
static int ListBegin(List_t* list, int *index);
static int ListEnd(List_t* list, int *index);
static int ListLinerization(List_t* list);
static int LogicalIndexToPhys(List_t* list, int logic_index, int* physic_index);
static int LogicaIlndexToPhys(List_t* list, int logic_index, int* physic_index);
static int Logica1IndexToPhys(List_t* list, int logic_index, int* physic_index);
static int Logica1lndexToPhys(List_t* list, int logic_index, int* physic_index);

//extern "C" inline void ListIterate(List_t* list, size_t* index);
static void ListIterate(List_t* list, size_t* index);

//======================================FUNCTION IMPLEMENTATIONS=====================================

static int LogicaIlndexToPhys(List_t* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pointer to list = null?\n");

    printf("You shouldn`t use this function. It is terribly not optimized!\n");
    LogPrintf("You shouldn`t use this function. It is terribly not optimized!\n");

    abort();
    return -1;
}

static int Logica1IndexToPhys(List_t* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pointer to list = null?\n");

    printf("Hey, man. This is bad neighborhood (function). Get out of here while the going is good\n");
    LogPrintf("Hey, man. This is bad neighborhood (function). Get out of here while the going is good\n");

    abort();
    return -1;
}

static int Logica1lndexToPhys(List_t* list, int logic_index, int* physic_index)
{
    if (list == nullptr)
        printf("Why pouinter to list = null?\n");

    printf("You are realy stuped, because you use this function! So, i have to delete all data in list\n");
    LogPrintf("You are realy stuped, because you use this function! So, i have to delete all data in list\n");
    
    free(list->data);

    return 0;
}

static void ListIterate(List_t* list, size_t* index)
{
    //*index = list->data[*index].next;
    //return;

    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif

    CHECK(index == nullptr || index == POISON_PTR, "index = nullptr", (void)-1);

    if (*index < 0 || (size_t)(*index) > list->capacity)
        return;

    *index = list->data[*index].next;

    return;
}

static int ListBegin(List_t* list, int *index)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif
    if (list->capacity >= 1)
        *index = list->data[0].next;
    else
        *index = -1;

    return 0;
}

static int ListEnd(List_t* list, int *index)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif
    if (list->capacity >= 1)
        *index = list->data[0].prev;
    else
        *index = -1;

    return 0;    
}

static int ListLinerization(List_t* list)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif

    if (list->linerized || list->size == 0)
        return 0;

    ListElem* new_data = (ListElem*)calloc(list->capacity + 1, sizeof(ListElem));

    size_t index = 0;
    for(int i = 0; i < list->size; i++)
    {
        ListIterate(list, &index);
        if (index == -1)
            return -1;

        memcpy(new_data[i + 1].val, list->data[index].val, sizeof(Element_t));

        if (i + 1 == list->size)
            new_data[i + 1].next = 0;
        else
            new_data[i + 1].next = i + 2;
        new_data[i + 1].prev = i;
    }

    new_data[0].next = 1;
    new_data[0].prev = list->size;

    free(list->data);
    list->data = new_data;

    if (list->capacity != list->size)
    {
        list->free = list->size + 1 ;
        list->data[list->size + 1].next = -1;
        for(int i = list->size + 2; i <= list->capacity; i++)
        {
            list->data[i - 1].prev = i;
            list->data[i].next     = -1;
            list->data[i].prev     = -1;
        }
    }

    list->linerized = true;

    return 0;
}

static void GraphicDump(List_t* list)
{
    char name[20] = "";
    sprintf(name, "GraphicDumps/dump%d", GRAPHIC_DUMP_CNT);
    FILE* fp = fopen(name, "w");

    fprintf(fp, "digraph G{\n");
    fprintf(fp, "rankdir = LR;\n");
    fprintf(fp, "node[shape = record, fontsize = 14, style=filled];\n");
    fprintf(fp, "splines = ortho\n");

    CHECK(list == nullptr || list == POISON_PTR, "Pointer to list = null or poison", (void)0);
    CHECK(list->data == nullptr || list->data == POISON_PTR, "Pointer to list data = null or poison", (void)0);

    fprintf(fp, "info[label = \"size = %zu\\n |"         \
                               "capasity = %ld \\n |"    \
                               "<f> free = %d \\n |"    \
                               "linerized = %d \\n \"]\n",
                               list->size, 
                               list->capacity, 
                               list->free, 
                               list->linerized);

    for(int i = 0; i <= list->capacity; i++)
    {
        if (i == 0)
            fprintf(fp, "Node%d[fillcolor = white, label = \"<i>%d \\n | <v>%p \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
        else if (list->data[i].next == -1 || list->data[i].prev == -1)
            fprintf(fp, "Node%d[fillcolor = \"#B1FF9F\", label = \"<i>%d \\n | <v>%p \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
        else
            fprintf(fp, "Node%d[fillcolor = \"#8F9EFF\", label = \"<i>%d \\n | <v>%p \\n | {<p> %d |<n>  %d}\"]\n", 
                          i,              i,    list->data[i].val, list->data[i].prev, list->data[i].next);
    }
    
    fprintf(fp, "edge[weight = 1000, style=\"invis\"]\n");
    for(int i = 0; i <= list->capacity; i++)
    {
        if (i == 0)
            fprintf(fp, "info:s -> Node0:v:n\n");
        else 
            fprintf(fp, "Node%d:v:e->Node%d:v:w\n", i - 1, i);
    }

    int index = 0;
    
    fprintf(fp, "edge [color = \"blue\", style=\"solid\"]\n");
    index = 0;
    for(int i = 0; i <= list->size; i++)
    {
        int next = list->data[index].next;

        fprintf(fp, "Node%d -> Node%d\n", index, next);

        index = next;
        if (index == 0)
            break;
    }

    fprintf(fp, "edge [color = \"black\", style=\"solid\"]\n");
    index = list->free;
    if (index != -1)
    {
        fprintf(fp, "info:<f>:e -> Node%d\n", index);
        for(int i = 0; i <= list->capacity; i++)    
        {
            int free_ind = list->data[index].prev;
            if (free_ind == -1)
                break;

            fprintf(fp, "Node%d -> Node%d\n", index, free_ind);

            index = free_ind;
        }
    }

    index = 0;
    fprintf(fp, "edge [constraint=false, color = \"red\"]\n");
    for(int i = 0; i <= list->size; i++)
    {
        int prev = list->data[index].prev;

        fprintf(fp, "Node%d -> Node%d\n", index, prev);

        index = prev;
        if (index == 0)
            break;
    }

    fprintf(fp, "}");

    fclose(fp);

    char comand[50] = "";
    sprintf(comand, COMAND_PROTOTYPE, GRAPHIC_DUMP_CNT, GRAPHIC_DUMP_CNT);
    system(comand);
    
    char path[50] = "";
    LogPrintf("<hr>");
    LogPrintf("<img src=\"GraphicDumps/Dump%d.png\">\n", GRAPHIC_DUMP_CNT);
    LogPrintf("<hr>");

    GRAPHIC_DUMP_CNT++;
}

static int ListCheck(List_t* list)
{
    int error = 0;
    if (list == nullptr || list == POISON_PTR)
        error |= NULL_LIST_POINTER;
    else
    {
        if (list->size     == POISON_SIZE_T)     error |= WRONG_SIZE;
        if (list->capacity == POISON_SIZE_T)     error |= WRONG_CAPACITY;
        if (list->data     == nullptr || 
            list->data     == POISON_PTR) error |= DAMAGED_DATA; 
        
        if (list->debug.file     == nullptr ||
            list->debug.file     == POISON_PTR) error |= DEBUG_FILE_DAMAGED;
        if (list->debug.function == nullptr ||
            list->debug.function == POISON_PTR) error |= DEBUG_FUNCTION_DAMAGED;
        if (list->debug.name     == nullptr ||
            list->debug.name     == POISON_PTR) error |= DEBUG_NAME_DAMAGED;
        if (list->debug.line     == POISON_SIZE_T)     error |= DEBUG_LINE_DAMAGED;
    }

    LogAndParseErr(error != 0, error);
    
    return error;
}

#define ListCtor(list, capacity)  ListConstructor(list, capacity, __LINE__, #list, __PRETTY_FUNCTION__, __FILE__)

static int ListConstructor(List_t* list, int capacity, int line, const char* name, const char* function, const char* file)
{
    LogAndParseErr(list == nullptr, NULL_LIST_POINTER);

    list->size     = 0;
    list->capacity = capacity;
    list->free     = -1;
    list->data     = (ListElem*)calloc(capacity + 1, sizeof(ListElem));
    if (list->data != nullptr)
        for(int i = list->capacity; i >= 1; i--)
        {
            list->data[i] = {{}, -1, list->free};
            list->free    = i;
        }

    list->debug.name     = name;
    list->debug.function = function;
    list->debug.file     = file;
    list->debug.line     = line;
    list->debug.status   = true;
    list->linerized      = true;

    #ifdef ON_LIST_CHECKING
        return ListCheck(list);
    #endif
    
    return NO_ERRORS;
}

static int ListDtor(List_t* list)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif
    
    list->capacity  = POISON_SIZE_T;
    list->size      = POISON_SIZE_T;
    list->free      = POISON_SIZE_T;
    list->linerized = false;

    free(list->data);
    list->data = (ListElem*)POISON_PTR;

    list->debug.file     = (const char*)POISON_PTR;
    list->debug.function = (const char*)POISON_PTR;
    list->debug.name     = (const char*)POISON_PTR;
    list->debug.line     = POISON_SIZE_T;

    return 0;
}

static int FindFree(List_t* list, int* index)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif
    *index = list->free;
    list->free = list->data[list->free].prev;

    return 0;
}

static int ListRemove(List_t* list, int index)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif

    CHECK(index > list->capacity || index <= 0, "Error index", -1);

    int next_ind = list->data[index].next;
    int prev_ind = list->data[index].prev;

    CHECK(next_ind == -1 || prev_ind == -1, "Index to not inserted element", -1);

    int tail = 0;
    ReturnIfError(ListEnd(list, &tail));
    if (index != tail)
        list->linerized = false;

    list->data[next_ind].prev = prev_ind;
    list->data[prev_ind].next = next_ind;

    list->data[index].prev = list->free;
    list->data[index].next = -1;

    list->free = index;
 
    list->size--;

    return 0;
}

static int ResizeUp(List_t* list, int new_capacity)
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif
    list->data = (ListElem*)realloc(list->data, sizeof(ListElem)*(new_capacity + 1));

    if (list->data == nullptr)
        return MEMORY_ALLOCATION_ERROR;

    for(int i = new_capacity; i >= list->capacity + 1; i--)
    {
        list->data[i] = {{}, -1, list->free};
        list->free = i;
    }
    
    list->capacity = new_capacity;

    return 0;
}

static int ResizeIfNeed(List_t *list)
{
    if (list->capacity == list->size)
    {
        int new_capacity = 0;
        if (list->capacity == 0)
            new_capacity = 2;
        else
            new_capacity = list->capacity * ResizeCoef;
        ReturnIfError(ResizeUp(list, new_capacity));
    }

    return 0;
}

static int ListInsert(List_t* list, Element_t value, int after_which, int* index) 
{
    #ifdef ON_LIST_CHECKING
        ReturnIfError(ListCheck(list));
    #endif

    CHECK(after_which > list->capacity || after_which < 0, "Error index", -1);
    
    CHECK(list->data[after_which].next == -1 || list->data[after_which].prev == -1, "Index to not inserted element", -1);

    ReturnIfError(ResizeIfNeed(list));

    int free_elem_index = -1;
    ReturnIfError(FindFree(list, &free_elem_index));

    if (index != nullptr && index != POISON_PTR)
        *index = free_elem_index;

    int tail = 0;
    ReturnIfError(ListEnd(list, &tail));
    if (after_which != tail)
        list->linerized = false;
    
    ListElem* new_elem = &list->data[free_elem_index];
    memcpy(new_elem->val, value, strlen(value));
    
    int next       = list->data[after_which].next;
    new_elem->next = next;
    new_elem->prev = after_which;

    list->data[next].prev        = free_elem_index; 
    list->data[after_which].next = free_elem_index;

    list->size++;

    return 0;
}

#endif //__SYM_LIST__