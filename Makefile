all: daiso

daiso: daiso.c
	gcc daiso.c -o daiso

clean:
	rm -rf daiso
