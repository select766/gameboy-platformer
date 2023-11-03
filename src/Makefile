# find *.c file to SRCS in Windows
SRCS = $(wildcard *.c)
HEADERS = $(wildcard *.h)

gbplat.gb: $(SRCS) $(HEADERS)
	..\..\gbdk-win64\gbdk\bin\lcc.exe -o gbplat.gb main.c bg_tile.c fg_tile.c platformer.c
