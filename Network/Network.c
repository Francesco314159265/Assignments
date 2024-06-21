#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <assert.h>
#include "../Dependencies/Vector.h"
#include "../Dependencies/sort.h"
#include "../Dependencies/SetOp.h"
#include "src/Nodelist.h"
#include "src/Connectivity.h"


#define min(a,b) (((a) < (b)) ? (a) : (b))
#define max(a,b) (((a) > (b)) ? (a) : (b))


typedef struct{
    double range[2];
    size_t frequency;
} Bin;


typedef struct {
    Bin* I;
    Bin* O;
    Bin* IO;
    Bin* TOT;
} BinsByClass;

void delete_binsByClass(BinsByClass* bins){
    free(bins->I);
    free(bins->O);
    free(bins->IO);
    free(bins->TOT);
    free(bins);
}

BinsByClass* make_bins(size_t nBins){
    BinsByClass* b = malloc(sizeof(BinsByClass));
    Bin* BinI = malloc(sizeof(Bin) * nBins);
    Bin* BinO = malloc(sizeof(Bin) * nBins);
    Bin* BinIO = malloc(sizeof(Bin) * nBins);
    Bin* BinTOT = malloc(sizeof(Bin) * nBins);

    return b;
}


int LessThan_Bin(void* bin1 , void* value){
    return *((double*) value) > ((Bin*) bin1)->range[0]; 
}

int Equal_Bin(void* bin1 , void* value){
    return ((Bin*) bin1)->range[0] == *((double*) value); 
}


size_t get_Bin_containing_value(Vector* bins , double value){

    size_t index = range_inf(bins , &value , LessThan_Bin );
    return index;
}




BinsByClass* constructIstogram(NodeList* nodelist , ConnectivityData* cData , size_t nBins){

    BinsByClass* _BinsByClass = make_bins(nBins);
    
    Vector* BinI = make_vector_with_capacity(sizeof(Bin) , nBins);
    Vector* BinO = make_vector_with_capacity(sizeof(Bin) , nBins);
    Vector* BinIO = make_vector_with_capacity(sizeof(Bin) , nBins);
    Vector* BinTOT = make_vector_with_capacity(sizeof(Bin) , nBins);

    _BinsByClass->I = BinI->data;
    _BinsByClass->O = BinO->data;
    _BinsByClass->IO = BinIO->data;
    _BinsByClass->TOT = BinTOT->data;

    Vector* BinsType[4] = {BinI , BinO , BinIO , BinTOT};

    double h[4] = {
        ( (double) cData->max.cI  - (double) cData->min.cI)  / (nBins ) ,
        ( (double) cData->max.cO  - (double) cData->min.cO)  / (nBins ) , 
        ( (double) cData->max.cIO - (double) cData->min.cIO) / (nBins ) ,
        ( (double) cData->max.cTOT - (double) cData->min.cTOT) / (nBins ) ,
    };


    for(int i = 0; i < nBins; i++){
        for(int j = 0; j < 4; j++){
            
            Bin currBin;
            currBin.range[0] = h[j] * i;
            currBin.range[1] = h[j] * (i + 1);
            currBin.frequency = 0;
            vector_push_back(BinsType[j] , &currBin);
        }
    }


    Vector* temp1 = make_vector(sizeof(int));
    Vector* temp2 = make_vector(sizeof(int));
    size_t nNodes =  NodeList_get_number_nodes(nodelist);
    for(int i = 0; i < nNodes; i++){
        Node* node = Nodelist_get_node(nodelist , i);

        vector_clear(temp1);
        vector_clear(temp2);
        set_intersection(temp1 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);
        set_union(temp2 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);

        size_t dimensions[4] = {
            node->list[IN]->size,
            node->list[OUT]->size,
            temp1->size,
            temp2->size,
        };


        int indII[4];
        for(int j = 0; j < 4; j++){
            indII[j] = get_Bin_containing_value(BinsType[j] , (double) dimensions[j]);   
            Bin* bin = (Bin*) vector_get(BinsType[j] , indII[j]);
            bin->frequency += 1;
        }




    }
    delete_vector(temp1);
    delete_vector(temp2);

    for(int i = 0; i < 4; i++){
        BinsType[i]->data = NULL;
        delete_vector(BinsType[i]);
    }
    return _BinsByClass;
    
}

void cMin(ConnectivityInt* cData , size_t in , size_t out , size_t IO , size_t TOT){
    cData->cI = min(cData->cI , in);
    cData->cO = min(cData->cO , out);
    cData->cIO = min(cData->cIO , IO);
    cData->cTOT = min(cData->cTOT , TOT);
}

void cMax(ConnectivityInt* cData , size_t in , size_t out , size_t IO , size_t TOT){
    cData->cI = max(cData->cI , in);
    cData->cO = max(cData->cO , out);
    cData->cIO = max(cData->cIO , IO);
    cData->cTOT = max(cData->cTOT , TOT);
}



int main(){


    NodeList* nodelist = parseFile("files/Network.txt");
    if(nodelist == NULL){
        return -1;
    }

    finalize_nodelist(nodelist); //Once all the nodes are in, we can finalize it to make it more compatible for analysis
    print_nodeList(nodelist);

    size_t nNodes = NodeList_get_number_nodes(nodelist);
    ConnectivityData cData = make_ConnectivityData();

    Vector* v1 = make_vector(sizeof(int));
    Vector* v2 = make_vector(sizeof(int));
    for(int i = 0; i < nNodes; i++){
        Node* node = Nodelist_get_node(nodelist , i);

        vector_clear(v1);
        vector_clear(v2);
        //the IN and OUT list must be sorted, this is done in finalize, so if nodes have to be added, 
        //finalize should be delayed until all the nodes have been inserted.
        set_intersection(v1 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);
        set_union(v2 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);

        cMin(&cData.min , node->list[IN]->size , node->list[OUT]->size , v1->size , v2->size);
        cMax(&cData.max , node->list[IN]->size , node->list[OUT]->size , v1->size , v2->size);
  
        cData.mean.cI += node->list[IN]->size;
        cData.mean.cO += node->list[OUT]->size;
        cData.mean.cIO += v1->size;
        cData.mean.cTOT += v2->size;

        cData.variance.cI += node->list[IN]->size * node->list[IN]->size;
        cData.variance.cO += node->list[OUT]->size * node->list[OUT]->size;
        cData.variance.cIO += v1->size * v1->size;
        cData.variance.cTOT += v2->size * v2->size;
    }
    delete_vector(v1);
    delete_vector(v2);

    cData.mean.cI /= nNodes;
    cData.mean.cO /= nNodes;
    cData.mean.cIO /= nNodes;
    cData.mean.cTOT /= nNodes;

    cData.variance.cI = cData.variance.cI / nNodes - cData.mean.cI * cData.mean.cI ;
    cData.variance.cO = cData.variance.cO / nNodes - cData.mean.cO * cData.mean.cO ;
    cData.variance.cIO = cData.variance.cIO / nNodes - cData.mean.cIO * cData.mean.cIO ;
    cData.variance.cTOT = cData.variance.cTOT / nNodes - cData.mean.cTOT * cData.mean.cTOT ;

    printf("\n\n----------------------\n");
    printf("IN  Connections : min : %d , max : %d , mean : %lf , variance : %lf\n" , cData.min.cI , cData.max.cI , cData.mean.cI , cData.variance.cI);
    printf("OUT Connections : min : %d , max : %d , mean : %lf , variance : %lf\n" , cData.min.cO , cData.max.cO , cData.mean.cO , cData.variance.cO);
    printf("IO  Connections : min : %d , max : %d , mean : %lf , variance : %lf\n" , cData.min.cIO , cData.max.cIO , cData.mean.cIO , cData.variance.cIO);
    printf("TOT Connections : min : %d , max : %d , mean : %lf , variance : %lf\n" , cData.min.cTOT , cData.max.cTOT , cData.mean.cTOT , cData.variance.cTOT);
    printf("----------------------");

    size_t nBins = 10;
    BinsByClass* bins = constructIstogram(nodelist , &cData , nBins);

    FILE* file = fopen("files/output.txt" , "w");
    if(file == NULL){
        perror("Unable to write in an output file");
        return -1;
    }
    //Header
    fprintf(file , "range , range , frequency\n");


    fprintf(file , "I\n");
    for(int i = 0; i < nBins; i++)
        fprintf(file , "%lf , %lf , %-9d\n" , bins->I[i].range[0] , bins->I[i].range[1] , bins->I[i].frequency);

    fprintf(file , "\nO\n");
    for(int i = 0; i < nBins; i++)
        fprintf(file , "%lf , %lf , %-9d\n" , bins->O[i].range[0] , bins->O[i].range[1] , bins->O[i].frequency);
    

    fprintf(file , "\nIO\n");
    for(int i = 0; i < nBins; i++)
        fprintf(file , "%lf , %lf , %-9d\n" , bins->IO[i].range[0] , bins->IO[i].range[1] , bins->IO[i].frequency);

    fprintf(file , "\nTOT\n");
    for(int i = 0; i < nBins; i++)
        fprintf(file , "%lf , %lf , %-9d\n" , bins->TOT[i].range[0] , bins->TOT[i].range[1] , bins->TOT[i].frequency);

    delete_binsByClass(bins);
    return 0;
}