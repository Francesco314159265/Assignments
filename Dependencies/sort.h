


#ifndef SORT_H
#define SORT_H


#include <stdio.h>
#include <inttypes.h>
#include <malloc.h>
#include <memory.h>
#include "math.h"
#include "Core.h"
#include "Vector.h"


#define NINT 4



void __swap(void* el1 , void* el2 , size_t sizeByte , void* temp){
    memcpy(temp , el1 , sizeByte);
    memcpy(el1 , el2 , sizeByte);
    memcpy(el2 , temp , sizeByte);
}

void swap_implace(void* el1 , void* el2 , size_t sizeByte ){
    void* temp = allocate_internal_var(1 , sizeByte);
    memcpy(temp , el1 , sizeByte);
    memcpy(el1 , el2 , sizeByte);
    memcpy(el2 , temp , sizeByte);
}

//Base sorting algorithm
void sortNsquared(void* a1 , size_t size1 , size_t sizeByte , int (*LessThan)(void* , void*)){

    char* a = (char*) a1;
    char* temp = allocate_internal_var(1 , sizeByte);
    // char temp[255];
    for(int i = 0; i < size1; i++){
        char* ai = a + i * sizeByte;
        for(int j = i + 1; j < size1; j++){
            char* aj = a + j * sizeByte;
            if(LessThan( aj , ai ) == TRUE){
                //Swap the two
                __swap(ai , aj , sizeByte , temp);
            }
        }
    }
    deallocate_internal_var(temp , 1 , sizeByte);
}




//Merges two sorted arrays into dest.
//if dest doesn't alias a1, pass _secure = TRUE (1), else pass FALSE (0)
void __mergeSort(void* _dest , int _secure , void* a1 , size_t size1 , void* a2 , size_t size2 , size_t sizeByte, int(*LessThan)(void* , void*) ){

    size_t i1 = 0;
    size_t i2 = 0;
    size_t k = 0;
    size_t size = size1 + size2;


    char* dest = _dest;
    char* arr1 = _secure == FALSE ? allocate_internal_var(size1 , sizeByte) : a1;
    char* arr2 = a2;


    if(_secure == FALSE){
        memcpy(arr1 , a1 , size1 * sizeByte);
    }



    while(k < size){
        char* ai = arr1 + i1 * sizeByte;
        char* aj = arr2 + i2 * sizeByte;
        if( i1 != size1 && (i2 == size2 || LessThan( ai , aj ) == TRUE)){
            memcpy(dest + k * sizeByte , ai , sizeByte);
            i1++; 
        }
        else{
            memcpy(dest + k * sizeByte , aj , sizeByte);
            i2++; 
        }
        k++;
    }

    
    if(_secure == FALSE){
        deallocate_internal_var(arr1 , size1 , sizeByte);
    }

}





//For marging the arrays implace, without copying one of the arrays in case _secure == FALSE,
//Performance-wise absolutely terrible , didn't even finish the timing test(1min+) against
//the other one(which had a mean of 240 ms for 1e6 random int elements)
void __mergeSort_into1(void* a1 , size_t size1 , void* a2 , size_t size2 , size_t sizeByte, int(*LessThan)(void* , void*) ){

    size_t i1 = 0;
    size_t i2 = 0;
    size_t k = 0;
    size_t size = size1 + size2;


    char* dest = a1;
    char* arr1 = a1;
    char* arr2 = a2;

    char temp[255];


    while( i1 < size1){
        char* ai = arr1 + i1 * sizeByte;
        char* aj = arr2 + i2 * sizeByte;
        if( i1 < size1 && (i2 == size2 || LessThan( ai , aj ) == TRUE)){
        }
        else{
            __swap(ai , aj , sizeByte , temp);

            char* aq = aj;
            char* a2_last = arr2 + (size2 - 1) * sizeByte;
            while( aq != a2_last && LessThan( aq , aq + sizeByte) == FALSE ){
                __swap(aq , aq + sizeByte , sizeByte , temp);
                aq += sizeByte;
            }
            // i2++;
        }
        i1++;
    }

    

}


void sort(void* array , size_t size , size_t sizeByte , int (*LessThan)(void* , void* )){


    //We must divide our array in chuncks of size NINT, (default 4).
    //Now, the division should be such that in this pass we will operate on a 
    //a power of two of intervals.
    //Let's say our array is 50 integers long, and we want to apply sort merge to it.
    //Let's say that we just 
    size_t NintervalsPowNINT = floor(log2( (double) size / NINT));
    size_t NInt = pow( 2 , NintervalsPowNINT);
    size_t nRemaining = size - NInt * NINT;


    //Not enough chuncks to procede, array is wuite small, we can use the base implementation 
    // and return
    if(NInt <= 1){
        sortNsquared(array , size , sizeByte , LessThan);
        return;
    }

    //Let's sort all the chunck
    char* a = array;
    for(int i = 0; i < NInt; i++){
        char* curri = a + i * sizeByte * NINT;
        sortNsquared(curri , NINT , sizeByte , LessThan);
    }

    //Here we repetedly merge the array by __mergesort, which merges two sorted arrays
    //mantaing the order. Now , __mergeSort takes a void* _dest pointer to the destination
    //on which we will write the results, by using the _secure = 0 parameter we
    //explicitly say that _dest will not be an alis for a1 and a2, meaning _dest is not over
    //lapping with neither of the two (particularly with a1, as the arrays are filled form bottom to top
    //so the first positions that will be overwritten will be of a1, never of a2). If transversed in
    //the opposite order will be exactly the opposite(and _dest should not alias a2, but the reverse 
    //iteration have not been implemented so no reason to think about it now)
    size_t Ni = NInt;
    size_t sizeI = NINT;

    while(Ni > 1){
        for(int i = 0; i < Ni - 1; i+= 2){
            char* a1 = a + sizeI * sizeByte * i;
            char* a2 = a1 + sizeByte * sizeI;
            __mergeSort(a1 , FALSE , a1 , sizeI , a2 , sizeI , sizeByte , LessThan);
            // __mergeSort_into1(a1 , sizeI , a2 , sizeI , sizeByte , LessThan);
        }    

        Ni /= 2;
        sizeI *= 2;
    }



    //The unsorted chuncks remaining will be sorted again by recursion
    //Remember that we effectively sorted the most big power of two of chuncks
    //contained inside the size of the array, so the number of stack calls is
    //at most log2(n)
    //Only thing to consider here is to follow alloca. It should absolutely not be used
    //inside this function, or the stack pointer would be moved on each
    //recursive call and not be freed until the last recursive calls return, so
    //we could easily stackoverflow.
    //Functions which uses alloca should absoulely not be inlined by the compiler
    //Or at the first loop we are good to crash. compilers usually do not inline functions
    //which contains alloca, even if the inline keyword is present( unless forced by keywords 
    //compiler dependent like __FORCE_INLINE , but any good compiler would at least give a warning
    //during compilation for inlining a function containing alloca.
    if(nRemaining != 0){
        char* arr_Rem = a + (size - nRemaining) * sizeByte;
        sort(arr_Rem, nRemaining , sizeByte , LessThan);
        __mergeSort(a , FALSE , a , size - nRemaining , arr_Rem  , nRemaining , sizeByte , LessThan);
    }

}








#endif