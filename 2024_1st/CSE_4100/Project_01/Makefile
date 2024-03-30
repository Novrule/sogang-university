CC = gcc
target = testlib
objects = main.o bitmap.o hash.o hex_dump.o list.o

$(target): $(objects)
	$(CC) -o $(target) $(objects)

$(objects) : bitmap.h hash.h hex_dump.h list.h

.PHONY : clean
clean :
	rm $(target) $(objects)