# find *.c file to SRCS in Windows
SRCS = $(wildcard common/*.c) $(wildcard gb/*.c) $(wildcard double_learning/*.c) $(wildcard int_learning/*.c)
HEADERS = $(wildcard common/*.h) $(wildcard gb/*.h) $(wildcard double_learning/*.h) $(wildcard int_learning/*.h)

gbplat.gb: $(SRCS) $(HEADERS)
	..\..\gbdk-win64\gbdk\bin\lcc.exe -o $@ gb/main.c gb/bg_tile.c gb/fg_tile.c common/platformer.c common/stage_map.c

double_learning.exe: $(SRCS) $(HEADERS)
	gcc -o $@ double_learning/main.c double_learning/qlearning.c common/platformer.c common/stage_map.c

int_learning.exe: $(SRCS) $(HEADERS)
	gcc -o $@ int_learning/pc_main.c int_learning/qlearning.c common/platformer.c common/stage_map.c
