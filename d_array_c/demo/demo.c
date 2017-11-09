#include <stdio.h>
#include "../src/dynamic_array.h"


void print_darray_ints(darray* da);

int main(int argc, const char **argv)
{


    //initailizing an empty array
    darray *myarray = new_darray();
    printf("initial size:%d, cap:%d\n",myarray->size, myarray->capacity);
   
    //requesting preallocating 10 elements
    reserve(myarray, 10);

    printf("\tafter reserving 10 elements:\nsize:%d, cap:%d\n"
	   ,myarray->size, myarray->capacity);

    //testing adding elements
    int elements_to_add=24;
    for (int i=0; i<elements_to_add; i++){
	add_element(myarray, i);
    }
    printf("\tadded %d elements\n",elements_to_add);
    printf("\tafter adding:\nsize:%d, cap:%d\n",myarray->size, myarray->capacity);

    //printing contents
    print_darray_ints(myarray);

    int rv;
    rv = free_darray(myarray);
    printf("freed darray, return code: %d\n",rv);

    return 0;

}

void print_darray_ints(darray* da)
{
    printf("\nprinting %d elements:\n\t[",da->size);
    for (int i=0; i< da->size; i++){
	if (i)
	    putchar(',');

	printf("%d",da->data[i]);
    }
    printf("]\n");
}
