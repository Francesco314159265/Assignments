
#ifndef SET_OP_H
#define SET_OP_H


#include <math.h>
#include <assert.h>
#include <inttypes.h>
#include "Core.h"
#include "Vector.h"





void Permute(Vector* v1){

    char* temp = allocate_internal_var(1 , v1->stride);
    for(int i = v1->size - 1; i >= 1 ; i--){
        int j = rand() % (i + 1);  

        memcpy(temp , vector_get(v1 , i) , v1->stride); 
        memcpy(vector_get(v1 , i) , vector_get(v1 , j) , v1->stride); 
        memcpy(vector_get(v1 , j) , temp , v1->stride); 
    }

    deallocate_internal_var(temp , 1 , v1->stride);
}





//The vector must obviously be sorted
size_t BinarySearch_array_index(void* v1 , size_t size , size_t sizeByte , void* value , int(*LessThan)(void* , void*) , int(*equal)(void* , void*)){
    int i = 0;
    int j = size - 1;

    char* array = v1;


    while( i <= j ){
        size_t k = floor((i + j) / 2);
        char* ak = array + k * sizeByte;


        if(LessThan(ak , value))
            i = k + 1;
        else
            j = k - 1;

        
        if(equal(ak , value))
            return k;
    }

    return size;
}



//The vector must obviously be sorted
char* BinarySearch_array(void* v1 , size_t size , size_t sizeByte , void* value , int(*LessThan)(void* , void*) , int(*equal)(void* , void*)){
    size_t index = BinarySearch_array_index(v1 , size , sizeByte , value , LessThan , equal);
    if(index == size)
        return NULL;
    
    return (char*) v1 + sizeByte * index;
}


//The vector must obviously be sorted
size_t BinarySearch_index(Vector* v1 , void* value , int(*LessThan)(void* , void*) , int(*equal)(void* , void*)){
    return BinarySearch_array_index(v1->data , v1->size , v1->stride , value , LessThan , equal);
}

//The vector must obviously be sorted
char* BinarySearch(Vector* v1 , void* value , int(*LessThan)(void* , void*) , int(*equal)(void* , void*)){
    return BinarySearch_array(v1->data , v1->size , v1->stride , value , LessThan , equal);
}






//The vector must obviously be sorted
size_t range_inf(Vector* v1 , void* value , int(*LessThan)(void* , void*) ){
    size_t i = 0;
    size_t j = v1->size - 1;

    if(v1->size < 2){
        return 0;
    }

    char* firstEl = vector_front(v1);
    char* lastEl = vector_back(v1);

    if(LessThan(firstEl , value) == FALSE){
        return 0;
    }
    else if( LessThan(lastEl , value) == TRUE ){
        return v1->size - 1;
    }

    while( j - i > 1 ){
        size_t k = (i + j + 1) / 2;
        char* ak = vector_get(v1 , k);
        
        if(LessThan(ak , value))
            i = k;
        else
            j = k;

    }
    if(LessThan(vector_get(v1 , j) , value) ){
        return j;
    }
    return i;

}



//The vector must obviously be sorted
size_t range_inf_array(void* v1 , size_t size , size_t sizeByte , void* value , int(*LessThan)(void* , void*) ){
    size_t i = 0;
    size_t j = size - 1;

    if(size < 2){
        return 0;
    }

    char* array = v1;
    char* firstEl = array;
    char* lastEl = array + (size - 1) * sizeByte;

    if(LessThan(firstEl , value) == FALSE){
        return 0;
    }
    else if( LessThan(lastEl , value) == TRUE ){
        return size - 1;
    }

    while( j - i > 1 ){
        size_t k = (i + j + 1) / 2;
        char* ak = array + sizeByte * k;
        
        if(LessThan(ak , value))
            i = k;
        else
            j = k;

    }
    if(LessThan(array + (sizeByte) * j , value) ){
        return j;
    }
    return i;

}


//Fill dest with elements which belongs to both v1 and v2 , without duplicates.
//v1 and v2 must be sorted
void set_intersection(Vector* dest , Vector* v1 , Vector* v2 , int (*lessThan)(void* , void*) ,  int (*equal)(void* , void*)){

    if(v1->size == 0 || v2->size == 0)
        return;

    // size_t ind1 = 0;
    // size_t ind2 = 0;

    size_t ind[2] = {0 , 0};
    char* val1;
    char* val2;
    char* valDest;

    while(ind[0] < v1->size && ind[1] < v2->size){
        val1 = vector_get( v1 , ind[0]);
        val2 = vector_get( v2 , ind[1]);

        if(equal(val1 , val2) && ( dest->size == 0 || equal( (valDest = vector_get(dest , dest->size - 1)) , val1) == FALSE ) ){
            vector_push_back(dest , val1);
            ind[0]++;
            ind[1]++;
            continue;
        } 

        int boolean = lessThan(val1 , val2) == TRUE;
        ind[!boolean]++;

    }

}




//Fill dest with elements which belongs to v1 or v2, without duplicates.
//v1 and v2 must be sorted
void set_union(Vector* dest , Vector* v1 , Vector* v2 , int(*lessThan) (void* , void* ) , int(*equal) (void* , void* ) ){

    size_t ind[2] = {0 , 0};
    char* val1;
    char* val2;
    char* valDest;

    while(ind[0] < v1->size && ind[1] < v2->size){
        val1 = vector_get( v1 , ind[0]);
        val2 = vector_get( v2 , ind[1]);

        int boolean = lessThan(val1 , val2) == TRUE;
        size_t* indexMin = &ind[!boolean]; 
        char* valMin = boolean ? val1 : val2;

        // if(dest->size == 0 || equal( (valDest = vector_get(dest , dest->size - 1)) , valMin) == FALSE ){
        //     vector_push_back(dest , valMin);
        // } 

        if(dest->size == 0 || equal( (valDest = vector_back(dest)) , valMin) == FALSE ){
            vector_push_back(dest , valMin);
        } 

        (*indexMin)++;
    }

    Vector* vRem = ind[0] == v1->size ? v2 : v1;
    size_t ii = ind[0] == v1->size ? 1 : 0;

    while( ind[ii] < vRem->size ){
        char* val = vector_get(vRem , ind[ii]);
        if(dest->size == 0 ||  equal( (valDest = vector_get(dest , dest->size - 1)) , val) == FALSE )
            vector_push_back(dest , val);

        ind[ii]++;
    }
}




int is_inside( Vector* v1 , void* val , int(*equal)(void* , void*)){

    for(int i = 0; i < v1->size; i++){
        if(equal(vector_get(v1 , i) , val) == TRUE)
            return TRUE;

    }
    return FALSE;
}







#endif