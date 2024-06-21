




#ifndef DISTRIBUTIONS_H
#define DISTRIBUTIONS_H


#include <math.h>
#include "../../Dependencies/Core.h"




typedef struct{
    double xMin;
    double alpha;

} PowerLawParam;

typedef struct{
    double mean;
    double std;

} LogNormalParam;









double PowerLaw_evaluate( double X , double XMin , double alpha){
    return (alpha - 1) / XMin * pow(X / XMin , -alpha);
}

double PowerLaw_cumulative_evaluate( double X , double XMin , double alpha){
    return 1 - pow(X / XMin , - alpha + 1);
}

void PowerLaw_gradient( double* f , double* fprime , double* fsecond , const double X , const double XMin ,const double alpha){
    
    double temp = pow(X / XMin , -alpha) / XMin;
    double log_ = log(X / XMin);
    *f = (alpha - 1) * temp;  
    *fprime = *f * ( 1 / ( alpha - 1) - log_);
    *fsecond = - *f / ((alpha - 1) * (alpha - 1)) + *fprime * ( 1 / ( alpha - 1) - log_);
}


double LogNormal_evaluate( double X , double mean , double std){
    double normalized = (log(X) - mean)/std;
    return 1 / (X * sqrt(2 * PI) * std) * exp(- normalized * normalized / 2);
}

double LogNormal_cumulative_evaluate( double X , double mean , double std){
    double normalized = (log(X) - mean) / std;
    return 0.5 + 0.5 * erf(normalized / sqrt(2));
}

void LogNormal_gradient( double* f , double fprime[2] , double fsecond[2][2] , const double X , const double mean ,const double std){
    
    double normalized = (log(X) - mean) / std;
    *f = 1 / (X * sqrt(2 * PI) * std) * exp(- ( normalized * normalized / 2) );
    double q = normalized / std;
    fprime[0] = q * *f;
    fprime[1] = *f * (normalized * q - 1) ;

    fsecond[0][0] = ( q * q - 1 / (std * std)  ) * *f; 
    fsecond[1][1] = *f * ( (q * normalized - 1) * (q * normalized - 1) - 3 * q * q );
    fsecond[0][1] = *f * q  * ( - 2 / std + q * normalized - 1);
    fsecond[1][0] = fsecond[0][1];
}
















void LogNormal_estimate_param(const double* const yii , size_t size , LogNormalParam* p){

    double mean = 0;
    double std = 0;

    for(int i = 0; i < size; i++){
        double logX = log(yii[i]);
        mean += logX;
        std += logX * logX;
    }

    mean /= size;
    std = sqrt(std / size - mean * mean);
    p->mean = mean;
    p->std = std;
}


#endif