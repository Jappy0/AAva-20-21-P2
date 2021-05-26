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

/* -----------------------NODE STRUCT----------------------- */

typedef struct node* node;
struct node{
    int Ti;                       /**< The value of i in Ti */
    int head;                     /**< The path-label startat &(Ti[head]) */
    int sdep;                     /**< String-Depth */
    node child;                   /**< Child */
    node brother;                 /**< brother */
    node slink;                   /**< Suffix link */
    node* hook;                   /**< What keeps this linked? */
};

/* -----------------------POINT STRUCT---------------------- */
typedef struct point* point;
struct point{
    node a;                       /**< node above */
    node b;                       /**< node bellow */
    int s;                        /**< String-Depth */
};

/* --------------------------------------------------------- */
char* saveNewString(char *S, int *S_cap, int *S_size);
bool DescendQ(point* p, char T_ij);
node* Ukkonnen(int k, char** Ti, int* ni);

/* --------------------------------------------------------- */

int main(int argc, char *argv[]){
    
    /*Reading the input*/
    int k;                          /*Number of strings*/
    scanf("%d", &k);
    char* Ti[k];                    /*List of all strings.*/
    int ni[k];                      /*List of sizes of all strings*/

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

    node* suffix_tree = Ukkonnen(k, Ti, ni);

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

node* Ukkonnen(int k, char** Ti, int* ni){
    int i = 0;
    /*while(i < k){
        Ti[i][ni[i]] = '\1';      

        int j = 0;
        while(j <= ni[i]){
            while(!DescendQ(p, Ti[i][j])){
                AddLeaf(p, i, j);
                SuffixLink(p);
            }
            Descend(p, Ti[i][j]);j++;}

        Ti[i][ni[i]] = '\0';
        i++;
    } */
    
}

bool DescendQ(point* p, char T_ij){
    return false;
}
