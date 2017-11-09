#include <stdio.h>
#include "../src/dynamic_array.h"


void print_darray(darray* da);

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
	//adding elements
	add_element(myarray, i);
    }
    printf("\tadded %d elements\n",elements_to_add);
    printf("\tafter adding:\nsize:%d, cap:%d\n",
	   myarray->size, myarray->capacity);

//printing contents using a helper function
    print_darray(myarray);

    //deleting elements
    printf("\n\ndeleting elements at indices 0, 4, and 7:",elements_to_add);
	delete_element(myarray, 7);
	delete_element(myarray, 4);
	delete_element(myarray, 0);
    print_darray(myarray);


    printf("\n\nshrinking size to 12:",elements_to_add);

	shrink_array(myarray, 12);

    print_darray(myarray);
    printf("\n\tcurrent size:\nsize: %d, cap:%d\n",
	    myarray->size, myarray->capacity);

    printf("freed darray, return code: %d\n", 
	    free_darray(myarray)
	  );

    return 0;

}

void print_darray(darray* da)
{
    printf("printing %d elements:\n\t[",da->size);
    for (int i=0; i< da->size; i++){
	if (i)
	    putchar(',');

	printf("%d",da->data[i]);
    }
    printf("]\n");
}
