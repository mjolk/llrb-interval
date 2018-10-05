/**
 * File   : llrb-test.c
 * License: MIT/X11
 * Author : Dries Pauwels <2mjolk@gmail.com>
 * Date   : di 11 sep 2018 07:55
 */
#include <stdlib.h>
#include "slist.h"
#include "llrb-interval.h"
#include "llrb.h"
#include <err.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


SLL_HEAD(range_group, node) lhead;
struct range_group *merge_list;
struct nptr {
    struct node* sle_next;
};
struct node { 
    LLRB_ENTRY(node) entry; 
    char start_key[5];
    char end_key[5];
    char max[5];
    struct nptr next;

};

int intcmp(struct node *e1, struct node *e2) 
{
    printf("char1: %s char2: %s\n", e1->start_key, e2->start_key);
   return strncmp(e1->start_key, e2->start_key, 4);//memcmp(e1->start_key, e2->start_key, 4); 
} 
LLRB_HEAD(range_tree, node);
struct range_tree head;
LLRB_PROTOTYPE(range_tree, node, entry, intcmp);
    LLRB_GENERATE(range_tree, node, entry, intcmp)
LLRB_RANGE_GROUP_GEN(range_tree, node, entry, range_group, next)	

    char (*testdata[8][2]) = { 
        {"0020", "0040"}, {"0016", "0018"}, {"0005", "0017"}, {"0013", "0099"}, {"0003", "0008"}, 
        {"0100", "0300"}, {"0056", "0057"}, {"0299", "1000"} 
    }; 

void print_tree(struct node *n) 
{ 
    struct node *left, *right; 

    if (n == NULL) { 
        printf("nil"); 
        return; 
    }
    left = LLRB_LEFT(n, entry); 
    right = LLRB_RIGHT(n, entry); 
    if (left == NULL && right == NULL) 
        printf("END [range %s, %s max: %s]", n->start_key, n->end_key, n->max); 
    else { 
        printf("LR[range %s, %s max: %s](", n->start_key, n->end_key, n->max); 
        print_tree(left); 
        printf(","); 
        print_tree(right); 
        printf(")"); 
    } 
}

void merge(struct node *to_merge, struct range_group *sll) {
    /*struct range *to_merge = malloc(sizeof(struct range));
    if(to_merge == 0) return;
    to_merge->node = node_to_merge;
    to_merge->start_key = node_to_merge->start_key;
    to_merge->end_key = node_to_merge->end_key;*/
    printf("<<<<<<<<<<<< MERGE >>>>>>>>>\n");
    printf("address: %p start: %s end: %s\n", to_merge, to_merge->start_key, to_merge->end_key);
    struct node *c, *prev, *nxt;
    nxt = SLL_FIRST(sll);
    SLL_INSERT_HEAD(sll, to_merge, next);
    if(nxt == 0) return;
    prev = SLL_FIRST(sll);
    while(nxt) {
        printf("CURRENT: %s addr: %p\n", nxt->start_key, nxt);
        c = nxt;
        nxt = SLL_NEXT(nxt, next);
        printf("merging %s/%s with %s/%s\n", to_merge->start_key, to_merge->end_key, c->start_key, c->end_key);
        if((strncmp(to_merge->start_key, c->end_key, 4)<= 0)&& (strncmp(to_merge->end_key, c->start_key, 4)>=0)) {
            /**overlap**/
            printf("start: %s end: %s OVERLAPS start: %s end: %s\n", to_merge->start_key, to_merge->end_key, c->start_key, c->end_key);
            if(strncmp(to_merge->start_key, c->start_key, 4)>0){
                strncpy(to_merge->start_key, c->start_key, 5);
            }
            if(strncmp(to_merge->end_key, c->end_key, 4)<0) {
                strncpy(to_merge->end_key, c->end_key, 5);
            }
            printf("<<remove>>\n");
            SLL_REMOVE_AFTER(prev, next);
            //printf("removed from filter list ::%p\n", c);
            LLRB_DELETE(range_tree, &head, c);
            printf("<<removed from tree>> %p\n", c);
            free(c);
        } else {
            prev = c;
        }
    }
}

struct node* new_range(char *start, char *end) {
    struct node *add_range;
    if ((add_range = malloc(sizeof(struct node))) == NULL) 
        err(1, NULL);
    strncpy(add_range->start_key, start, 5);
    strncpy(add_range->end_key, end, 5);
    strncpy(add_range->max, "0000", 5);
    return add_range;
}

int add_to_range(struct node* nn) {
    int grown = LLRB_RANGE_GROUP_ADD(range_tree, &head, nn, merge_list, merge);
    if(grown > 0){
        printf("range has grown\n");
    } else {
        printf("range already fully contained\n");
    }
    return grown;
}

int main(void) 
{
    merge_list = &lhead;	
    SLL_INIT(merge_list);
    size_t i; 
    struct node *n;
    LLRB_INIT(&head);	

    for (i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) { 
        if ((n = malloc(sizeof(struct node))) == NULL) 
            err(1, NULL);
       printf("est data: %s\n", testdata[i][1]); 
        strncpy(n->start_key, testdata[i][0], 5);
        printf("start key copied: %s\n", n->start_key);
       printf("start_key %s\n", testdata[i][0]); 
        strncpy(n->end_key, testdata[i][1], 5);
       printf("end_key %s\n", testdata[i][1]); 
        memset(n->max, 0, 4);	
     //  printf("max %s\n", n->max);
      printf("adding key: %s\n", n->start_key); 
        LLRB_INSERT(range_tree, &head, n); 
    }	
printf("loaded data --->>>\n");
    LLRB_FOREACH(n, range_tree, &head) { 
        printf("%s\n", n->start_key); 
    }
    LLRB_FOREACH_REVERSE(n, range_tree, &head) {
        printf("%s\n", n->start_key);
    }
    print_tree(LLRB_ROOT(&head)); 
    printf("\n");
    struct node d;
    strncpy(d.start_key, "0013", 5);
    struct node *deleted;
    deleted = LLRB_DELETE(range_tree, &head, &d);
    print_tree(LLRB_ROOT(&head));
    printf("\n");
    free(deleted);
    struct node *in;
    if ((in = malloc(sizeof(struct node))) == NULL) 
        err(1, NULL); 
    strncpy(in->start_key, "0020", 5);
    strncpy(in->end_key, "0030", 5);
    strncpy(in->max, "0000", 5);
    struct node *r = LLRB_INSERT(range_tree, &head, in);
    if(r){
        printf("insert failed for %s\n", in->start_key);
    }
    print_tree(LLRB_ROOT(&head));


    LLRB_INIT(&head);	
    SLL_INIT(merge_list);

    add_to_range(new_range("0012", "0046"));
    add_to_range(new_range("0001", "0002"));
    add_to_range(new_range("0006", "0100"));
    add_to_range(new_range("0022", "0078"));
    add_to_range(new_range("0222", "0788"));
    struct node *last = new_range("0300", "0800");
    printf("adding start %s end: %s to range \n", last->start_key, last->end_key);
    add_to_range(last);
    add_to_range(new_range("0222", "0788"));
    add_to_range(new_range("0002", "0005"));
    add_to_range(new_range("0050", "0069"));
    add_to_range(new_range("0030", "0089"));
    //add_to_range(last);
    //
    add_to_range(new_range("0001", "0004"));

    print_tree(LLRB_ROOT(&head));
    return (0); 
}
