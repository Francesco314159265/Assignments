


#ifndef NODE_LIST_H
#define NODE_LIST_H

#include <malloc.h>
#include "../../Dependencies/Vector.h"
#include "../../Dependencies/SetOp.h"



#define IN 0
#define OUT 1
#define MAX_LINE_LENGTH 500








// The Node struct stores pointers to vectors of integers, 
// which are, respectively, the indices of the nodes
// connecting IN(OUT) to the Node
typedef struct {
    Vector* list[2];
    size_t index;
} Node;


Node* make_node(){
    Node* node = malloc(sizeof(Node));
    node->list[0] = make_vector(sizeof(int));
    node->list[1] = make_vector(sizeof(int));

    return node;
}

// The NodeList stores pointers to Node instances
typedef struct{
    Vector* nodes;
} NodeList;
 

// Whenever we add a node, we construct it approprietly, and then return the pointer to the Node instance
// to the OP
Node* Nodelist_add_node(NodeList* nodeList){
    Node* node = make_node();
    node->index = nodeList->nodes->size;
    vector_push_back(nodeList->nodes , &node);
    return node;
}

// Nodes are intimately connected to their index, as it's effectively what let you find them in the NodeList.
// The reason why the list connection IN and list connection OUT are not list of pointers to node, 
// is that the pointers will be invalidated when the vector NodeList is reallocated (which could be frequently if OP 
// does not know the number of nodes ahead of time, so that the vector must reallocate itself when it's capacity is filled).
// if the OS reallocates in a new chunck of memory, all pointers to previous memory will be invalidated, and we should remap
// all of them, this is the reason why we use indices in the vector, because they continue to map indirectly to the same instances.
// This in reality is not a good solution, and the reason is that we hsould not have used a vector in the first place.
// A major drawback of using a vector for storing the nodes is that if a node must be removed from the list, all the nodes which comes 
// after him will be moved to the left by one index, so that all the indices will be changed.
// nodeList should have been something like an unordered map, which requires considerably more work
// to build , so for now the option of deleting the node is in stand_by 

// Node* Nodelist_delete_node(NodeList* nodeList){
// }

Node* Nodelist_get_node(NodeList* nodeList , size_t index){
    return *(Node**) vector_get(nodeList->nodes , index);
}

size_t NodeList_get_number_nodes(NodeList* nodeList){
    return nodeList->nodes->size;
}

NodeList* make_NodeList(){
    NodeList* newList = malloc(sizeof(NodeList));
    newList->nodes = make_vector(sizeof(Node*));

    return newList;
}

void delete_NodeList(NodeList* nodeList){
    for(int i = 0; i < nodeList->nodes->size; i++){
        Node* node = (Node*) vector_get(nodeList->nodes , i);
        delete_vector(node->list[IN]);
        delete_vector(node->list[OUT]);
        free(node);
    }

    free(nodeList->nodes);
}


void finalize_nodelist(NodeList* nodelist){
    size_t nNodes = NodeList_get_number_nodes(nodelist);

    for(int i = 0; i < nNodes; i++  ){
        Node* node = Nodelist_get_node(nodelist , i);

        sort( node->list[IN]->data , node->list[IN]->size , node->list[IN]->stride , LessThan_int );
        sort( node->list[OUT]->data , node->list[OUT]->size , node->list[OUT]->stride , LessThan_int );
    }    
}


void print_nodeList(NodeList* nodelist){
    size_t nNodes = NodeList_get_number_nodes(nodelist);

    Vector* v1 = make_vector(sizeof(int));
    Vector* v2 = make_vector(sizeof(int));
    for(int i = 0; i < nNodes; i++  ){
        Node* node = Nodelist_get_node(nodelist , i);

        vector_clear(v1);
        vector_clear(v2);
        printf("Node %d , IN : " , node->index);
        print_vector_int(node->list[IN]);
        printf(" , OUT : ");
        print_vector_int(node->list[OUT]);

        sort( node->list[IN]->data , node->list[IN]->size , node->list[IN]->stride , LessThan_int );
        sort( node->list[OUT]->data , node->list[OUT]->size , node->list[OUT]->stride , LessThan_int );
        set_intersection(v1 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);
        set_union(v2 , node->list[IN] , node->list[OUT] , LessThan_int , Equal_int);

        printf(" , IO : ");
        print_vector_int(v1);
        printf(" , TOT : ");
        print_vector_int(v2);
        printf("\n");

    }    
    delete_vector(v1);
    delete_vector(v2);
}





NodeList* parseFile(const char* filename ) {
    FILE* file = fopen(filename, "r");

    if (!file) {
        perror("Can't open file");
        return NULL;
    }

    NodeList* nodelist = make_NodeList();

    char line[MAX_LINE_LENGTH];
    //First line is the header line, can be used as comments
    // fgets(line, sizeof(line), file);
    while (fgets(line, sizeof(line), file)) {

        //We leave lines starting with // as comments
        if (strstr(line, "//") != NULL || strcmp(line ,"\n") == 0) 
            continue;


        
        int nodeIndex;
        char* str = strtok(line, ", ");
        if (str) {
            nodeIndex = atoi(str);
            str = strtok(NULL, ", ");
            Node* node = Nodelist_add_node(nodelist);
            assert(node->index == nodeIndex);

            while (str && !( strcmp(str, "\n") == 0 ) ) {
                int connectedNode = atoi(str);
                vector_push_back(node->list[IN] , &connectedNode); 
                str = strtok(NULL, ", ");
            }
        }
    }

    fclose(file);


    for(int i = 0; i < nodelist->nodes->size; i++){
        Node* node = (Node*) Nodelist_get_node(nodelist , i);
        for(int j = 0; j < node->list[IN]->size; j++){
            int jInd = *(int*) vector_get(node->list[IN] , j);
            Node* nodeJ = Nodelist_get_node(nodelist , jInd);
            vector_push_back(nodeJ->list[OUT] , &i);
        }
    }

    return nodelist;

}



#endif