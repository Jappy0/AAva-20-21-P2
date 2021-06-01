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
void DFS(int* visited, node root);
void printTree(node root);
void allocateNodeMemory(int textSize );
void cleanUp(int k);

/* --------------------------------------------------------- */

int main(int argc, char *argv[]){
    
    /*Reading the input*/
    int k;                          /*Number of strings*/
    scanf("%d", &k);
    Ti = (char**)malloc(sizeof(char*)*k);                    /*List of all strings.*/
    ni = (int*)malloc(sizeof(int)*k);                      /*List of sizes of all strings*/

    int i = 0;
    int m = 0;
    int textSize = 0; 
    while(i < k){
        scanf("%d", &m);            /*Size of string i*/
        ni[i] = m;
        Ti[i] = (char*)malloc(sizeof(char)* (m+1));
        scanf("%s", Ti[i]);             /*Actual string i*/
        i++;
        textSize += (m+1);
    }

    allocateNodeMemory(textSize);
    numNodes = 0;
    node suffix_tree_root = Ukkonnen(k);
    printTree(suffix_tree_root);

    cleanUp(k);
    return 0;
}

void allocateNodeMemory(int textSize ){
    int maxNumNodes = 2*textSize +1;
    int i;
    nodes = (node*)malloc(sizeof(node)*maxNumNodes);

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
    printf("Ukkonen T[0][0] = %c\n", Ti[0][0]);
    /* Initializing sentinel*/
    node sentinel; 
    sentinel = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    sentinel ->id = -1;
    sentinel ->slink = NULL;
    sentinel ->head = -1;
    sentinel ->sdep = -1;


    /* Initializing root*/
    node root; 
    root = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    nodes[numNodes] = root;
    root ->id = 0;
    root ->slink = sentinel;
    root ->sdep = 0;
    root ->head = 0;
    root-> child = NULL;
    root->brother = NULL;
    numNodes ++;

    sentinel->child = root;
    
    /* Initializing point*/
    point p;
    p = (point)malloc(sizeof(struct point));
    p->a = root;
    p->s = 0;
    p->b = NULL;
    int j = 0;
    int l;
    int i = 0;

    while(i < k){
        Ti[i][ni[i]] = '\1';      
        j = 0;
        e = 0;

        while(j <= ni[i]){
            lastNewNode = NULL;
            printf("Phase %d: T[i][j]= %c\n", j, Ti[i][j]);            
            while(!DescendQ(p, i, j)){
                AddLeaf(root, p, i, j);
                SuffixLink(p,i,j);
            }
            Descend(p, i, j);
            j++;
            e++;
        }

        Ti[i][ni[i]] = '\0';
        i++;
    } 

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
        printf("\tDescendQ- Case 1\n");
        return 1;
    }

    /* Case 2) We're in a edge*/
    if( p->s > p->a->sdep ){
        printf("\tDescendQ- Case 2: p->s= %d and p->a->sdep = %d\n", p->s, p->a->sdep);
        edgePos = p->b->head + p->s;

        if(edgePos < ni[i]){
            edgeChar = Ti[i][edgePos];

            if(newChar == edgeChar) return 1;
        }
    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges, traversing p->a->child and so on*/
    else{
        printf("\tDescendQ- Case 3\n");
        node next;
        next = p->a->child;

        while(next != NULL){
            edgePos = next->head + p->s;

            if(edgePos < ni[i]){
                edgeChar = Ti[i][edgePos];
                if(newChar == edgeChar) return 1;
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
        printf("\tDescend- Case 1\n");
        p->a = p->a->child; /* We want p to point to the root, and then go to Case 3)*/
        p->s = 0;
        p->b = NULL;
    }
    
    /* Case 2) We're in a edge*/
    
    else if( p->s > p->a->sdep ){
        printf("\tDescend- Case 2\n");
        p->s ++;
        if(p->s == p->b->sdep){
            printf("\t\tUpdated p->a\n");
            p->a = p->b;
            p->b = NULL;
        }
        printf("\t\tp->a->id = %d\n", p->a->id);
        printf("\t\tp->b->id = %d\n", p->b->id);
        printf("\t\tp->s = %d\n", p->s);

    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges, traversing p->a->child and so on*/
    else{
        printf("\tDescend- Case 3: \n");
        node next;
        next = p->a->child;

        while(next != NULL){
            edgePos = next->head + p->s;
            if(edgePos < ni[i]){
                edgeChar = Ti[i][edgePos];
                if(newChar == edgeChar){
                    p->b = next;
                    p->s ++;
                    if(p->s == p->b->sdep){
                        printf("\t\tUpdated p->a\n");
                        p->a = p->b;
                        p->b = NULL;
                    }
                    break;
                }
            }
            next = next->brother;
        }
        printf("\t\tp->a->id = %d\n", p->a->id);
        /*printf("\t\tp->b->id = %d\n", p->b->id);*/
        printf("\t\tp->s = %d\n", p->s);
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
   if( p->s == p->a->sdep){
       printf("\tAddLeaf - Case 1\n");
       
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
        leaf->head = j - p->a->sdep;
        leaf->sdep = END;    /*e - leaf-> head;*/
   }
    

   /* Case 2) Need to add internal node*/
    else{
        printf("\tAddLeaf - Case 2\n");
        node internal; /* Our internal node*/
        internal = (node)malloc(sizeof(struct node)); /*TODO: check if malloc returns null*/
        nodes[numNodes] = internal;
        internal->id = numNodes;
        internal->Ti = i;
        numNodes++;

        printf("\t\tp->a->id = %d\n", p->a->id);
        printf("\t\tp->b->id = %d\n", p->b->id);
        
        
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

        internal->sdep= p->a->sdep + p->s;
        leaf->sdep = END;

        internal->slink = root; /* By default, any new internal node has its slink to root. This might change in future iterations*/ 
        if(lastNewNode != NULL){
            lastNewNode->slink = internal;
        }
        lastNewNode = internal;
    }
}

void SuffixLink(point p, int i, int j){
    
    int travelSize; /* Number of chars we need to pass when walking up the edge to p->above*/
    char c;
    int edgeLength;
    edgeLength = 0;
    int placedP;
    placedP = 0;

    node next;

    /* p->a pode nao ser a root
        - passas para p->a->slink    
    */
    /* p->a for a root:
        - p->s == 0 -> passas para p->a->slink
        - p->s != 0 -> nao passas para p->a->slink
    */

    if(p->s == 0){
        p->a = p->a->slink;
        p->b = NULL;
        return;
    }

    else{
        if (p->a->id != 0 ){
            p->a = p->a->slink; 
        }
        p->s--;
        travelSize = p->s - p->a->sdep ;
        next = p->a->child;
        p->b = NULL; /* By default*/

        /*Going down the tree to place p in its right place*/
        while((placedP == 0) && (next != NULL)){
            printf("next->head=%d, p->a->sdep=%d\n",next==NULL? -1 : next->head , p->a==NULL? -1 : p->a->sdep );
            printf("\t\tEdge char = %c.", Ti[i][next->head + p->a->sdep]);
            printf(" Text char = %c\n", Ti[i][j - travelSize]);
            if(Ti[i][next->head + p->a->sdep] == Ti[i][j - travelSize]){ /* If chars match*/

                if(next->sdep == -1){
                    edgeLength = (e - next->head) - p->a->sdep;
                }
                else{
                    edgeLength = next->sdep - p->a->sdep;
                }
                
                if(edgeLength <= travelSize){
                    travelSize = travelSize -edgeLength;
                    p->a = next;
                    next = next->child;
                    if(travelSize == 0){
                        placedP = 1;
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
        printf("\t\tp->a->id = %d\n", p->a->id);
        printf("\t\tp->b->id = %d\n", p->b== NULL? -1: p->b->id);
        printf("\t\tp->s = %d\n", p->s);
    }

}


void printTree(node root){
    /*---- Initializing the visited list for DFS --------*/
    int visited[numNodes];
    int i;
    for(i=0; i<numNodes; i++){
        visited[i] = 0;
    }

    printf("digraph g {\n");
    DFS(visited, root);
    printf("}\n");
}

void DFS(int* visited, node root){

    visited[root->id] = 1;
    node node_visited = root;
    node next;
    int i;
    next = node_visited->child;

    if(node_visited->brother !=NULL){
        printf("\t%d -> %d [style=dotted]\n", node_visited->id, node_visited->brother->id);
    }
    
    while(next!= NULL){

        if(next->sdep == END){ 
            next->sdep = ni[next->Ti] +1 - next->head;
        }   

        printf("\t%d -> %d [label=\"", node_visited->id, next->id);
        
        for(i= next->head + node_visited->sdep; i< next->head + next->sdep; i++){
            printf("%c", Ti[next->Ti][i]); 
        }
        /*if (Ti[next->Ti][i] == '\0') printf("$");*/
        printf("\"]\n");
        
        DFS(visited, next);
        next = next->brother;
    }
}