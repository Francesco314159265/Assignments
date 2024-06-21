#include "stdio.h"
#include "../SetOp.h"
#include "../Vector.h"


int main(){
    const size_t N = 10;
    Vector* v1 = make_vector(sizeof(int));
    for(int i = 0; i < N; i++){
        int n = rand();
        vector_push_back(v1 , &n);
    }

    Vector* v2 = make_vector(sizeof(int));
    for(int i = 0; i < N; i++){
        int n = rand();
        vector_push_back(v2 , &n);
    }

    Vector* v3 = make_vector(sizeof(int));

    set_union(v3 , v1 , v2 , LessThan_int , Equal_int);
    delete_vector(v3);
}