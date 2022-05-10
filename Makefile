$(info ***************         Start is here         *************)
default : test.out workaround.out
	./test.out

test.out : mremap_test.c
	gcc -g mremap_test.c -o test.out -Wall

workaround.out :

clean:
	rm *.out

