#define KEY_SIZE 5
#include "llrb.h"

#include <assert.h>
#include <err.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llrb-interval.h"

SLIST_HEAD(range_group, node) lhead;
struct range_group *merge_list;
struct nptr {
  struct node *sle_next;
};
struct node {
  LLRB_ENTRY(node) entry;
  char start_key[KEY_SIZE];
  char end_key[KEY_SIZE];
  char max[KEY_SIZE];
  struct nptr next;
};

int intcmp(struct node *e1, struct node *e2) {
  return strcmp(e1->start_key,
                e2->start_key);  // memcmp(e1->start_key, e2->start_key, 4);
}
LLRB_HEAD(range_tree, node);
struct range_tree head;
LLRB_PROTOTYPE(range_tree, node, entry, intcmp);
LLRB_GENERATE(range_tree, node, entry, intcmp)
LLRB_RANGE_GROUP_GEN(range_tree, node, entry, range_group, next)

char(*testdata[8][2]) = {{"0020", "0040"}, {"0016", "0018"}, {"0005", "0017"},
                         {"0013", "0099"}, {"0003", "0008"}, {"0100", "0300"},
                         {"0056", "0057"}, {"0299", "1000"}};

void print_tree(struct node *n) {
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

void merge(struct range_tree *t, struct node *to_merge,
           struct range_group *sll) {
  (void)t;
  struct node *c, *prev, *nxt;
  nxt = SLIST_FIRST(sll);
  SLIST_INSERT_HEAD(sll, to_merge, next);
  if (nxt == 0) return;
  prev = SLIST_FIRST(sll);
  while (nxt) {
    c = nxt;
    nxt = SLIST_NEXT(nxt, next);
    if ((strcmp(to_merge->start_key, c->end_key) <= 0) &&
        (strcmp(to_merge->end_key, c->start_key) >= 0)) {
      /**overlap**/
      if (strcmp(to_merge->start_key, c->start_key) > 0) {
        strcpy(to_merge->start_key, c->start_key);
      }
      if (strcmp(to_merge->end_key, c->end_key) < 0) {
        strcpy(to_merge->end_key, c->end_key);
      }
      SLIST_REMOVE_AFTER(prev, next);
      free(LLRB_DELETE(range_tree, &head, c));
    } else {
      prev = c;
    }
  }
  struct node *ln;
  SLIST_FOREACH(ln, sll, next) {
    printf("sll list start:%s end:%s", ln->start_key, ln->end_key);
  }
}

int add_to_range(char *start, char *end) {
  struct node nn;
  strcpy(nn.start_key, start);
  strcpy(nn.end_key, end);
  strcpy(nn.max, "0000");
  int grown = LLRB_RANGE_GROUP_ADD(range_tree, &head, &nn, merge_list, merge);
  if (grown > 0) {
    printf("range has grown\n");
    struct node *add = malloc(sizeof(struct node));
    if (add == 0) return 1;
    *add = nn;
    LLRB_INSERT(range_tree, &head, add);
  } else {
    printf("range already fully contained\n");
  }
  return grown;
}

void delete_tree() {
  struct node *n = LLRB_MIN(range_tree, &head);
  while (n) {
    struct node *c = n;
    n = LLRB_NEXT(range_tree, &head, n);
    free(LLRB_DELETE(range_tree, &head, c));
  }
}

int main(void) {
  merge_list = &lhead;
  SLIST_INIT(merge_list);
  size_t i;
  struct node *n;
  LLRB_INIT(&head);

  for (i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
    if ((n = malloc(sizeof(struct node))) == NULL) err(1, NULL);
    strcpy(n->start_key, testdata[i][0]);
    strcpy(n->end_key, testdata[i][1]);
    memset(n->max, 0, KEY_SIZE);
    //  printf("max %s\n", n->max);
    LLRB_INSERT(range_tree, &head, n);
  }
  LLRB_FOREACH(n, range_tree, &head) { printf("%s\n", n->start_key); }
  printf("reverse\n");
  LLRB_FOREACH_REVERSE(n, range_tree, &head) { printf("%s\n", n->start_key); }
  print_tree(LLRB_ROOT(&head));
  struct node d;
  strcpy(d.start_key, "0013");
  struct node *deleted;
  deleted = LLRB_DELETE(range_tree, &head, &d);
  free(deleted);
  print_tree(LLRB_ROOT(&head));
  printf("\n");
  struct node *in;
  if ((in = malloc(sizeof(struct node))) == NULL) err(1, NULL);
  strcpy(in->start_key, "0020");
  strcpy(in->end_key, "0030");
  strcpy(in->max, "0000");
  struct node *r = LLRB_INSERT(range_tree, &head, in);
  if (r) {
    printf("insert failed for %s\n", in->start_key);
    assert(r != 0);
    free(in);
  }
  print_tree(LLRB_ROOT(&head));
  delete_tree();

  LLRB_INIT(&head);
  SLIST_INIT(merge_list);

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
  // add_to_range(last);
  //
  assert(add_to_range("0001", "0004") == 0);

  print_tree(LLRB_ROOT(&head));

  delete_tree();
  return (0);
}
