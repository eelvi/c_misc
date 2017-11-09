#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H

//type
typedef int element_t;

typedef struct _darray {
int size;
int capacity;
element_t* data;
} darray;

darray* new_darray();
int add_element (darray *dest_da, element_t val);
int reserve(darray *dest_da, int n);
int free_darray(darray* da_ptr);

#endif
