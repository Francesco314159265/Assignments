

#include "../sort.h"
#include "../SetOp.h"



int main(){

    Vector* v = make_vector(sizeof(int));

    for(int i = 0; i < 100; i++){
        int q = rand();
        vector_push_back(v , &q);
    }

    sort(v->data , v->size , v->stride , LessThan_int);

    int s = rand() % 100;
    int* s0 = (int*) vector_get(v , s);
    int val = *s0;
    size_t ind = BinarySearch_index(v , &val , LessThan_int , Equal_int);

    int s1 = -10;
    size_t ind2 = BinarySearch_index(v , &s1 , LessThan_int , Equal_int);
}