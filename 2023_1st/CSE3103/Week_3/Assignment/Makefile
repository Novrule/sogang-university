cc = g++
cppflags= -g -c

target = main

objects = main.o Str.o
src = main.cpp Str.cpp

$(target): $(objects)
	$(cc) -o $(target) $(objects)

clean: 
	rm $(objects) $(target)
