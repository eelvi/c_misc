all: darray_demo


obj/dynamic_array.o:
	gcc -c src/dynamic_array.c -o obj/dynamic_array.o

obj/demo.o:
	gcc -c demo/demo.c -o obj/demo.o

darray_demo: obj/dynamic_array.o obj/demo.o
	gcc obj/dynamic_array.o obj/demo.o -o darray_demo

clean:
	rm obj/*.o
	rm darray_demo
