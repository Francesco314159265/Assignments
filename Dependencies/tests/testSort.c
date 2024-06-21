#include "../sort.h"
#include "../SetOp.h"
#include "../Vector.h"
#include <inttypes.h>
#include <stdio.h>

// void print_vector_int(int* arr , size_t size){
//     printf("( ");
//     for(int i = 0; i < size; i++){
//         if(i < size - 1)
//             printf("%d , " , arr[i]);
//         else
//             printf("%d " , arr[i]);
//     }
//     printf(" )");
// }




int LessThan_char(void* v1 , void* v2){
    return *(char*) v1 < *(char*) v2;
}

int Equal_char(void* v1 , void* v2){
    return *(char*) v1 == *(char*) v2;
}






int main(){
    size_t N1 = 10;
    Vector* v1 = make_vector(sizeof(int));

    size_t N2 = 10; size_t N3 = 20;
    Vector* v2 = make_vector(sizeof(int));

    Vector* v3 = make_vector(sizeof(int));
    Vector* v4 = make_vector(sizeof(int));


    for(int i =0; i < N1; i++){
        int k = rand();
        vector_push_back(v1 , &k);
    }

    for(int i =0; i < N2; i++){
        int k = rand();
        vector_push_back(v2 , &k);
    }



    *(int*)vector_get(v1 , 2) = *(int*)vector_get(v2 , 4);
    *(int*)vector_get(v1 , 7) = *(int*)vector_get(v2 , 1);
    *(int*)vector_get(v1 , 9) = *(int*)vector_get(v2 , 6);

    printf("v1: "); print_vector_int(v1); printf("\n");
    printf("v2: "); print_vector_int(v2); printf("\n");

    sort(v1->data , v1->size , v1->stride , LessThan_int );
    sort(v2->data , v2->size , v2->stride , LessThan_int );

    set_union(v3 , v2 , v1 , LessThan_int , Equal_int);
    set_intersection(v4 , v2 , v1 , LessThan_int , Equal_int);
    printf("v1: "); print_vector_int(v1); printf("\n");
    printf("v2: "); print_vector_int(v2); printf("\n");
    printf("union v3: "); print_vector_int(v3); printf("\n");
    printf("intersection v4: "); print_vector_int(v4); printf("\n");
    int value = 2995;
    int* ind = (int*) BinarySearch(v3 , &value , LessThan_int , Equal_int);

    Vector* vNew = make_vector(sizeof(double));
    double boo = 2;
    vector_push_back(vNew , &boo );
    print_vector_double(vNew); printf("\n");


}