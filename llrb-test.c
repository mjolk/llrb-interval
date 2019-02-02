/**
 * File   : llrb-test.c
 * License: MIT/X11
 * Author : Dries Pauwels <2mjolk@gmail.com>
 * Date   : di 11 sep 2018 07:55
 */
#define KEY_SIZE 5
#include <stdlib.h>
#include "slist.h"
#include "llrb.h"
#include "llrb-interval.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>


SLL_HEAD(range_group, node) lhead;
struct range_group *merge_list;
struct nptr {
    struct node* sle_next;
};
struct node {
    LLRB_ENTRY(node) entry;
    char start_key[KEY_SIZE];
    char end_key[KEY_SIZE];
    char max[KEY_SIZE];
    struct nptr next;

};

int intcmp(struct node *e1, struct node *e2)
{
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
    struct node *c, *prev, *nxt;
    nxt = SLL_FIRST(sll);
    SLL_INSERT_HEAD(sll, to_merge, next);
    if(nxt == 0) return;
    prev = SLL_FIRST(sll);
    while(nxt) {
        c = nxt;
        nxt = SLL_NEXT(nxt, next);
        if((strncmp(to_merge->start_key, c->end_key, KEY_SIZE)<= 0)&&
                (strncmp(to_merge->end_key, c->start_key, KEY_SIZE)>=0)) {
            /**overlap**/
            if(strncmp(to_merge->start_key, c->start_key, KEY_SIZE)>0){
                strncpy(to_merge->start_key, c->start_key, KEY_SIZE);
            }
            if(strncmp(to_merge->end_key, c->end_key, KEY_SIZE)<0) {
                strncpy(to_merge->end_key, c->end_key, KEY_SIZE);
            }
            SLL_REMOVE_AFTER(prev, next);
            free(LLRB_DELETE(range_tree, &head, c));
        } else {
            prev = c;
        }
    }
    struct node *ln;
    SLL_FOREACH(ln, sll, next){
        printf("sll list start:%s end:%s", ln->start_key, ln->end_key);
    }
}

int add_to_range(char *start, char *end) {
    struct node nn;
    strncpy(nn.start_key, start, KEY_SIZE);
    strncpy(nn.end_key, end, KEY_SIZE);
    strncpy(nn.max, "0000", KEY_SIZE);
    int grown = LLRB_RANGE_GROUP_ADD(range_tree, &head, &nn, merge_list, merge);
    if(grown > 0){
        printf("range has grown\n");
        struct node *add = malloc(sizeof(struct node));
        if(add == 0) return 1;
        *add = nn;
        LLRB_INSERT(range_tree, &head, add);
    } else {
        printf("range already fully contained\n");
    }
    return grown;
}

void delete_tree(){
    struct node *n = LLRB_MIN(range_tree, &head);
    while(n){
        struct node *c = n;
        n = LLRB_NEXT(range_tree, &head, n);
        free(LLRB_DELETE(range_tree, &head, c));
    }
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
        strncpy(n->start_key, testdata[i][0], KEY_SIZE);
        strncpy(n->end_key, testdata[i][1], KEY_SIZE);
        memset(n->max, 0, KEY_SIZE);
        //  printf("max %s\n", n->max);
        LLRB_INSERT(range_tree, &head, n);
    }
    LLRB_FOREACH(n, range_tree, &head) {
        printf("%s\n", n->start_key);
    }
    printf("reverse\n");
    LLRB_FOREACH_REVERSE(n, range_tree, &head) {
        printf("%s\n", n->start_key);
    }
    print_tree(LLRB_ROOT(&head));
    struct node d;
    strncpy(d.start_key, "0013", KEY_SIZE);
    struct node *deleted;
    deleted = LLRB_DELETE(range_tree, &head, &d);
    free(deleted);
    print_tree(LLRB_ROOT(&head));
    printf("\n");
    struct node *in;
    if ((in = malloc(sizeof(struct node))) == NULL)
        err(1, NULL);
    strncpy(in->start_key, "0020", KEY_SIZE);
    strncpy(in->end_key, "0030", KEY_SIZE);
    strncpy(in->max, "0000", KEY_SIZE);
    struct node *r = LLRB_INSERT(range_tree, &head, in);
    if(r){
        printf("insert failed for %s\n", in->start_key);
        assert(r != 0);
        free(in);
    }
    print_tree(LLRB_ROOT(&head));
    delete_tree();

    LLRB_INIT(&head);
    SLL_INIT(merge_list);

    assert(add_to_range("0012", "0046") > 0);
    assert(add_to_range("0001", "0002") > 0);
    assert(add_to_range("0006", "0100") > 0);
    assert(add_to_range("0022", "0078") == 0);
    assert(add_to_range("0222", "0788") > 0);
    assert(add_to_range("0300", "0800") > 0);
    assert(add_to_range("0223", "0787") == 0);
    assert(add_to_range("0002", "0005") > 0);
    assert(add_to_range("0050", "0069") == 0);
    assert(add_to_range("0030", "0089") == 0);
    //add_to_range(last);
    //
    assert(add_to_range("0001", "0004") == 0);

    print_tree(LLRB_ROOT(&head));

    delete_tree();
    return (0);
}
