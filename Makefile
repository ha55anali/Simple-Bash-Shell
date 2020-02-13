cc=g++
objects=source.o shell.o

ex.o: $(objects)
	   $(cc) $(objects) -o ex.out

source.o: source.cpp             
	   $(cc) -c source.cpp    

shell.o: shell.cpp             
	   $(cc) -c shell.cpp    

clean:
	rm -f *.o
