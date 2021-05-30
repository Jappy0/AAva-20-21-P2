/* ------------ Advanced Algorithms 2020/2021 ------------*/
/* ------------ Project 2 - Suffix Trees --------------*/

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
#define true 1
#define false 0


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

int* e;                          /* For implicit extensions*/ 
char** Ti;                       /* List of all strings.*/
int* ni;                         /* List of sizes of all strings*/
node* nodes;                     /* List of all our nodes*/
int numNodes;                    /* Number of nodes*/
node lastNewNode;                /* Last internal node. This will be used to set suffix links*/

/* --------------------------------------------------------- */
char* saveNewString(char *S, int *S_cap, int *S_size);
int DescendQ(point p, int i, int j);
void Descend(point p, int i, int j);
void AddLeaf(node root, point p, int i, int j);
void SuffixLink(point p, int i, int j);
node Ukkonnen(int k, char** Ti, int* ni);
void setNode(node n, int Ti, int head, int sdep, node child, node brother, node slink, node* hook);
void DFS(int* visited, node root);
void printTree(node root);

/* --------------------------------------------------------- */

int main(int argc, char *argv[]){
    
    /*Reading the input*/
    int k;                          /*Number of strings*/
    scanf("%d", &k);
    Ti = (char**)malloc(sizeof(char*)*k);                    /*List of all strings.*/
    ni = (int*)malloc(sizeof(int)*k);                      /*List of sizes of all strings*/

    int i = 0;
    int m = 0;
    while(i < k){
        scanf("%d", &m);            /*Size of string i*/
        ni[i] = m;
        char t[m+1];
        scanf("%s", t);             /*Actual string i*/
        Ti[i] = t;
        i++;
    }

    node suffix_tree_root = Ukkonnen(k, Ti, ni);
    printTree(suffix_tree_root);

    return 0;
}

char* saveNewString(char *S, int *S_cap, int *S_size){
    char c;
    int n;
    c = getchar(); /*Space char*/

    if (c == ' '){
        n = 0;
        c = getchar();
        while (c != '\n'){
            if( (n+1) == *S_cap){
                //S = increaseMemory(S, S_cap);
            }
            S[n] = c;
            n++;
            c = getchar();  
        }
        S[n] = '\0';
        *S_size = n;  
    }
    else{
        printf("Bad command format\n");
    }
    return S;
}

node Ukkonnen(int k, char** Ti, int* ni){
    int i = 0;

    /* Initializing sentinel*/
    node sentinel; 
    sentinel = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    sentinel ->id = -1;
    sentinel ->slink = NULL;

    /* Initializing root*/
    node root; 
    root = (node)malloc(sizeof(struct node));  /*TODO: check if malloc returns null*/
    root ->id = 0;
    root ->slink = sentinel;
    numNodes ++;

    
    /* Initializing point*/
    point p;
    p = (point)malloc(sizeof(struct point));
    p->a = root;
    p->s = 0;

    while(i < k){
        Ti[i][ni[i]] = '\1';      
        int j = 0;
        e = 0;
        while(j <= ni[i]){
            lastNewNode = NULL;
            while(!DescendQ(p, i, j)){
                AddLeaf(root, p, i, j);
                SuffixLink(p,i,j);
            }
            Descend(p, i, j);
            j++;
        }

        Ti[i][ni[i]] = '\0';
        i++;
    } 
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
    if( p->s > p->a->sdep ){
        edgePos = p->b->head + p->s;

        if(edgePos < ni[i]){
            edgeChar = Ti[i][edgePos];

            if(newChar == edgeChar) return 1;
        }
    }

    /* Case 3) We're in a internal node
        - We need to try all possible edges, traversing p->a->child and so on*/
    else{
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
    
    /* Case 1) We're in a edge*/
    if( p->s > p->a->sdep ){
        p->s ++;
    }

    /* Case 2) We're in a internal node
        - We need to try all possible edges, traversing p->a->child and so on*/
    else{
        node next;
        next = p->a->child;

        while(next != NULL){
            edgePos = next->head + p->s;
            if(edgePos < ni[i]){
                edgeChar = Ti[i][edgePos];
                if(newChar == edgeChar){
                    p->b = next;
                    p->s ++;
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

    node leaf; /* Our new leaf node*/
    leaf = (node)malloc(sizeof(struct node)); /*TODO: check if malloc returns null*/
    leaf->id = numNodes;
    numNodes ++;
   /* Case 1) Need to add only a leaf */
   if( p->s == 0){
       
        p->a->child = leaf;

        leaf->head = j - p->a->sdep;
        leaf->sdep = *e - leaf-> head;

        leaf->Ti = i;
        leaf->hook = &(p->a->child);
   }
    

   /* Case 2) Need to add internal node*/
    else{
        node internal; /* Our internal node*/
        internal = (node)malloc(sizeof(struct node)); /*TODO: check if malloc returns null*/
        internal->id = numNodes;
        numNodes++;
        internal->Ti = i;

        p->a->child = internal;
        internal->child = p->b;
        internal->brother = leaf;

        internal->head = p->a->head;
        leaf->head = j - p->s;

        internal->sdep= p->a->sdep + p->s;
        leaf->sdep = *e - leaf-> head;

        internal->slink = root; /* By default, any new internal node has its slink to root. This might change in future iterations*/ 
        if(lastNewNode != NULL){
            lastNewNode->slink = internal;
        }
        lastNewNode = internal;
    }
}

void SuffixLink(point p, int i, int j){
    p->s--;
    p->a = p->a->slink;

    int char_to_travel; /* Number of chars we need to pass when walking up the edge to p->above*/
    char_to_travel = p->s - p->a->sdep;


    
    
}

void setNode(node n, int Ti, int head, int sdep, node child, node brother, node slink, node* hook){
    n->Ti = Ti;
    n->head = head;
    n->sdep = sdep;
    n->child = child;
    n->brother = brother;
    n->hook = hook;
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
    printf("}");
}

void DFS(int* visited, node root){

    visited[root->id] = 1;
    node node_visited = root;
    node next;
    
    if (node_visited->child !=NULL){
        printf("\t%d -> %d [label=\"%s\"]\n", node_visited->id, node_visited->child->id, "edge label");
        DFS(visited, node_visited->child);
    }
    
    next = node_visited ->brother;
    while (next != NULL){
        printf("\t%d -> %d [label=\"%s\"]\n", node_visited->id, next->id, "edge label");
        DFS(visited, next);
        next = next->brother;
    }
}