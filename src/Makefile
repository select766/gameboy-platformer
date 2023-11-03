# find *.c file to SRCS in Windows
SRCS = $(wildcard common/*.c) $(wildcard gb/*.c)
HEADERS = $(wildcard common/*.h) $(wildcard gb/*.h)

gbplat.gb: $(SRCS) $(HEADERS)
	..\..\gbdk-win64\gbdk\bin\lcc.exe -o gbplat.gb gb/main.c gb/bg_tile.c gb/fg_tile.c common/platformer.c
