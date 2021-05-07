CFLAGS	+= -Wall
INC		+=
DEF		+=
CC		?= gcc
OBJ		:= $(subst .c,.o,$(shell ls *.c))
TRG		?= $(notdir $(PWD)) 

# Compile C files
%.o: %.c
	@echo 'Compiling $@...'
	$(CC) -c -o $@ $< $(CFLAGS) $(INC) $(DEF)

all:  $(OBJ)
	$(CC) $(OBJ) -o $(TRG) $(CFLAGS)

clean:
	rm -f $(OBJ) $(TRG)