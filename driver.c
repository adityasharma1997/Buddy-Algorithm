#include <stdio.h>
#include <math.h>
#include "malloc.h"

size_t InternalFrag(size_t size)
{
    size_t total_size = size + sizeof(meta_data);
    if (total_size >= 4096)
        return 0;
    size_t order = ceil(log2(total_size));
    size_t fragment = pow(2, order) - (total_size);
    return fragment;
}

size_t InternalFrag1(size_t size,size_t alignment)
{
    size_t total_size = size + 2*sizeof(meta_data)+alignment;
    if (total_size >= 4096)
        return 0;
    size_t order = ceil(log2(total_size));
    size_t fragment = pow(2, order) - (size + 2*sizeof(meta_data));
    return fragment;
}

int main()
{   
	int choice;
    char *mem;
	while(true)
	{
     printf("Choices: \n");
     printf("1. malloc \n");
     printf("2. calloc\n");
     printf("3. realloc\n ");
     printf("4. realloc_array\n");
     printf("5. memalign \n");
     printf("6. posixmemalign \n");
     printf("7. free \n");
     printf("8. exit \n");
     printf("Enter Option: ");
     scanf("%d",&choice);
     size_t size;

     if (choice == 1)
        {
            printf("Enter the size: ");
            scanf("%zu", &size);
            mem = (char *)malloc(size);
            
            printf("Successfully malloc'd %zu bytes at start addr %p and at ending addr %p\n", size, mem,(char*)mem+(size));          
            printf("Internal fragmention of: %zu",InternalFrag(size));
            printf(" bytes\n");
        }
             //calloc
      else if (choice == 2)
        {
            size_t member,size;
            printf("Enter the number of members:");
            scanf("%zu",&member);
            printf("Enter size of one member: ");
            scanf("%zu", &size);
            mem = (char *)calloc(member, size);
            printf("Successfully calloc'd %zu bytes at start addr %p and at ending addr %p\n", member*size, mem,(char*)mem+(member*(size)));
            printf("Intial content of calloc'd memory:\n");
            for(size_t i=0;i<member*size;i++)
            {
            	printf("%d",*(mem+i));
            }
            printf("\n");          
            printf("Internal fragmention of: %zu",InternalFrag(size*member));
            printf(" bytes\n");
        }
              //realloc
      else if (choice == 3)
        {
            printf("Enter size of to reallocate: ");
            scanf("%zu", &size);
            mem = (char *)realloc(mem, size);
            printf("Successfully realloc'd %zu bytes at start addr %p and at ending addr %p\n", size, mem,(char*)mem+(size));           
            printf("Internal fragmention of: %zu",InternalFrag(size));
            printf(" bytes\n");
        }
        ///realloc_array
      else if (choice == 4)
        {
            size_t member, size;
            printf("Enter the number of members: ");
            scanf("%zu", &member);
            printf("Enter size of one member: ");
            scanf("%zu",&size);
            mem = (char *)reallocarray(mem, member, size);
            printf("Successfully malloc'd %zu bytes at start addr %p and at ending addr %p\n", size*member, mem,(char*)mem+(member*size)); 
            printf("Internal fragmention of:%zu",InternalFrag(size*member));
            printf(" bytes\n");
        }
              //Memalign
      else if (choice == 5)
        {
            size_t alignment;
            printf("Enter the byte alignment (Alignment shouzu be in power of 2): ");
            scanf("%zu",&alignment);
            printf("Enter the size: ");
            scanf(" %zu",&size);
            mem = (char *)memalign(alignment, size);
            printf("Successfully memalign'd %zu bytes at start addr %p and at ending addr %p\n", size, mem,(char*)mem+(size));          
            printf("Internal fragmention of: %zu",InternalFrag1(size,alignment));
            printf(" bytes\n");
        }
        //posixmemalign
      else if (choice == 6)
        {
            void *memptr=NULL;
            size_t alignment;
            printf("Enter the byte alignment (Alignment shouzu be in power of 2): ");
            scanf("%zu",&alignment);
            printf("Enter the size: ");
            scanf("%zu",&size);
            posix_memalign(&memptr, alignment, size);
            printf("Successfully posix_memalign'd %zu bytes at start addr %p and at ending addr %p\n", size, memptr,(char*)memptr+(size));            
            printf("Internal fragmention of:%zu",InternalFrag1(size,alignment));
            printf(" bytes\n");
        }
      else if (choice == 7)
        {
            free((void *)mem);
            printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
        }
      else if (choice == 8)
        {
            break;
        }

	}
	return 0;
}