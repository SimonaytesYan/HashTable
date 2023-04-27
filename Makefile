DEBUG_FLAGS   = -D _DEBUG -ggdb3 -std=c++2a -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wstack-usage=8192 -pie -fPIE -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,nonnull-attribute,leak,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr
FAST_FLAGS    = -O1

debug: hash_table_debug utility_debug crc32
	g++ $(DEBUG_FLAGS) Src/main.cpp Obj/Logging.obj Obj/Swap.obj Obj/HashTable.obj Obj/HashCrc32.obj Obj/Utility.obj -o Exe/Run
fast: hash_table utility_fast crc32
	g++ -no-pie $(FAST_FLAGS) Src/main.cpp Obj/Logging.obj Obj/Swap.obj Obj/HashTable.obj Obj/HashCrc32.obj Obj/Utility.obj -o Exe/Run

hash_table: logging swap
	g++ $(FAST_FLAGS) -c Src/Libs/HashTable/HashTable.cpp Obj/Logging.obj Obj/Swap.obj -o Obj/HashTable.obj
hash_table_debug: logging swap 
	g++ $(DEBUG_FLAGS) -c Src/Libs/HashTable/HashTable.cpp Obj/Logging.obj Obj/Swap.obj -o Obj/HashTable.obj

crc32:
	nasm -f elf64 Src/Libs/HashCrc32.s -o Obj/HashCrc32.obj

utility_debug:
	g++ $(DEBUG_FLAGS) -c Src/Libs/Utility/Utility.cpp -o Obj/Utility.obj
utility_fast:
	g++ $(FAST_FLAGS)  -c Src/Libs/Utility/Utility.cpp -o Obj/Utility.obj

logging:
	g++ $(FAST_FLAGS) -c Src/Libs/Logging/Logging.cpp -o Obj/Logging.obj
swap:
	g++ $(FAST_FLAGS) -c Src/Libs/Swap/Swap.cpp -o Obj/Swap.obj