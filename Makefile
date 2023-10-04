CC = gcc
CFLAGS = -Wall -pedantic-errors

SRC_DIR = src/
OBJ_DIR = obj/
OUTPUT_DIR = out/

LIB_NAME = tecs.a

debug: CFLAGS += -g
debug: tecs.a

tecs.a: $(patsubst $(SRC_DIR)%.c, %.o, $(wildcard $(SRC_DIR)*.c))
	ar rcs -o $(OUTPUT_DIR)tecs.a $(OBJ_DIR)*.o

%.o: $(SRC_DIR)%.c
	$(CC) $(CFLAGS) -o $(OBJ_DIR)$@ -c $<

.PHONY: clean
clean: 
	rm -f $(OBJ_DIR)*.o
