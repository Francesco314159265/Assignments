

#ifndef RANDM_H
#define RANDM_H

#include <stdlib.h>
#include <math.h>
#include "Core.h"




typedef struct{
    double cached;
    int ready;
} __GaussCachedData;


double gaussDraw(const double mean , const double std){

    static __GaussCachedData d = { .cached = 0 , .ready = FALSE};

    if( d.ready ){
        d.ready = FALSE;
        return d.cached * std + mean;
    }

    double u , v , s = 0;

    while( s <= 0 || s >= 1){
        u = (double) rand() / RAND_MAX;
        v = (double) rand() / RAND_MAX;

        u = ( u - 0.5 ) * 2;
        v = ( v - 0.5 ) * 2;
        s = u * u + v * v;
    }   

    double fact = sqrt( - 2 * log(s) / s );

    double z1 = u * fact;
    double z2 = v * fact;

    d.cached = z2;
    d.ready = TRUE;
    return z1 * std + mean ;
}   




#endif