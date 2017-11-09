#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dynamic_array.h"

#define DA_INITIAL_SIZE 8
#define DA_GROWTH 2

darray* new_darray()
{

    darray* new_da = malloc( sizeof(darray) );
    new_da->capacity = new_da->size = 0;
    new_da->data = NULL;
    return new_da;
}
// 0: success, 1:invalid array ptr, 2:invalid data_ptr
int free_darray(darray* da_ptr)
{
    if (!da_ptr)
	return 1;
    element_t* data_ptr = da_ptr->data;
    free(da_ptr);
    if (data_ptr){
	free(data_ptr);
	return 0;
    }
    return 2;
}

int nearest_power_of_two(int current_n, int target_n)
{
    //when current_n is zero it is assigned the minimum size
    int nearest = (current_n?current_n:DA_INITIAL_SIZE);
    while (nearest<=target_n){
	nearest*=2;
    }
    return nearest;
}

//rv0: success, rv1: failure
int reserve(darray *dest_da, int n)
{
    if (!dest_da)
	return 1;

    if ( dest_da->size < n || n==0 ){
	const size_t element_size = sizeof(element_t);
	//guarenteed even if null
	int nearest_po2 = nearest_power_of_two(dest_da->capacity,n);
	element_t* new_data = realloc(dest_da->data,element_size*nearest_po2);
	if (new_data){
	    dest_da->data = new_data;
	    dest_da->capacity = nearest_po2;
	    return 0;
	}
    }

    return 1;
}
//rv0: success, rv1: invalid da ptr , rv2:can't reallocate
int add_element (darray *dest_da, element_t val)
{
    if(!dest_da){
    return 1;
    }
    if (dest_da->size+1 > dest_da->capacity ){ //reallocate
	int rv = reserve( dest_da, dest_da->size+1 );
	if (rv!=0)
	    return 2;
    }
    int last_idx = dest_da->size;
    dest_da->data[last_idx]=val;
    dest_da->size++;

    return 0;
}
