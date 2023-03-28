cc = g++
CPPFLAGS = -g -c

target = main

objects = main.o Str.o
src = main.cpp Str.cpp

$(target): $(objects) Str.h
	$(cc) -o $(target) $(objects)

clean: 
	rm $(objects) $(target)
