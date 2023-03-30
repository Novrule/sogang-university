cc = g++
cppflags = -g -c

target = main

objectas = main.o Array.o RangeArray.o
src = main.cpp Array.cpp RangeArray.cpp

$(target): $(objects)
	$(cc) -o $(target) $(objects)

clean: 
	rm $(objects) $(target)
