ide_test:
	gcc -g -m32 main_ide.c ide.c

bio_test:
	gcc -g -m32 main_bio.c bio.c ide.c

makefs:
	gcc -g -m32 -o makefs mkfs.c
	touch DISK0 DISK1 test_file1 test_file2 
	echo "hello,world" > test_file1
	echo "hi,tiny file system!" > test_file2
	./makefs ./DISK1 test_file1 test_file2
syscall_test:
	gcc -g -m32 sysfile.c file.c fs.c log.c bio.c ide.c main.c -std=c99
clean:
	rm ./a.out
rmfs:
	rm makefs DISK0 DISK1 test_file1 test_file2
