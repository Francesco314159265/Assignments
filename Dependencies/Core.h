


#ifndef CORE_H
#define CORE_H


//Here we define these macros just to automate a little the process of allocating temporary arrays
//In theory the use of alloca is discouraged, cause it's not really portable (it's not in C++ standard
//and i'm not sure about C but i'm pretty sure it's not even in any C standard), but it's implemented by
//most compilers (gcc and MSVC by experience , clang most certainly, never used it). 
//In theory C11 makes the Variable length arrays standard in order to discourage alloca
//but if i'm not mistaken after C11 they became optional as well, so i just stick with alloca.
//Alloca can be really useful in certain situations, most of all when you have to allocate multiple times
//little arrays of length not known at compile time (you could use arrays of max size, which have an edge over alloca
//in certain cases at the drawback of allocating more size that needed, so choosing beetween them is 
//case dependent) whitout having to do a syscall to heap allocate it thorugh malloc or derived. Performance-wise, it just
//moves the stack pointer up to sizeBytes, effectively allocating an empty slot of memory in the stack which is
//garanted to not be used(as the stack pointer is moved after it), and is also garanted to be automatically
//freed when the function is exited(because the stack pointer is restored at the position in which was when the
//function was called), so it's important to not call free on pointers allocated with alloca.
//Also very important, never force-inline a function containing it, or if the function containing it is in a for loop,
//there will be a stack allocation for each iteration (easy to stack overflow this way).
//Compilers do not inline functions containing alloca, and beacuse inling can be quite effectively as an optimization
//(not just as itself nowadays, a function call is not that expensive in most of the situations nowadays , 
//but in what the compiler can optimize AFTER inling), it's use should be restricted in allocating temporary
//arrays of little size in functions which will be called frequently, if performance is a concern, always profile
//cause could be more effective inlining the function
//Fixed size arrays get deallocated at the end of a SCOPE, not at the return of a function, so they are safer in this aspect
//and using them let's the compiler optimize, example:

// void f(){
//     int array[MAX_SIZE_BUFFER];
//     //do some stuff;
// }

// int main(){

//     for(int i = 0; i < 10000; i++){
//         f();
//     }
// }

//if the function is a good candidate fo inlining, the compiler will generate this function



// int main(){

//     for(int i = 0; i < 10000; i++){
//         int array[MAX_SIZE_BUFFER];
//         //do some stuff
//     }
// }


//THEN, it will most certainly move the array allocation out of the loop, effectively using the same 
//memory over and over AND, having the possibility to also optimize in the loop logic the "do some stuff"
//part, which can be quite effetcive performance-wise


//with alloca, the major draw-back is that the function will not be inlined, and if force inlined,
//will look like this

// int main(){

//     for(int i = 0; i < 10000; i++){
//         int* array = alloca(sizeof(int) * sizeNeeded);
//         //do some stuff
//     }
// }

//now alloca allocate stack array which will not be deallocated on each iteration, so at each iteration, an
//array will be allocated and not deallocated, and at loop done, we will jave 10000 arrays allocated on the stack
//which will not be freed until the function exit (easy to stack overflow in this way)

#define MAX_ALLOC_STACK 10000
#define allocate_internal_var(size , sizeByte) size * sizeByte >= MAX_ALLOC_STACK ? malloc(size * sizeByte) : alloca(size * sizeByte)
#define deallocate_internal_var(pointer , count , sizeByte) if(count * sizeByte >= MAX_ALLOC_STACK) free(pointer)


#define TRUE 1
#define FALSE 0


#define PI 3.141592653586



int LessThan_int(void* a1, void* a2){
    return *(int*) a1 < *(int*) a2;
}

int Equal_int(void* a1, void* a2){
    return *(int*) a1 == *(int*) a2;
}

int LessThan_double(void* a1, void* a2){
    return *(double*) a1 < *(double*) a2;
}

int Equal_double(void* a1, void* a2){
    return *(double*) a1 == *(double*) a2;
}



#endif