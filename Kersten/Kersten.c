#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "../Dependencies/Randm.h"
#include "../Dependencies/Vector.h"
#include "../Dependencies/sort.h"
#include "../Dependencies/SetOp.h"
#include "src/BasicAlgebra.h"
#include "src/Fit.h"


typedef struct{
    size_t N;

    double T;
    double X0;
    double L;
    double m;
    double dt;

} KerstenParam;



void KerstenModel( double* dest , KerstenParam* param){

    const double L = param->L;
    const double T = param->T;
    const size_t N = param->N;
    const double X0 = param->X0;
    const double m = param->m;


    const double dt = (T - 0) / (N - 1);
    param->dt = dt;

    const double std = sqrt(2 * dt);

    double* x = dest;
    x[0] = X0;

    for(int i = 0; i < N - 1; i++){
        if( x[i] < L) {
            
            // x[i + 1] = x[i];
            //Do not need to continue, it will always hit this branch from now on
            x[N - 1] = x[i];
            break;
        }
        else{
            x[i + 1] = x[i] * ( 1 - (m + gaussDraw(0 , std) ) );
        } 
    } 


}


Vector* Repeat( const size_t runs , KerstenParam* param){

    Vector* values = make_vector_with_capacity(sizeof(double) , param->N );
    double* x0 = malloc(sizeof(double) * param->N);
    for(int i = 0; i < runs; i++){
        KerstenModel(x0 , param);
        double newVal = x0[param->N - 1];
        //Should we discard values under L? 
        if(newVal < param->L){
            i--;
            continue;
        }
        vector_push_back(values , &newVal);
    } 
    free(x0);
    return values;
}









int main(){

    // const size_t N = 2000;
    const size_t N = 2000;
    // const double L = 0.1;
    const double L = 17.0;
    const double T = 1;

    // const double X0 = 2.0;
    const double X0 = 20.0;
    // const double m = -0.001;
    const double m = -1e-6;
    const size_t runs = 10000;

    KerstenParam parameters = { 
        .L = L , 
        .m = m , 
        .N = N , 
        .T = T , 
        .X0 = X0,
        .dt = (double) T / (N - 1)
        };


    //We Simulate the Kersten Model number = runs times, selecting each time
    //the last value assumed on each run.
    //Here x contains all the values which have been sampled, it's of size = runs
    printf("Simulating: ");
    Vector* x = Repeat(runs , &parameters);
    printf("Done, number samples : %d\n" , 10000);


    //We sort here in a copy just to not do it multiple times
    //FitPowerLaw searches for the optimal parameters xMin and alpha
    //See the comment in the function for more info
    Vector* xSorted = copy_vector(x);
    sort(xSorted->data , xSorted->size , xSorted->stride , LessThan_double);
    PowerLawParam pPower = FitPowerLaw(xSorted->data , runs , 1e-10);

    //PowerLaw found a fit in the data that ranges from [pPower.xMin , max(x)]
    //Inspecting the histogram plot of the data in python seemed data in the range
    // [min(x) , pPower.xMin] a good fit could be a logNormal distribution
    //Now we seek to find a fit in the range [min(x) , pPower.xMin] for the logNormal
    //distribution.
    //We find the index position of xMin in the sorted array, and use it in order to
    //clamp the x array in the range given above, over which logNormal Fit will search
    //for parameters.
    size_t q = BinarySearch_index(xSorted , &pPower.xMin , LessThan_double , Equal_double);
    LogNormalParam pLog = FitLogNormal(xSorted->data , q , 1e-10);


    // Just writing into a file for python visualizations
    FILE* file = fopen("files/data.txt" , "w");
    if(file == NULL){
        perror("Unable to open output file");
        return -1;
    }

    fprintf( file , "%d\n" , runs);
    fprintf( file , "%lf , %lf\n" , pPower.xMin , pPower.alpha);
    fprintf( file , "%lf , %lf\n" , pLog.mean , pLog.std);

    for(int i = 0; i < runs; i++){
        fprintf(file , "%lf\n" , *(double*)vector_get(x , i));
    }
    fclose(file);
    return 0;
}