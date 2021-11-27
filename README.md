		To Run
		open the terminal in the same folder as the code files and type the command 'make all'
		to execute the file type the command ./testing	
		(make file will compile the t-tes1.c file using our custom malloc library which is the test file provided in the project problem 				document)
		
	
	Design Overview
		Arena of Free list of size 13 that is array of pointers that points to 2^index block.
		It will allocate the next bigger size that is in power of 2.
		If not present then it will call sbrk to allocate memory of size PAGE_SIZE and will split 
		In free by freeing up memory we will see if there is any buddy present of that block and if it is then we will simply merge
		both the block.
		In realloc if given size is lesser than available memory size we are returning the same pointer.
		posix_memalign will return 0 if successfull else will return err.
		



	Design decision:-
		we are allocating memory in 8-byte addressable.
		we are calling mmap is requested size is >PAGE_SIZE
		else we are calling sbrk(PAGE_SIZE)
		It will split the memory if asked smaller amount 
		It will merge if both free are contiguous allocated and of same size.
		
		
	Known bugs and error
		posix_memalign will return error if it does not able to point that pointer to memory else 0 will be returned.
		
		
