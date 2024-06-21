
#include "../Dependencies/Vector.h"
#include "../Dependencies/SetOp.h"

#define OPINION_NO 0
#define OPINION_YES 1
#define MAX_GROUP_SIZE 10
#define MIN_GROUP_SIZE 2

//Index is not really used, anyway
typedef struct{
    int opinion;
    int index;
} Agent;




typedef struct{
    Vector* agents;
} Population;


//Just a constructor for the Population struct
//instanciate the agents list as a vector of Pointers to Agent instances
//(could as well been instanciated directly in the vector, if Agent struct is
//pretty big it avoids copying the whole data of the agent struct into the vector buffer
//(only drawback is memory fragmentation and the indirection of derefercing the pointer
//to access the agent Data))
Population* make_population(size_t size , size_t numberNO ){
    assert(numberNO < size);

    Population* p = malloc(sizeof(Population));
    p->agents = make_vector_with_capacity(sizeof(Agent) , size);

    for(int i = 0; i < size; i++){  
        int opinion =  i < numberNO ? OPINION_NO : OPINION_YES;
        Agent agent = { .opinion = opinion , .index = p->agents->size};

        vector_push_back(p->agents , &agent);
    }

    return p;
}

//Extract a group of randomly chosen agents from the Population array, of size given 
//by sizeGroup, stores pointers to those agents in the dest vector.
//The extraction is done by randomly permuting the population array
//And extracting the first sizeGroup agents from the list
//
//WARNING, dest vector is not cleared, it's up to the user,
//agents are just pushed into the vector
void extract_group(Vector* dest , Population* popultation , size_t sizeGroup){
    
    Permute(popultation->agents);
    for(int i = 0; i < sizeGroup; i++){
        Agent* agent = (Agent*) vector_get(popultation->agents , i);
        vector_push_back(dest , &agent);
    }

}

void print_vector_opinion(Vector* v , char charOP[2]){

    printf("( ");
    int size = (int)v->size;
    for(int i = 0; i < size - 1; i++){
        Agent* agent = *(Agent**) vector_get(v , i);
        int opinion = agent->opinion;

        printf("%c , " , charOP[opinion]);
    }

    if(v->size > 0){
        Agent* agent = *(Agent**) vector_back(v);
        int opinion = agent->opinion;
        printf("%c"   , charOP[opinion]);
    }
    printf(" )");
}



int main(){

    size_t sizePop = 100;
    size_t numberNO = 40;
    const char* stringOpinion[2] = { "OP NO" , "OP YES"};
    char charOP[2] = { 'N' , 'Y'};
    int totOpinions[2] = { numberNO , sizePop - numberNO };


    Population* population = make_population(sizePop , numberNO);

    //Print the agents list
    if(sizePop < 200){
        for(int i = 0; i <sizePop; i++){
            Agent* agent = (Agent*) vector_get(population->agents , i ); 
            printf( "agent number %d , opinion : %c\n" , i , charOP[agent->opinion] );
        }
    }




    Vector* groupVec = make_vector(sizeof(Agent*));
    while(totOpinions[OPINION_NO] < sizePop && totOpinions[OPINION_YES] < sizePop){

        size_t groupSize = rand() % ( MAX_GROUP_SIZE - MIN_GROUP_SIZE) + MIN_GROUP_SIZE;
        int op[2] = {0 , 0};


        vector_clear(groupVec);
        extract_group(groupVec , population , groupSize);


        printf("\n-----------------------------------------\n");
        print_vector_opinion(groupVec , charOP);
        printf("\n");

        //Counting opinions in the group
        for(int i = 0; i < groupVec->size; i++)
            op[ (*(Agent**) vector_get(groupVec , i))->opinion] += 1; 
        
        
        int maxOP = op[OPINION_NO] > op[OPINION_YES] ? OPINION_NO : OPINION_YES;
        //In case of tie
        if(op[0] == op[1]){
            maxOP = rand() % 2;
        }

        //Update the total counts of opinions in the population
        int otherOP = !maxOP;
        totOpinions[maxOP] += op[otherOP]; 
        totOpinions[otherOP] -= op[otherOP];    


        //set the Major opinion across all the agents in the group
        for(int i = 0; i < groupVec->size; i++){
            ( *(Agent**) vector_get(groupVec , i))->opinion = maxOP;
        }


        printf("Group size is : %d , the majority is : %s , with counts ( NO : %d , YES : %d ) , total counts : ( N : %d , Y : %d ) \n" ,
         groupSize , stringOpinion[maxOP] , op[OPINION_NO] , op[OPINION_YES] , totOpinions[OPINION_NO] , totOpinions[OPINION_YES]  );

    }




}