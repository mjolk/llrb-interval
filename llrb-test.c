/**
 * File   : ../llrb-interval/llrb-test.c
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


SLL_HEAD(range_group, node) lhead;
struct range_group *merge_list;
struct nptr {
    struct node* sle_next;
};
struct node { 
    LLRB_ENTRY(node) entry; 
    uint64_t start_key;
    uint64_t end_key;
    uint64_t max;
    struct nptr next;

};

struct range {
    uint64_t start_key;
    uint64_t end_key;
    struct node *node;
    SLL_ENTRY(range) next;
};

int intcmp(struct node *e1, struct node *e2) 
{ 
    return (e1->start_key < e2->start_key ? -1 : e1->start_key > e2->start_key); 
} 
LLRB_HEAD(range_tree, node);
struct range_tree head;
LLRB_PROTOTYPE(range_tree, node, entry, intcmp);
    LLRB_GENERATE(range_tree, node, entry, intcmp)
LLRB_RANGE_GROUP_GEN(range_tree, node, entry, range_group, next)	

    int testdata[8][2] = { 
        {20, 40}, {16, 18}, {5, 17}, {13, 99}, {3, 8}, {100, 300},
        {56, 57}, {299, 1000} 
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
        printf("END [range %lu, %lu max: %lu]", n->start_key, n->end_key, n->max); 
    else { 
        printf("LR[range %lu, %lu max: %lu](", n->start_key, n->end_key, n->max); 
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
    printf("address: %p start: %lu end: %lu\n", to_merge, to_merge->start_key, to_merge->end_key);
    struct node *c, *prev, *nxt;
    nxt = SLL_FIRST(sll);
    SLL_INSERT_HEAD(sll, to_merge, next);
    if(nxt == 0) return;
    prev = SLL_FIRST(sll);
    while(nxt) {
        printf("CURRENT: %lu addr: %p\n", nxt->start_key, nxt);
        c = nxt;
        nxt = SLL_NEXT(nxt, next);
        printf("merging %lu/%lu with %lu/%lu\n", to_merge->start_key, to_merge->end_key, c->start_key, c->end_key);
        if(to_merge->start_key <= c->end_key && to_merge->end_key >= c->start_key) {
            /**overlap**/
            printf("start: %lu end: %lu OVERLAPS start: %lu end: %lu\n", to_merge->start_key, to_merge->end_key, c->start_key, c->end_key);
            if(to_merge->start_key > c->start_key){
                to_merge->start_key = to_merge->start_key = c->start_key;
            }
            if(to_merge->end_key < c->end_key) {
                to_merge->end_key = to_merge->end_key = c->end_key;
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

struct node* new_range(int start, int end) {
    struct node *add_range;
    if ((add_range = malloc(sizeof(struct node))) == NULL) 
        err(1, NULL);
    add_range->start_key = start;
    add_range->end_key = end;
    add_range->max = 0;
    return add_range;
}

int add_to_range(struct node* nn) {
    SLL_INIT(merge_list);
    int grown = LLRB_RANGE_GROUP_ADD(range_tree, &head, nn, merge_list, &merge);
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
        n->start_key = testdata[i][0];
        n->end_key = testdata[i][1];
        n->max = 0;	
        LLRB_INSERT(range_tree, &head, n); 
    }	

    LLRB_FOREACH(n, range_tree, &head) { 
        printf("%lu\n", n->start_key); 
    }
    LLRB_FOREACH_REVERSE(n, range_tree, &head) {
        printf("%lu\n", n->start_key);
    }
    print_tree(LLRB_ROOT(&head)); 
    printf("\n");
    struct node d = {.start_key = 13};
    struct node *deleted;
    deleted = LLRB_DELETE(range_tree, &head, &d);
    print_tree(LLRB_ROOT(&head));
    printf("\n");
    free(deleted);
    struct node *in;
    if ((in = malloc(sizeof(struct node))) == NULL) 
        err(1, NULL); 
    in->start_key = 20;
    in->end_key = 30;
    in->max = 0;
    //LLRB_INSERT(range_tree, &head, in);
    print_tree(LLRB_ROOT(&head));


    LLRB_INIT(&head);	
    SLL_INIT(merge_list);

    add_to_range(new_range(12, 46));
    add_to_range(new_range(1, 2));
    add_to_range(new_range(6, 100));
    add_to_range(new_range(22, 78));
    add_to_range(new_range(222, 788));
    struct node *last = new_range(300, 800);
    add_to_range(last);
    add_to_range(new_range(222, 788));
    add_to_range(new_range(2, 5));
    add_to_range(new_range(50, 69));
    add_to_range(new_range(30, 89));
    //add_to_range(last);
    //
    add_to_range(new_range(1, 4));

    print_tree(LLRB_ROOT(&head));
    return (0); 
}
