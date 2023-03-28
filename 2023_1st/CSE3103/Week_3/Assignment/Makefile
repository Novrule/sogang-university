cc = g++
CPPFLAGS = -g -c

target = main

objects = maㅁin.o Str.o
src = main.cpp Str.cpp

$(target): $(objects) Str.h
	$(cc) -o $(target) $(objects)

clean: 
	rm $(objects) $(target)
