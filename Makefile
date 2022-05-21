$(info ***************         Start is here         *************)
default : test.out workaround.out
	./test.out

test.out : mremap_test.c
	gcc -g mremap_test.c -o test.out -Wall

workaround.out : mremap_around.c
	gcc -g mremap_around.c -o around.out -Wall

clean:
	rm *.out

