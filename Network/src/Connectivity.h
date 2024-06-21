

#ifndef CONNECTIVITY_H
#define CONNECTIVITY_H

#define MAX_LINE_LENGTH 500






typedef struct {
    size_t cI;
    size_t cO;
    size_t cIO;
    size_t cTOT;
    
} ConnectivityInt;


typedef struct {
    double cI;
    double cO;
    double cIO;
    double cTOT;
} ConnectivityDouble;

typedef struct {
    ConnectivityInt max;
    ConnectivityInt min;

    ConnectivityDouble mean;
    ConnectivityDouble variance;

} ConnectivityData;


ConnectivityData make_ConnectivityData(){
    ConnectivityData d = { 
        .max.cI = 0,
        .min.cI = INT64_MAX,
        .min.cO = INT64_MAX,
        .min.cIO = INT64_MAX,
        .mean.cI = 0,
        .variance.cI = 0
     };

    return d;
}





#endif
