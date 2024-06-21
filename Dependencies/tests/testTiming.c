
#include "time.h"
#include "../sort.h"
#include "../Vector.h"

int main(){
    const size_t N = 1e7;
    Vector* v = make_vector_with_capacity(sizeof(int) , N);

    for(int i = 0; i < N; i++){
        int val = rand();
        vector_push_back(v , &val );
    }

    clock_t t1 = clock() / (CLOCKS_PER_SEC / 1000);
    
    for(int i = 0; i < 20; i++){
        sort(v->data , v->size , v->stride , LessThan_int);
    }

    clock_t t2 = clock() / (CLOCKS_PER_SEC / 1000);
    printf("%d" , t2 - t1);



}