


#ifndef FIT_H
#define FIT_H



#include <stdio.h>
#include <stdlib.h>
#include "BasicAlgebra.h"
#include "Distributions.h"









double testScore(const double* const yii , size_t size , double alpha , double XMin){

    double _max = -INFINITY;
    for(int i = 0; i < size; i++){
        double fi = PowerLaw_cumulative_evaluate(yii[i] , XMin , alpha);
        double fhati = (double) i / size;
        double dist = fabs(fi - fhati);
        _max = _max > dist ? _max : dist ;
    }

    return _max;
}



typedef enum{
    CONVERGED,
    DIVERGED,
    MAX_IT
} ConvergingType;









/*

WARNING : ARRAY DATA MUST BE ALREADY SORTED
This function try to fit the incoming observed data in the yii array into a power Law distribution 
of the form p(x) = (alpha - 1) / XMin * (X / XMin)**(-alpha)

Where alpha and XMin are the objective parameters of the fit
It computes them by minimizing the log likehood estimator using Newton method
We iterate over the data, restricting the data in the range [XMin , DataMax] , where XMin is at each iteration
given by yii[i], and by computing alpha minimizing the log likehood function given XMin.

The fit is tested by the Kolmogorov-test, and at the end the set of parameters {XMin , alpha} with
the lowest score is selected.

The Newton method minimize the log-likehood searching for zeros of it's derivative, and so computes
the new estimake alpha[k+1] = alpha[k] - f'(XMin , alpha[k]) / f"(XMin , alpha[k])


One easy way to speed up computation, is by transversing the search of the xMin in reverse, meaning starting from 
the higher values, going down to the lower ones.

This could save us time from doing inefficient operations in the first range (where the method doesn't converge almost all the times,
and the computation is also slowed down by the nan math)
I have tried a version in which you do this, and you stop when you first encounter your first divergence,
but it wasn't as robust as this one, beacuse even at high xMin you could encounter problems in convergence, so sometimes 
it would have ended prematurely the search, finding an xMin too high.
Probably the rigth solution would be to not stop the search when we encounter the first divergence problem ( so we check all of the possible
xMins) , but we check at each inner loop iteration (the one which adds up all the contribution from each i-sample) if
we have encountered nan problems, in case stopping the loop and going to the next one xMin candidate.
This solution should be better thinking about it, but for now i will stick with this
*/


PowerLawParam FitPowerLaw( const double* const yjj, const size_t size , const double epsilon){

    //We sort the incoming data in ascending order. This is useful when iterating over the entire (almost)
    //array when using yii[q] as the current XMin , 
    //as we can create a restriction of the data of type [XMin , DataMax]
    //by simply offsetting the yii pointer by the index of XMin (q), 
    //viewing the subarray the array [yii + q , yii + size - 1] 


    double minScore = INFINITY;
    const size_t MaxIteration = 20 , maxRestriction_size = 10;
    PowerLawParam p;




    printf("Data Ranges in the interval [%lf , %lf]\n" , yjj[0] , yjj[size - 1] );
    printf("Searching for optimal parameters for Power law Fitting:\n\n");
    ConvergingType converged = DIVERGED;
    double alpha;

    for(int q = 0; q < size - maxRestriction_size; q++){

        //We use as starting point the last converged one
        
        alpha = 2 + (double) rand() / RAND_MAX;


        double XMin = yjj[q];
        size_t iteration = 0;
        converged = DIVERGED;

        while(converged != CONVERGED && iteration < MaxIteration){
            double f = 0 , fprime = 0 , fsecond = 0;
            double fi , fprimei , fsecondi;

            for(int i = q; i < size; i++){
                const double yi = yjj[i];

                PowerLaw_gradient(&fi , &fprimei , &fsecondi , yi , XMin , alpha);

                f += log(fi);
                fprime += 1 / fi * fprimei;
                fsecond += - (1 / fi * fprimei) * ( 1 / fi * fprimei) + 1 / fi * fsecondi;
            }

            double xkNew = alpha - fprime / fsecond;

            if(isnan(xkNew)){
                converged = DIVERGED;
                break;
            }

            if( fabs(xkNew - alpha) < epsilon )
                converged = CONVERGED;

            alpha = xkNew;
            iteration++;
        }

        if(iteration == MaxIteration && converged != CONVERGED){
            converged = MAX_IT;
        }

        
    
        //The iteration converged, we test the fit and update the best parameters if they happen to
        //score the current best result
        if(converged == CONVERGED){
            double score = testScore(yjj + q , size - q , alpha , XMin);
            if(score < minScore){
                minScore = score;
                p.alpha = alpha;
                p.xMin = XMin; 
                printf("Current best = XMin : %lf , alpha : %lf , Newton iterations : %d , score : %10.5e , progress : %3d %% \n" , 
                    XMin , 
                    alpha , 
                    iteration , 
                    score , 
                    (int) floor( (double) (size - maxRestriction_size - q) * 100 / size)
                );
            }
        }
        
    }

    
    printf("Done\n");
    return p;
}







//WARNING : ARRAY DATA MUST BE ALREADY SORTED
LogNormalParam FitLogNormal( const double* const yii, const size_t size , const double epsilon){

    //We sort the incoming data in ascending order. This is useful when iteraing over the entire (almost)
    //array when using yii[q] as the current XMin , 
    //as we can create a restriction of the data of type [XMin , DataMax]
    //by simply offsetting the yii pointer by the index of XMin (q), 
    //viewing the subarray the array [yii + q , yii + size - 1] 


    const size_t MaxIteration = 20 , maxRestriction_size = 10;
    LogNormalParam p;

    LogNormal_estimate_param(yii , size , &p);


    printf("Searching for optimal parameters for Log Normal Fitting:\n\n");
    printf("Range of data for fitting is : [ %lf , %lf ]\n" , yii[0] , yii[size - 1]);


    size_t iteration = 0;
    double alpha[2] = {p.mean , p.std  };

    printf("Starting newton : mean %1.7e , std : %1.7e\n" , alpha[0] , alpha[1]);

    int converged = 0;
    int nanDetected = 0;
    while(!converged && iteration < MaxIteration){
        double mean = alpha[0];
        double std = alpha[1];


        double f = 0;
        double fprime[2] = {0 , 0};
        double fsecond[2][2] = {
            {0 , 0},
            {0 , 0}
        };

        double fi;
        double fprimei[2];
        double fsecondi[2][2];

        for(int i = 0; i < size; i++){
            const double yi = yii[i];

            LogNormal_gradient(&fi , fprimei , fsecondi , yi , mean , std);

            f += log(fi);

            multiply_scalar_impl(1 / fi , fprimei);
            add_into_vec(fprime , fprimei);

            double outerP[2][2];
            outer_vec22(outerP , fprimei , fprimei);
            scalar_multiply_matrix_implace( - 1 , outerP);

            scalar_multiply_matrix_implace(1 / fi , fsecondi);

            add_matrix22_implace(fsecond , outerP);
            add_matrix22_implace(fsecond , fsecondi);
        }

        double inverseM[2][2];
        double diff[2];
        inverse_mat22(inverseM , fsecond);
        multiply_matrix_vec22(diff , inverseM , fprime );


        if(isnan(diff[0]) || isnan(diff[1])){
            iteration = MaxIteration;
            nanDetected = TRUE;
            break;
        }

        double error = sqrt( diff[0] * diff[0] + diff[1] * diff[1] ) ;
        if( error < epsilon / size )
            converged = 1;

        sub_vec2_into_first(alpha , diff);
        printf("Newton it : %d , f : %2.6e , error : %2.6e , mean %1.7e , std : %1.7e\n" , iteration , f , error , alpha[0] , alpha[1]);
        iteration++;
    }

    if(!converged){
        printf("converged : false , reason : %s\n" , nanDetected ? "nan" : "Max number iterations" );
    }
    else{
        printf("Done , converged : %s , to : [ mean : %1.7e , std : %1.7e ]\n" , converged ? "true" : "false" , p.mean , p.std);
    }
    p.mean = alpha[0];
    p.std = alpha[1];

    return p;
}








#endif