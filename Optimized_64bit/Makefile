EXE := skein
CC := gcc
AR := ar cr
LD := gcc

CFLAGS = -m64 -O2 -std=c99

SRC    = main.c	SHA3api_ref.c  skein_block.c  skein.c 
OBJ    = $(SRC:.c=.o)

all: $(EXE)

$(EXE): $(OBJ)
	@echo "Linking the target $@"
	$(LD) $(OBJ) -o $@
%.o: %.c
	@echo "Compiling C++ source: $<"
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ)
	rm -f $(EXE)
