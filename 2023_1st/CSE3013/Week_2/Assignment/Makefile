cc = gcc
target = main
objects = main.o calculate.o print.o

$(target) : $(objects)
	$(cc) -o $(target) $(objects)

$(objects) : header.h

.PHONY : clean
clean :
	rm $(target) $(objects)
