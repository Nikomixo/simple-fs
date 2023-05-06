CCOPTS = -Wall -Wextra

simfs_test: simfs_test.o simfs.a
	gcc -o $@ $^

simfs.a: image.o block.o free.o inode.o mkfs.o
	ar rcs $@ $^

#compile simfs_test with flag
simfs_test.o: simfs_test.c
	gcc $(CCOPTS) -DCTEST_ENABLE -o $@ -c $^

# compile all .c files into .o files
%.o: %.c
	gcc $(CCOPTS) -o $@ -c $^

.PHONY: test clean

test: simfs_test
	./simfs_test

clean:
	rm -f *.o *.a *_test