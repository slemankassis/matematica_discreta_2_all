VALGRINDFLAGS = valgrind --leak-check=full -v --show-reachable=yes

all:
	$ rm -f *~ *.o coloreo
	$ gcc -Wall -Wextra -m32 -O3 -std=c99 -g main.c Cthulhu.c -o coloreo

gdb:
	$ gdb ./coloreo

clean:
	$ rm -f *~ *.o coloreo

valgrind:
	$ rm -f *~ *.o coloreo
	$ gcc -Wall -Wextra -m32 -O3 -std=c99 -g main.c Cthulhu.c -o coloreo
	$(VALGRINDFLAGS) ./coloreo < "grafo"
