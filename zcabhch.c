//
//  main.c
//  Tableau coursework
//
//  Created by harrychen on 16/12/22.
//  Copyright © 2016年 harrychen. All rights reserved.
//

#include <stdio.h>
#include <string.h>   /* for all the new-fangled string functions */
#include <stdlib.h>     /* malloc, free, rand */

int Fsize=50;
int cases=10;

int parse(char *a);
struct tableau;
void complete(struct tableau *t);

/*typedef struct tableau tableau;*/


struct tableau {
    char *root;
    struct tableau *left;
    struct tableau *right;
    struct tableau *parent;
}*tab, *node, *node1, *kid, *pa; //定义tableau的同时定义5个指向tableau类型的指针.



//Here are the functions for parsers
//int parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0

int variable (char a){
    if ((a=='p') || (a=='q') || (a=='r')) {
        return 1;
    }
    else return 0;
}

int neg(char *a){
    if (a[0]=='-' && parse(a+1)!=0) {
        return 1;
    }
    else return 0;
}

int binconnective(char a){
    if ((a=='>')||(a=='^')||(a=='v')) {
        return 1;
    }
    else return 0;
}

int posofbin (char *a){
    int left=0;
    int right=0;
    int pos=0;
    while (*a) {
        if (left-right==1 && binconnective(*a)) {
            return pos;
        }
        if (*a=='(') {
            left+=1;
        }
        if (*a==')') {
            right+=1;
        }
        a++;
        pos++;
    }
    return pos;
}
// These two functions are for binary formula only

char* getpartone(char *a){      //    char* a="((p>q)^(q>p))";
    char *result=(char*)malloc(sizeof(char)*strlen(a));
    char *temp=(char*)malloc(sizeof(char)*strlen(a));
    for (int i=0; i<strlen(a)-2; i++) {
        temp[i]=a[i+1];
    }
    for (int j=0; j<posofbin(a)-1; j++) {
        result[j]=temp[j];
    }
    result[posofbin(a)-1]='\0';
    return result;
}



char *getparttwo(char *a){
    
    int k=0;
    char *result=(char*)malloc(sizeof(char)*strlen(a));
    char *temp=(char*)malloc(sizeof(char)*strlen(a));
    for (int i=0; i<strlen(a)-2; i++) {
        temp[i]=a[i+1];
    }
    for (int j=posofbin(a); temp[j]!='\0'; k++,j++) {
        result[k]=temp[j];
    }
//    result[k]='\0';
    return result;
    
}

int binary(char *a){
    if (*a=='(') {
        char *one=getpartone(a);
        char *two=getparttwo(a);
        
        if (parse(one)!=0 && parse(two)!=0) {
            return 1;
        }
    
    }
    return 0;
}

int parse(char *a){
    if (variable(a[0])&& a[1]=='\0') {
        return 1;
    }
    else if (neg(a)) {
        return 2;
    }
    else if (binary(a)){
        return 3;
    }
    return 0;
}


//Here are the functions for complete()
//void complete(struct tableau *t)
//parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
//which expands the root of the tableau and recursively completes any child tableaus.

char *delete_negative(char *a){
    char* te=(char*)malloc(sizeof(char)*strlen(a));
    int i = 0;
    for (; a[i]!='\0'; i++) {
        te[i]=a[i+1];
    }
    te[i] = '\0';
    return te;
}

char *add_negative(char *a){
    char* et=(char*)malloc(sizeof(char)*(strlen(a)+3));
    int i=0;
    et[0]='-';
    for (; a[i]!='\0'; i++) {
        et[i+1]=a[i];
    }
    return et;
}

// get the type of input for expansion: 0 for not formula, 1 for prop:(q, -q); 2 for double negation; 3 for alpha; 4 for beta
int gettype(char *a){
    if ( parse(a)==1 || ((parse(a)==2) && (strlen(a)==2)) ) {
        return 1;
    }
    if (parse(a)==2 && parse(delete_negative(a))==2) { //double negation
        return 2;
    }
    if (parse(a)==3) { //if binary
        int n=posofbin(a);
        char c=a[n];
        switch (c) {
            case '^':
                return 3;
                break;
            case 'v':
                return 4;
                break;
            case '>':
                return 4;
                break;
            default:return 0;
                break;
        }
    }
    if (parse(a)==2 && parse(delete_negative(a))==3) {  //if neg-binary
        char *c=delete_negative(a);
        int n=posofbin(c);
        char d=c[n];
        switch (d) {
            case '^':
                return 4;
                break;
            case 'v':
                return 3;
                break;
            case '>':
                return 3;
                break;
            default:return 0;break;
        }
    }
    return 0;
}


//parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
//This is the function for getting first part of formula after applying splitting rules, also include double negation
char* get_expansion_one(char*a){
    if (parse(a)==3) { // split binary
        int n=posofbin(a);
        char c=a[n];
        switch (c) {
            case '^':
                return getpartone(a);
                break;
            case 'v':
                return getpartone(a);
                break;
            case '>':
                return add_negative(getpartone(a));
                break;
            default:
                break;
        }
    }
    else if(parse(a)==2 && parse(delete_negative(a))==3){ // negative binary
        char *temp=(char*)malloc(sizeof(char)*strlen(a));
        temp=delete_negative(a);
        int k=posofbin(temp);
        char t=temp[k];
        switch (t) {
            case '^':
                return add_negative(getpartone(temp));
                break;
            case 'v':
                return add_negative(getpartone(temp));
                break;
            case '>':
                return getpartone(temp);
                break;
            default:
                break;
        }
    }
    else if (parse(a)==2 && parse(delete_negative(a))==2){ //double negation
        return delete_negative(delete_negative(a));
    }
    return 0;
}

//This is the function for getting seconf part of formula after applying splitting rules, exclude double negation
//parse(char *g) which returns 1 if a proposition, 2 if neg, 3 if binary, ow 0
char* get_expansion_two(char* a){
    if (parse(a)==3) { // split binary
        int n=posofbin(a);
        char c=a[n];
        switch (c) {
            case '^':
                return getparttwo(a);
                break;
            case 'v':
                return getpartone(a);
                break;
            case '>':
                return getparttwo(a);
                break;
            default:
                break;
        }
    }
    else if(parse(a)==2 && parse(delete_negative(a))==3){ // negative binary
        char *temp=(char*)malloc(sizeof(char)*strlen(a));
        temp=delete_negative(a);
        int k=posofbin(temp);
        char t=temp[k];
        switch (t) {
            case '^':
                return add_negative(getparttwo(temp));
                break;
            case 'v':
                return add_negative(getparttwo(temp));
                break;
            case '>':
                return add_negative(getparttwo(temp));
                break;
            default:
                break;
        }
    }
    return 0;

}


void add_left_node(struct tableau *root, char *g){
    struct tableau *new=(struct tableau*)malloc(sizeof(struct tableau));
    new->root=g;
    new->left=NULL;
    new->right=NULL;
    root->left=new;
    new->parent=root;
}

void add_right_node(struct tableau *root, char *g){
    struct tableau *new=malloc(sizeof(struct tableau));
    new->root=g;
    new->right=NULL;
    new->left=NULL;
    root->right=new;
    new->parent=root;
}

//void complete(struct tableau *t)
//which expands the root of the tableau and recursively completes any child tableaus.

void add_node_alpha(struct tableau *a, char * b){
    if (a->left==NULL) {
        add_left_node(a, b);
    }
    else add_node_alpha(a->left,b);
    if (a->right==NULL) {
        //DO NOTHING
    }
    else add_node_alpha(a->right, b);
}

void apply_alpha(struct tableau *a, char* first_part, char* second_part){
    add_node_alpha(a, first_part);
    add_node_alpha(a, second_part);
}

void apply_beta(struct tableau *a, char* first_part, char* second_part){
    //be noticed that if we apply alpha rule first, there may EXIST left child and left child of left child,
    //so once apply beta rule, first need to check whether the left node is empty.
    if((*a).left == NULL) {
        add_left_node(a,first_part);
        add_right_node(a,second_part);
    } else {
        apply_beta((*a).left, first_part, second_part);
        if((*a).right != NULL) { apply_beta((*a).right, first_part, second_part); }
    }
    
}

//get the type of input for expansion: 0 for not formula, 1 for prop:(q, -q); 2 for double negation; 3 for alpha; 4 for beta
void complete(struct tableau* a){
    if (a!=NULL) {
        char *root =a->root;
        switch (gettype(root)) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                a->root=get_expansion_one(a->root);
                complete(a);
                break;
            case 3:
                apply_alpha(a, get_expansion_one(a->root), get_expansion_two(a->root));
                break;
            case 4:
                apply_beta(a, get_expansion_one(a->root), get_expansion_two(a->root));
                break;
            default:
                break;
        }
        if(a->left!=NULL) complete(a->left);
        if(a->right!=NULL) complete(a->right);
    }
}


int help_testing(struct tableau *a, int *stat) {
    char* root = a->root;
    int newstat[6];
    memcpy(newstat, stat, 6 * sizeof(int));
    
    if(parse(root) == 1) {
        switch(root[0]) {
            case 'p': newstat[0] = 1; break;
            case 'q': newstat[1] = 1; break;
            case 'r': newstat[2] = 1; break;
        }
    }
    if(parse(root) == 2 && parse(delete_negative(root)) == 1) {
        switch(root[1]) {
            case 'p': newstat[3] = 1; break;
            case 'q': newstat[4] = 1; break;
            case 'r': newstat[5] = 1; break;
        }
    }

    int i;
    for(i = 0; i < 3; i++) {
        if(newstat[i] == 1 && newstat[i+3] == 1) { return 1; }
    }
    
    if(a->left == NULL && a->right == NULL)  return 0;
    if(a->left != NULL && a->right == NULL)  return help_testing(a->left, newstat);
    if(a->left != NULL && a->right != NULL)  return (help_testing(a->left, newstat)&& help_testing(a->right, newstat));
    return 0;
}

int closed(struct tableau *a) {   //return 1 if it is not satisfiable, 0 if satisfiable
    //checks if tableau is closed, returns 1 if it is, 0 otherwise
    int stat[6] = {0,0,0,0,0,0};
    char* root = a->root;
    
    if(parse(root) == 0) { return 1; }     if(parse(root) == 1) {
        switch(root[0]) {
            case 'p': stat[0] = 1; break;
            case 'q': stat[1] = 1; break;
            case 'r': stat[2] = 1; break;
            }
    }
    if(parse(root) == 2 && parse(delete_negative(root)) == 1) {
        //neg prop
        switch(root[1]) {
            case 'p': stat[3] = 1; break;
            case 'q': stat[4] = 1; break;
            case 'r': stat[5] = 1; break;
        }
    }
    
    if(a->left == NULL && a->right == NULL)  return 0;
    if(a->left != NULL && a->right == NULL)  return help_testing(a->left, stat);
    if(a->left != NULL && a->right != NULL)  return (help_testing(a->left, stat) && help_testing(a->right, stat));
    return 0;
}

//Testing code
//int main(){
//    char* a="((p>q)^(q>p))";  // test getpartone/two()
//    char* a="-((p>(qvr))>((p>q)v(p>r)))";
//    char* a="((p>(qvr))>((p>q)v(p>r)))";
//    char *two=getparttwo(a);
//    printf("%s\n",two);
//    return 0;

    
    
//    char* c="-(p>q)"; //test parse
//    int a=parse(c);
//    printf("%d\n",a);
//    return 0;
    
//    struct tableau a={"-(p>q)"};   //test apply_alpha_rule
//    struct tableau *c=&a;
//    apply_alpha(c, "p", "-q");
//    printf("%s\n",(c->left)->root);
//    return 0;
    
//    struct tableau a={"(p>q)"};   //test apply_beta_rule
//    struct tableau *c=&a;
//    apply_beta(c, "-p", "q");
//    printf("%s\n",(c->right)->root);
//    return 0;
    
//    struct tableau a={"-((p>(qvr))>((p>q)v(p>r)))"};  //test complete()
//    struct tableau *c=&a;
//    complete(c);
////    int n=0;              //注意:是right node为Null, right->root会造成错误,显示不出null.
////    if ((c->right)==NULL) {
////        n=1;
////    }
////    printf("%d\n",n);
//    printf("%s\n",((((c->left)->left))->root));
//    return 0;
    
//    struct tableau a={"-(p>p)"};  //return 0 if it is satsfiable
//    struct tableau *c=&a;
//    complete(c);
//    int i=closed(c);
//    printf("%d\n",i);
//    return 0;
//}




int main()

{ /*input a string and check if its a propositional formula */
    
    
    char *name = malloc(Fsize);
    FILE *fp, *fpout;
    
    /* reads from input.txt, writes to output.txt*/
    if ((  fp=fopen("input.txt","r"))==NULL){printf("Error opening file");exit(1);}
    if ((  fpout=fopen("output.txt","w"))==NULL){printf("Error opening file");exit(1);}
    
    int j;
    for(j=0;j<cases;j++)
    {
        fscanf(fp, "%s",name);/*read formula*/
        switch (parse(name)){
            case(0): fprintf(fpout, "%s is not a formula.  ", name);break;
            case(1): fprintf(fpout, "%s is a proposition.  ", name);break;
            case(2): fprintf(fpout, "%s is a negation.  ", name);break;
            case(3):fprintf(fpout, "%s is a binary.  ", name);break;
            default:fprintf(fpout, "What the f***!  ");
        }
        
        /*make new tableau with name at root, no children, no parent*/
        
        struct tableau t={name, NULL, NULL, NULL};
        
        /*expand the root, recursively complete the children*/
        if (parse(name)!=0)
        { complete(&t);
            if (closed(&t)) fprintf(fpout, "%s is not satisfiable.\n", name);
            else fprintf(fpout, "%s is satisfiable.\n", name);
        }
        else fprintf(fpout, "I told you, %s is not a formula.\n", name);
    }
    
    fclose(fp);
    fclose(fpout);
    free(name);
    
    return(0);
}

















