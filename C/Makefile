# SKEIN_BLOCK = skein_block_x64.o
SKEIN_BLOCK = skein_block.o

CFLAGS = -Wall -Werror

all: aggie_skein

aggie_skein: aggie_skein.o skein.o $(SKEIN_BLOCK)
	gcc -o aggie_skein aggie_skein.o skein.o $(SKEIN_BLOCK)

clean:
	rm -f *.o
