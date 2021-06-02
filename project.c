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

int e;                          /* For implicit extensions*/ 
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
    int k;                          /*Number of strings*/
    int i = 0;
    int m = 0;
    int textSize = 0; 
    node suffix_tree_root;
    scanf("%d", &k);
    Ti = (char**)malloc(sizeof(char*) * (unsigned long)k);                    /*List of all strings.*/
    ni = (int*)malloc(sizeof(int) * (unsigned long)k);                      /*List of sizes of all strings*/

    while(i < k){
        scanf("%d", &m);            /*Size of string i*/
        ni[i] = m;
        Ti[i] = (char*)malloc(sizeof(char)* (unsigned long)(m+1));
        scanf("%s", Ti[i]);             /*Actual string i*/
        i++;
        textSize += (m+1);
    }

    allocateNodeMemory(textSize);
    numNodes = 0;
    suffix_tree_root = Ukkonnen(k);
    printLCSS(suffix_tree_root, k);

    cleanUp(k);
    return 0;
}

void allocateNodeMemory(int textSize ){
    int maxNumNodes = 2*textSize +1;
    nodes = (node*)malloc(sizeof(node) * (unsigned long)maxNumNodes);

}

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

node Ukkonnen(int k){
    node root; 
    node sentinel; 
    point p;

    int j = 0;
    int i = 0;

    /* Initializing sentinel*/
    sentinel = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    sentinel ->id = -1;
    sentinel ->slink = NULL;
    sentinel ->head = -1;
    sentinel ->sdep = -1;


    /* Initializing root*/
    root = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
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
    p->a = root;
    p->s = 0;
    p->b = NULL;

    while(i < k){
        Ti[i][ni[i]] = '&';      
        j = 0;
        e = 0;

        while(j <= ni[i]){
            lastNewNode = NULL;          
            while(!DescendQ(p, i, j)){
                AddLeaf(root, p, i, j);
                fflush(stdout);
                SuffixLink(p,i,j);
                fflush(stdout);
            }
            Descend(p, i, j);
            fflush(stdout);
            j++;
            e++;
        }

        Ti[i][ni[i]] = '$';
        i++;
    } 
    
    /*printTree(root);*/
    free(sentinel);
    free(p);
    return root;   
}

int DescendQ(point p, int i, int j){
    char newChar;
    char edgeChar;
    int edgePos;
    newChar = Ti[i][j];
    
    /* Case 1) We're at the sentinel node*/
    if(p->a->id == -1){
        return 1;
    }

    /* Case 2) We're in a edge*/
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
        - We need to try all possible edges, traversing p->a->child and so on*/
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

void Descend(point p, int i, int j){ /* TODO: Add case of the sentinel */
    char newChar;
    char edgeChar;
    int edgePos;
    newChar = Ti[i][j];

    /* Case 1) We're in the sentinel/root */
    if(p->a->id == -1){
        p->a = p->a->child; /* We want p to point to the root, and then go to Case 3)*/
        p->s = 0;
        p->b = NULL;
        return;
    }
    
    /* Case 2) We're in a edge*/
    if( (p->s > p->a->sdep) ){
        p->s ++;
        if(p->s == p->b->sdep){
            p->a = p->b;
            p->b = NULL;
        }
    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges, traversing p->a->child and so on*/
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

/*  
    Creates a new leaf and inserts it into the tree
    It might also be necessary to create a new internal node.
*/
void AddLeaf(node root, point p, int i, int j){
    /* Our new leaf node*/
    node leaf; 
    leaf = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    nodes[numNodes] = leaf;
    leaf->id = numNodes;
    numNodes ++;

    leaf->child = NULL;
    leaf->brother = NULL;
    leaf->slink = NULL;
    leaf->Ti = i; 


   /* Case 1) Need to add only a leaf */
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
    

   /* Case 2) Need to add internal node*/
    else{
        node internal; /* Our internal node*/
        internal = (node)malloc(sizeof(struct node)); /*TODO: check if malloc returns null*/
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

        internal->slink = root; /* By default, any new internal node has its slink to root. This might change in future iterations*/ 
        if(lastNewNode != NULL){
            lastNewNode->slink = internal;
        }
        if(internal->sdep != 1){
            lastNewNode = internal;
        }
        else{
            lastNewNode = NULL;
        }

    }
}

void SuffixLink(point p, int i, int j){
    int travelSize; /* Number of chars we need to pass when walking up the edge to p->above*/
    int edgeLength;
    int placedP;
    node next;

    edgeLength = 0;
    placedP = 0;


    if(p->s == 0){
        p->a = p->a->slink;
        p->b = NULL;
        return;
    }

    else{
        if (p->a->id != 0)
            p->a = p->a->slink; 
        
        p->s--;
        travelSize = p->s - p->a->sdep ;
        next = p->a->child;
        p->b = NULL; /* By default*/

        /*Going down the tree to place p in its right place*/
        while((placedP == 0) && (next != NULL)){        

            if((ni[next->Ti] >= next->head + p->a->sdep) && (Ti[next->Ti][next->head + p->a->sdep] == Ti[i][j - travelSize])){ /* If chars match*/

                if(next->sdep == -1){
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

void printTree(node root){
    /*---- Initializing the visited list for DFS --------*/
    int* visited = (int*)malloc(sizeof(int) * (unsigned long)numNodes);
    int i;
    FILE* f;

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

void printLCSS(node root, int k){
    int* visited = (int*)malloc(sizeof(int) * (unsigned long)numNodes);
    int i;
    int d;
    int *LCSS = (int*)malloc(sizeof(int)* (unsigned long)k);
    node n;
    
    for(d=0; d<k; d++){
        LCSS[d] = 0;
    }
    
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