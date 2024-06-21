



#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <malloc.h>
#include "string.h"
#include <memory.h>
#include <assert.h>




//I miss templates in C++ so much, this looks just so ugly with void*
//Pretty sure this code should not have been done in this way,
//but my little experience in C doesn't help to figure out how should
//be implemented
//I have seen once an implementation of a linked list where the data
//is completely separated from like the Node struct, that meant that the user
//should have declared it's struct of data, and then used the logic of 
//the linked list with macros, but that meant that the user had to 
//write the insertion and remove functions himself, which are effectively
//the core logic of the Vector, 


typedef struct{
    void* data;
    size_t size;
    size_t capacity;
    size_t stride;
} Vector;



Vector* make_vector_with_capacity( size_t stride , size_t RequestedCapacity ){
    const size_t capacity = RequestedCapacity < 4 ? 2 * 4 : RequestedCapacity; 
    
    Vector* v = (Vector*) malloc(sizeof(Vector));
    if(v == NULL){
        perror("Unable to allocate the vector");
        return NULL;
    }

    v->capacity = capacity;
    v->stride = stride;
    v->size = 0;

    v->data = malloc(capacity * stride);
    if(v->data == NULL){
        perror("Unable to allocate a chunk of memory of the requested capacity");
        return NULL;
    }


    return v;
}

Vector* make_vector(size_t stride){
    return make_vector_with_capacity(stride , 4);
}

Vector* copy_vector(Vector* v1){
    Vector* vNew = make_vector_with_capacity(v1->stride , v1->size);
    memcpy(vNew->data , v1->data , v1->size * v1->stride);
    vNew->size = v1->size;
    return vNew;
}


//Buffer has to be allocated with malloc, or has to be detached from the vector before
//vector destruction
Vector* vector_from_raw_buffer(void* buffer , size_t size , size_t stride){
    Vector* vNew = malloc(sizeof(Vector));
    vNew->size = size;
    vNew->stride = stride;
    vNew->data = buffer;
    return vNew;
}

void delete_vector(Vector* v){
    if(v->data)
        free(v->data);
    free(v);
}


void __vector_reallocate(Vector* v , const size_t NewCapacity){
    void* tmp = realloc(v->data , NewCapacity * v->stride);
    if(tmp == NULL){
        perror("unable to reallocate to the new capacity");
        return;
    }

    v->capacity = NewCapacity;
    v->data = tmp;
}


void vector_resize(Vector* v , const size_t size){
    if(size > v->capacity)
        __vector_reallocate(v , size);
    
    v->size = size;
}

void vector_reserve(Vector* v , const size_t size){
    if(size <= v->capacity)
        return;

    __vector_reallocate(v , size);
}

void vector_push_back(Vector* v , void* element){
    if(v->size >= v->capacity )
        __vector_reallocate(v , v->size * 2);
    
    memcpy( ((char*)v->data + (v->size++) * v->stride) , (char*)element , v->stride );
}

void vector_remove_item(Vector* v , size_t index){
    if(index >= v->size)
        return;
    
    memcpy( ((char*)v->data + index * v->stride) , ((char*)v->data + ( index + 1 )  * v->stride)  , (v->size - index) * v->stride );
}

void vector_clear(Vector* v ){
    v->size = 0;
}

char* vector_get(Vector* v , size_t index){
    assert(index <= v->size);
    return (char*)(v->data) + (index * v->stride);
}

char* vector_back(Vector* v){
    if(v->size == 0) 
        return (char*)v->data;

    return vector_get(v , v->size - 1);
}

char* vector_front(Vector* v ){
    return (char*)v->data;
}


void print_vector_int(Vector* v){
    printf("( ");
    int size = (int)v->size;
    for(int i = 0; i < size - 1; i++){
        printf("%d , " , *(int*) vector_get(v , i));
    }
    if(v->size > 0){
        // printf("%d" , *(int*) vector_get(v , v->size - 1));
        printf("%d" , *(int*) vector_back(v));
    }
    printf(" )");
}

void print_vector_double(Vector* v){
    printf("( ");
    int size = (int)v->size;
    for(int i = 0; i <  size - 1; i++){
        printf("%lf , " , *(double*) vector_get(v , i));
    }
    if(v->size > 0){
        printf("%lf" , *(double*) vector_get(v , v->size - 1));
    }
    printf(" )");
}



void print_array_int(int* v , size_t size){
    printf("( ");
    for(int i = 0; i < (int)size - 1; i++){
        printf("%d , " , v[i]);
    }
    if(size > 0){
        printf("%d" , v[size - 1]);
    }
    printf(" )");
}

void print_array_double(double* v , size_t size){
    printf("( ");
    for(int i = 0; i < (int)size - 1; i++){
        printf("%d , " , v[i]);
    }
    if(size > 0){
        printf("%d" , v[size - 1]);
    }
    printf(" )");
}

#endif