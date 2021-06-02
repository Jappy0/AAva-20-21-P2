/* ------------ Advanced Algorithms 2020/2021 ---------------*/
/* ------------ Project 2 - Suffix Trees --------------------*/

/* Andreia Pereira  | n. 89414 */
/* Pedro Nunes      | n. 89525 */

/* --------------------------------------------------------- */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
/* --------------------------------------------------------- */

#define TSIZE 64
#define END -1

/* -----------------------NODE STRUCT----------------------- */

typedef struct node* node;
struct node{
    int id;                       /* Node's id*/
    int Ti;                       /* The value of i in Ti */
    int head;                     /* The path-label startat &(Ti[head]) */
    int sdep;                     /* String-Depth */
    node child;                   /* Child */
    node brother;                 /* brother */
    node slink;                   /* Suffix link */
    node* hook;                   /* What keeps this linked? */
    int* childrenSuffixes;
    int numSuffixes;
};

/* -----------------------POINT STRUCT---------------------- */
typedef struct point* point;
struct point{
    node a;                       /**< node above */
    node b;                       /**< node bellow */
    int s;                        /**< String-Depth */
};

/* ----------------------Global Variables--------------------- */

char** Ti;                       /* List of all strings.*/
int* ni;                         /* List of sizes of all strings*/
node* nodes;                     /* List of all our nodes*/
int numNodes;                    /* Number of nodes*/
node lastNewNode;                /* Last internal node. This will be used to set suffix links*/

/* --------------------------------------------------------- */
int DescendQ(point p, int i, int j);
void Descend(point p, int i, int j);
void AddLeaf(node root, point p, int i, int j);
void SuffixLink(point p, int i, int j);
node Ukkonnen(int k);
void printingDFS(FILE *f, int* visited, node root);
void DFS(int* visited, node root, int k);
void printLCSS(node root, int k);
void printTree(node root);
void allocateNodeMemory(int textSize );
void cleanUp(int k);

/* --------------------------------------------------------- */

int main(){
    
    /*Reading the input*/
    int k;                                                                  /* Number of strings*/
    int i = 0;
    int m = 0;                                                              /* Text size*/
    int textSize = 0;                                                       /* Sum of all text sizes m_i*/
    node suffix_tree_root;                                                  /* The root of the suffix tree*/
    scanf("%d", &k);
    Ti = (char**)malloc(sizeof(char*) * (unsigned long)k);                  /* List of all strings.*/
    if(Ti == NULL) exit(-1);
    
    ni = (int*)malloc(sizeof(int) * (unsigned long)k);                      /* List of sizes of all strings*/
    if(ni == NULL) exit(-1);

    while(i < k){
        scanf("%d", &m);                                                    /*Size of string i*/
        ni[i] = m;
        Ti[i] = (char*)malloc(sizeof(char)* (unsigned long)(m+1));
        if(Ti[i] == NULL) exit(-1);
        scanf("%s", Ti[i]);                                                 /*Actual string i*/
        i++;
        textSize += (m+1);
    }

    allocateNodeMemory(textSize);
    numNodes = 0;                                                            /* Number of nodes*/
    suffix_tree_root = Ukkonnen(k);
    printLCSS(suffix_tree_root, k);

    cleanUp(k);
    return 0;
}
/**
 * allocateNodeMemory
 * 
 * Allocates the needed memory for the possible 2*textSize + 1 nodes of the tree.
 * 
*/
void allocateNodeMemory(int textSize ){
    int maxNumNodes = 2*textSize +1;
    nodes = (node*)malloc(sizeof(node) * (unsigned long)maxNumNodes);
    if(nodes == NULL) exit(-1);
}

/**
 * cleanUp
 * 
 * Frees all used memory:
 *  - All strings in Ti
 *  - String size array ni
 *  - All nodes
*/
void cleanUp(int numStrings){
    /* Free memory occupied by strings*/
    int i;

    for(i=0; i < numStrings; i++){
        free(Ti[i]);
    }
    free(Ti);
    free(ni);

    /* Free memory occupied by nodes*/
    for(i = 0; i<numNodes; i++){
        if(nodes[i] != NULL){
            free(nodes[i]);
        }
    }
    free(nodes);
}

/**
 * Ukkonnen
 * 
 * Executes the Ukkonnen algorithm
 * 
 * Receives the number of strings, k
 * 
 * Returns the suffix tree root
*/
node Ukkonnen(int k){
    node root; 
    node sentinel; 
    point p;

    int j = 0;
    int i = 0;

    /* Initializing sentinel node*/
    sentinel = (node)malloc(sizeof(struct node));  
    if(sentinel == NULL) exit(-1);
    sentinel ->id = -1;
    sentinel ->slink = NULL;
    sentinel ->head = -1;
    sentinel ->sdep = -1;


    /* Initializing root node*/
    root = (node)malloc(sizeof(struct node));  
    if(root == NULL) exit(-1);
    nodes[numNodes] = root;
    root ->id = 0;
    root ->slink = sentinel;
    root ->sdep = 0;
    root ->head = 0;
    root ->Ti = 0;
    root-> child = NULL;
    root->brother = NULL;
    numNodes ++;

    sentinel->child = root;
    
    /* Initializing point*/
    p = (point)malloc(sizeof(struct point));
    if(p == NULL) exit(-1);
    p->a = root;
    p->s = 0;
    p->b = NULL;

    while(i < k){                           /* Iterates through all k strings*/
        Ti[i][ni[i]] = '&';      
        j = 0;

        while(j <= ni[i]){                  /* Iterates through all j Phases*/
            lastNewNode = NULL;             /* At each phase we clean the lastNewNode*/
            while(!DescendQ(p, i, j)){
                AddLeaf(root, p, i, j);
                SuffixLink(p,i,j);
            }
            Descend(p, i, j);
            j++;
        }

        Ti[i][ni[i]] = '$';
        i++;
    } 
    
    /*printTree(root); < This was used for debugging*/
    free(sentinel);
    free(p);
    return root;   
}

/**
 * DescendQ
 * 
 * Receives a point p and a char c position (i, j)
 * 
 * Returns: 
 *  - 1 (true) if we can descend from point p with char c
 *  - 0 (false), otherwise
*/
int DescendQ(point p, int i, int j){
    char newChar;
    char edgeChar;
    int edgePos;
    newChar = Ti[i][j];
    
    /* Case 1) We're at the sentinel node
        - We can always descend from the sentinel node
    */
    if(p->a->id == -1){
        return 1;
    }

    /* Case 2) We're in a edge 
        - If p->b is null, we cannot descend
        - If not, we check if the char pointed from p is the same as T[i][j]
    */
    if( p->s > p->a->sdep){
        if(p->b == NULL){
            return 0;
        }
        else{
            edgePos = p->b->head + p->s;

            if(edgePos < ni[p->b->Ti]){
                edgeChar = Ti[p->b->Ti][edgePos];

                if(newChar == edgeChar){
                    return 1;
                } 
            }
        }
    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges, and see if we can descend from any of them
    */
    else{
        node next;
        next = p->a->child;

        while(next != NULL){
            edgePos = next->head + p->s;

            if(edgePos < ni[next->Ti]){
                edgeChar = Ti[next->Ti][edgePos];
                if(newChar == edgeChar){
                    return 1; 
                } 
            }
            next = next->brother;
        }
    }
    return 0;
}

/**
 * DescendQ
 * 
 * Receives a point p and a char c position (i, j)
 * 
 * Descends the tree, placing point p in correct position
*/
void Descend(point p, int i, int j){ 
    char newChar;
    char edgeChar;
    int edgePos;
    newChar = Ti[i][j];

    /* Case 1) We're in the sentinel/root 
        - We want p->a to be the root
    */
    if(p->a->id == -1){
        p->a = p->a->child;  /* The root*/
        p->s = 0;
        p->b = NULL;
        return;
    }
    
    /* Case 2) We're in a edge
        - We increase p->s
        - If this puts us at the end of the edge, we descend to the node bellow
    */
    if( (p->s > p->a->sdep) ){
        p->s ++;
        if(p->s == p->b->sdep){
            p->a = p->b;
            p->b = NULL;
        }
    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges to figure out through which edge we should descend
    */
    else{
        node next;
        next = p->a->child;

        while(next != NULL){
            edgePos = next->head + p->s;

            if(edgePos < ni[next->Ti]){
                edgeChar = Ti[next->Ti][edgePos];
                if(newChar == edgeChar){
                    p->b = next;
                    p->s ++;
                    if(p->s == p->b->sdep){
                        p->a = p->b;
                        p->b = NULL;
                    }
                    break;
                }
            }
            next = next->brother;
        }
    }    
}

/**
 * AddLeaf
 * 
 * Creates a new leaf node and inserts it into the tree.
 * It might also be necessary to create a new internal node.
*/
void AddLeaf(node root, point p, int i, int j){
    /* Our new leaf node*/
    node leaf; 
    leaf = (node)malloc(sizeof(struct node));  
    if(leaf == NULL) exit(-1);
    nodes[numNodes] = leaf;
    leaf->id = numNodes;
    numNodes ++;

    leaf->child = NULL;
    leaf->brother = NULL;
    leaf->slink = NULL;
    leaf->Ti = i; 


   /* Case 1) Need to add only a leaf node*/
   if( (p->s == p->a->sdep) || (p->b == NULL)){
       
        node next;
        next = p->a->child;

        if (next == NULL){
            p->a->child = leaf;
            leaf->hook = &(p->a->child);

        }
        else{
            int foundLastChild;
            foundLastChild = 0;
            while( foundLastChild == 0){
                if(next->brother == NULL){
                    next->brother = leaf;
                    leaf->hook = &(next->brother);
                    foundLastChild = 1;
                }
                next = next->brother;
            }   
        }
        leaf->head = j - p->s;
        leaf->sdep = END;   
   }
    

    /* Case 2) We also need to add a new internal node*/
    else{
        node internal; /* Our internal node*/
        internal = (node)malloc(sizeof(struct node)); 
        if(internal == NULL) exit(-1);
        nodes[numNodes] = internal;
        internal->id = numNodes;
        internal->Ti = p->b->Ti;
        numNodes++;

        *(p->b->hook) = internal; 
        internal-> hook = p->b->hook;

        internal->brother = p->b->brother;
        if(p->b->brother != NULL){
            p->b->brother->hook = &(internal->brother);
        }
    
        internal->child = p->b;
        p->b->hook = &(internal->child);

        p->b->brother = leaf;
        leaf->hook = &(p->b->brother);

        internal->head = p->b->head; 
        leaf->head = j - p->s;

        internal->sdep= p->s ;
        leaf->sdep = END;

        /* By default, a new internal node has its slink to root. This might change in future iterations*/ 
        internal->slink = root; 
        if(lastNewNode != NULL){
            lastNewNode->slink = internal;
        }

        /* A new internal node with string depth of 1 will forever have its suffix link as the root.*/
        if(internal->sdep != 1){
            lastNewNode = internal;
        }
        else{
            lastNewNode = NULL;
        }

    }
}

/**
 * SuffixLink
 * 
 * Places point p in the right position after AddLeaf
*/
void SuffixLink(point p, int i, int j){
    int travelSize; /* Number of chars we need to pass when walking up the edge to p->above*/
    int edgeLength;
    int placedP;
    node next;

    edgeLength = 0;
    placedP = 0;

    /* Case 1) p->s is 0 - there are no remaining chars to consider
        - We simply travel to p->a's suffix link and set p->b to NULL
    */
    if(p->s == 0){
        p->a = p->a->slink;
        p->b = NULL;
        return;
    }

    /* Case 2) There are remaining chars to consider*/
    else{

        /*  If we're at the root we want to start looking from there. 
            Otherwise, we travel to p->a's suffix link
        */
        if (p->a->id != 0)
            p->a = p->a->slink; 
        
        p->s--;
        travelSize = p->s - p->a->sdep ;
        next = p->a->child;
        p->b = NULL; /* By default*/

        /*Going down the tree to place p in its right place*/
        while((placedP == 0) && (next != NULL)){        
            
            /* Checking to see if chars match*/
            if((ni[next->Ti] >= next->head + p->a->sdep) && (Ti[next->Ti][next->head + p->a->sdep] == Ti[i][j - travelSize])){ 

                if(next->sdep == -1){ /* When we reach a leaf, we should stop*/
                    p->b = next;
                    break;
                }
                else{
                    edgeLength = next->sdep - p->a->sdep;
                }
                
                if(edgeLength <= travelSize){
                    travelSize = travelSize -edgeLength;
                    p->a = next;
                    p->b = NULL;
                    next = next->child;
                    if(travelSize == 0){
                        placedP =1;
                    }
                }
                else{
                    placedP =1;
                    p->b = next;
                }
            }
            else{
                next = next->brother; 
            }     
        }
    }

}

/**
 * printTree
 * 
 * A debug function that prints the generated suffix tree to a 'tree.gv' file in .dot format
*/
void printTree(node root){
    /*---- Initializing the visited list for DFS --------*/
    int i;
    FILE* f;

    int* visited = (int*)malloc(sizeof(int) * (unsigned long)numNodes);
    if(visited == NULL) exit(-1);

    for(i=0; i<numNodes; i++){
        visited[i] = 0;
    }
    f = fopen("tree.gv", "w");
    fprintf(f, "digraph g {\n");
    printingDFS(f, visited, root);
    fprintf(f, "}\n");
    fclose(f);

    free(visited);
}

/**
 * printingDFS
 * 
 * An auxiliary function to printTree
 * Does a DFS search and prints the nodes in .dot format
*/
void printingDFS(FILE *f, int* visited, node root){

    node node_visited = root;
    node next;
    int i;
    visited[root->id] = 1;
    next = node_visited->child;


    if(node_visited->slink != NULL){
        fprintf(f, "\t%d -> %d [style=dotted]\n", node_visited->id, node_visited->slink->id);
        fflush(f);
    }
    
    while(next!= NULL){

        if(next->sdep == END){ 
            next->sdep = ni[next->Ti] +1 - next->head;
        }   

        fprintf(f, "\t%d -> %d [label=\"", node_visited->id, next->id);
        fflush(f);
        /*printf("\tNode %d: head = %d , sdep = %d\n", node_visited->id, node_visited->head, node_visited->sdep);*/
        
        for(i= next->head + node_visited->sdep; i< next->head + next->sdep; i++){ 
            printf("\t next->id: %d, next->Ti: %d, i: %d \n", next->id, next->Ti, i);
            fflush(stdout);
            fprintf(f, "%c", Ti[next->Ti][i]); 
            fflush(f);
        }
        
        fprintf(f, "\"]\n");
        fflush(f);
        
        printingDFS(f, visited, next);
        next = next->brother;
    }
}

/**
 * printLCSS
 * 
 * Finds the Longest Common Substring between d strings, for 1 < d <= k
*/
void printLCSS(node root, int k){
    int i;
    int d;
    node n;

    int* visited = (int*)malloc(sizeof(int) * (unsigned long)numNodes);
    int *LCSS = (int*)malloc(sizeof(int)* (unsigned long)k);
    if((visited == NULL) || (LCSS == NULL)) exit(-1);
    
    /* LCSS array initialization*/
    for(d=0; d<k; d++){
        LCSS[d] = 0;
    }
    
    /* Visisted list initialization (for DFS)*/
    for(i=0; i<numNodes; i++){
        visited[i] = 0;
    }

    DFS(visited, root, k);
    free(root->childrenSuffixes);

    /* Finding LCCS between d nodes*/
    d = 1;
    for(i= 0; i<numNodes; i++){
        n = nodes[i];
        if( LCSS[n->numSuffixes - 1] < n->sdep){
            LCSS[n->numSuffixes - 1] = n->sdep;
        }    
    }
    /* A backward traversal to adjust the previous calculated values*/
    for(i=k-1; i>0; i--){
        if(LCSS[i] > LCSS[i-1]) LCSS[i-1] = LCSS[i];
    }

    for(i = 1; i<k; i++){
        printf("%d ", LCSS[i]);
    }
    printf("\n");

    free(visited);
    free(LCSS);
}

/**
 * DFS (Depth First Search)
 * 
 * Recursive DFS function.
 * Aditionally, this function saves the node's number of suffixes by mergind its children's suffixes.
*/
void DFS(int* visited, node root, int k){
    node node_visited = root;
    node next;
    int i;
    
    next = node_visited->child;
    visited[root->id] = 1;
    
    
    /* Search Phase*/
    while(next!= NULL){ 
        if(visited[next->id] == 0){
            DFS(visited, next, k);
        } 
        next = next->brother;
    }

    /*Children Suffix Lists Merge*/
    if(node_visited->sdep !=END){ /* Leafs don't need to allocate a list - they'll only belong to one string*/
        node_visited->childrenSuffixes = (int*)malloc(sizeof(int)* (unsigned long)k);
        if( node_visited->childrenSuffixes == NULL) exit(-1);
        
        node_visited->numSuffixes = 0;
        for(i=0; i<k;i++){
            node_visited->childrenSuffixes[i] = 0;
        }
        next = node_visited->child;

        while(next!=NULL){
            if (next->sdep == END){
                node_visited->childrenSuffixes[next->Ti] = 1;
            }
            else{
                for(i=0; i<k;i++){
                    if(next->childrenSuffixes[i] == 1){
                        node_visited->childrenSuffixes[i] = 1;
                    } 
                }
                free(next->childrenSuffixes);
            }
            next = next->brother;
        }
        for(i=0; i<k;i++){
            if( node_visited->childrenSuffixes[i] == 1){
                node_visited->numSuffixes++;
            }
        }
    }
    else node_visited->numSuffixes = 1;  /* A leaf will always only belong to one suffix.*/
}
