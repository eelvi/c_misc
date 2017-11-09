#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

//type
typedef int element_t;

typedef struct _darray {
int size;
int capacity;
element_t* data;
} darray;

//creates a new empty array and returns a pointer to it.
darray* new_darray();

//adds a new element of type element_t.
int add_element (darray *dest_da, element_t val);

//reserves a size to fit at least n elements 
//  (instead of multiple reallocations)
int reserve(darray *dest_da, int n);

//clears the array, it becomes empty
int clear_array(darray *dest_da);

//if the array has a bigger capacity than it needs it reduces it
//keeping the original elements
int prune(darray *dest_da);

//shrinks the array to the given size, elements after the index are removed.
//capacity will decrease when possible
int shrink_array (darray *dest_da, int new_size);

//deletes the indivudal element at the given index
int delete_element (darray *dest_da, int idx);

//frees the resources used the associated array including the array itself
//  (it's good idea to set the pointer to null after calling this)
int free_darray(darray* da_ptr);
#endif
