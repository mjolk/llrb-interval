/**
 * expanded macro version for debugging
 *
 */

#include <assert.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "llrb.h"
#include "slist.h"

#define LLRB_ENTRY(type)                            \
  struct {                                          \
    struct type *rbe_left, *rbe_right, *rbe_parent; \
    _Bool rbe_color;                                \
  }

#define LLRB_LEFT(elm, field) (elm)->field.rbe_left
#define LLRB_RIGHT(elm, field) (elm)->field.rbe_right
#define LLRB_PARENT(elm, field) (elm)->field.rbe_parent
#define LLRB_EDGE(head, elm, field)                         \
  (((elm) == LLRB_ROOT(head)) ? &LLRB_ROOT(head)            \
   : ((elm) == LLRB_LEFT(LLRB_PARENT((elm), field), field)) \
       ? &LLRB_LEFT(LLRB_PARENT((elm), field), field)       \
       : &LLRB_RIGHT(LLRB_PARENT((elm), field), field))
#define LLRB_COLOR(elm, field) (elm)->field.rbe_color
#define LLRB_ROOT(head) (head)->rbh_root
#define LLRB_EMPTY(head) ((head)->rbh_root == 0)
#define LLRB_ISRED(elm, field) ((elm) && LLRB_COLOR((elm), field) == LLRB_RED)

SLL_HEAD(range_group, node) lhead;
struct range_group *merge_list;
struct nptr {
  struct node *sle_next;
};
struct node {
  LLRB_ENTRY(node) entry;
  char start_key[5];
  char end_key[5];
  char max[5];
  struct nptr next;
};

int intcmp(struct node *e1, struct node *e2) {
  return strncmp(e1->start_key, e2->start_key,
                 4);  // memcmp(e1->start_key, e2->start_key, 4);
}
// LLRB_HEAD(range_tree, node);
struct range_tree head;
// LLRB_PROTOTYPE(range_tree, node, entry, intcmp);
// LLRB_GENERATE(range_tree, node, entry, intcmp)
// LLRB_RANGE_GROUP_GEN(range_tree, node, entry, range_group, next)

struct range_tree {
  struct node *rbh_root;
};
struct range_tree head;
struct node *range_tree_LLRB_INSERT(struct range_tree *, struct node *);
struct node *range_tree_LLRB_DELETE(struct range_tree *, struct node *);
struct node *range_tree_LLRB_FIND(struct range_tree *, struct node *);
struct node *range_tree_LLRB_MIN(struct node *);
struct node *range_tree_LLRB_MAX(struct node *);
struct node *range_tree_LLRB_NEXT(struct node *);
struct node *range_tree_LLRB_PREV(struct node *);
;
static inline void range_tree_LLRB_ROTL(struct node **pivot) {
  struct node *a = *pivot;
  struct node *b = (a)->entry.rbe_right;
  if (((a)->entry.rbe_right = (b)->entry.rbe_left))
    ((a)->entry.rbe_right)->entry.rbe_parent = a;
  (b)->entry.rbe_left = a;
  (b)->entry.rbe_color = (a)->entry.rbe_color;
  (a)->entry.rbe_color = 1;
  (b)->entry.rbe_parent = (a)->entry.rbe_parent;
  (a)->entry.rbe_parent = b;
  *pivot = b;
}
static inline void range_tree_LLRB_ROTR(struct node **pivot) {
  struct node *b = *pivot;
  struct node *a = (b)->entry.rbe_left;
  if (((b)->entry.rbe_left = (a)->entry.rbe_right))
    ((b)->entry.rbe_left)->entry.rbe_parent = b;
  (a)->entry.rbe_right = b;
  (a)->entry.rbe_color = (b)->entry.rbe_color;
  (b)->entry.rbe_color = 1;
  (a)->entry.rbe_parent = (b)->entry.rbe_parent;
  (b)->entry.rbe_parent = a;
  *pivot = a;
}
static inline void range_tree_LLRB_FLIP(struct node *root) {
  (root)->entry.rbe_color = !(root)->entry.rbe_color;
  ((root)->entry.rbe_left)->entry.rbe_color =
      !((root)->entry.rbe_left)->entry.rbe_color;
  ((root)->entry.rbe_right)->entry.rbe_color =
      !((root)->entry.rbe_right)->entry.rbe_color;
}
static inline void range_tree_LLRB_FIXUP(struct node **root) {
  if ((((*root)->entry.rbe_right) &&
       (((*root)->entry.rbe_right))->entry.rbe_color == 1) &&
      !(((*root)->entry.rbe_left) &&
        (((*root)->entry.rbe_left))->entry.rbe_color == 1))
    range_tree_LLRB_ROTL(root);
  if ((((*root)->entry.rbe_left) &&
       (((*root)->entry.rbe_left))->entry.rbe_color == 1) &&
      ((((*root)->entry.rbe_left)->entry.rbe_left) &&
       ((((*root)->entry.rbe_left)->entry.rbe_left))->entry.rbe_color == 1))
    range_tree_LLRB_ROTR(root);
  if ((((*root)->entry.rbe_left) &&
       (((*root)->entry.rbe_left))->entry.rbe_color == 1) &&
      (((*root)->entry.rbe_right) &&
       (((*root)->entry.rbe_right))->entry.rbe_color == 1))
    range_tree_LLRB_FLIP(*root);
}
struct node *range_tree_LLRB_INSERT(struct range_tree *head, struct node *elm) {
  struct node **root = &(head)->rbh_root;
  struct node *parent = 0;
  while (*root) {
    int comp = (intcmp)((elm), (*root));
    parent = *root;
    if (comp < 0)
      root = &(*root)->entry.rbe_left;
    else if (comp > 0)
      root = &(*root)->entry.rbe_right;
    else
      return *root;
  }
  ((elm))->entry.rbe_left = 0;
  ((elm))->entry.rbe_right = 0;
  ((elm))->entry.rbe_color = 1;
  ((elm))->entry.rbe_parent = parent;
  *root = (elm);
  while (parent && ((((parent)->entry.rbe_left) &&
                     (((parent)->entry.rbe_left))->entry.rbe_color == 1) ||
                    (((parent)->entry.rbe_right) &&
                     (((parent)->entry.rbe_right))->entry.rbe_color == 1))) {
    root = (((parent) == (head)->rbh_root) ? &(head)->rbh_root
            : ((parent) == (((parent))->entry.rbe_parent)->entry.rbe_left)
                ? &(((parent))->entry.rbe_parent)->entry.rbe_left
                : &(((parent))->entry.rbe_parent)->entry.rbe_right);
    parent = (parent)->entry.rbe_parent;
    range_tree_LLRB_FIXUP(root);
  }
  ((head)->rbh_root)->entry.rbe_color = 0;
  struct node *p = 0;
  p = elm;
  while (p) {
    char left_max[4 + 1] = "";
    char right_max[4 + 1] = "";
    if ((p)->entry.rbe_left) {
      strncpy(left_max,
              (strncmp(((p)->entry.rbe_left)->end_key,
                       ((p)->entry.rbe_left)->max, 4) > 0
                   ? ((p)->entry.rbe_left)->end_key
                   : ((p)->entry.rbe_left)->max),
              4);
    }
    if ((p)->entry.rbe_right) {
      strncpy(right_max,
              (strncmp(((p)->entry.rbe_right)->end_key,
                       ((p)->entry.rbe_right)->max, 4) > 0
                   ? ((p)->entry.rbe_right)->end_key
                   : ((p)->entry.rbe_right)->max),
              4);
    }
    char maxc[4 + 1] = "";
    char max[4 + 1] = "";
    strncpy(maxc, (strncmp(left_max, right_max, 4) > 0 ? left_max : right_max),
            4 + 1);
    strncpy(max, (strncmp(maxc, (p)->end_key, 4) > 0 ? maxc : (p)->end_key),
            4 + 1);
    strncpy((p)->max, (strncmp((p)->max, max, 4) > 0 ? (p)->max : max), 4 + 1);
    p = (p)->entry.rbe_parent;
  }
  return 0;
}
static inline void range_tree_LLRB_MOVL(struct node **pivot) {
  range_tree_LLRB_FLIP(*pivot);
  if (((((*pivot)->entry.rbe_right)->entry.rbe_left) &&
       ((((*pivot)->entry.rbe_right)->entry.rbe_left))->entry.rbe_color == 1)) {
    range_tree_LLRB_ROTR(&(*pivot)->entry.rbe_right);
    range_tree_LLRB_ROTL(pivot);
    range_tree_LLRB_FLIP(*pivot);
  }
}
static inline void range_tree_LLRB_MOVR(struct node **pivot) {
  range_tree_LLRB_FLIP(*pivot);
  if (((((*pivot)->entry.rbe_left)->entry.rbe_left) &&
       ((((*pivot)->entry.rbe_left)->entry.rbe_left))->entry.rbe_color == 1)) {
    range_tree_LLRB_ROTR(pivot);
    range_tree_LLRB_FLIP(*pivot);
  }
}
static inline struct node *range_tree_DELETEMIN(struct range_tree *head,
                                                struct node **root) {
  struct node **pivot = root, *deleted, *parent;
  while ((*pivot)->entry.rbe_left) {
    if (!(((*pivot)->entry.rbe_left) &&
          (((*pivot)->entry.rbe_left))->entry.rbe_color == 1) &&
        !((((*pivot)->entry.rbe_left)->entry.rbe_left) &&
          ((((*pivot)->entry.rbe_left)->entry.rbe_left))->entry.rbe_color == 1))
      range_tree_LLRB_MOVL(pivot);
    pivot = &(*pivot)->entry.rbe_left;
  }
  deleted = *pivot;
  parent = (*pivot)->entry.rbe_parent;
  *pivot = 0;
  while (root != pivot) {
    pivot = (((parent) == (head)->rbh_root) ? &(head)->rbh_root
             : ((parent) == (((parent))->entry.rbe_parent)->entry.rbe_left)
                 ? &(((parent))->entry.rbe_parent)->entry.rbe_left
                 : &(((parent))->entry.rbe_parent)->entry.rbe_right);
    parent = (parent)->entry.rbe_parent;
    range_tree_LLRB_FIXUP(pivot);
  }
  return deleted;
}
struct node *range_tree_LLRB_DELETE(struct range_tree *head, struct node *elm) {
  struct node **root = &(head)->rbh_root, *parent = 0, *deleted = 0;
  int comp;
  while (*root) {
    parent = (*root)->entry.rbe_parent;
    comp = (intcmp)(elm, *root);
    if (comp < 0) {
      if ((*root)->entry.rbe_left &&
          !(((*root)->entry.rbe_left) &&
            (((*root)->entry.rbe_left))->entry.rbe_color == 1) &&
          !((((*root)->entry.rbe_left)->entry.rbe_left) &&
            ((((*root)->entry.rbe_left)->entry.rbe_left))->entry.rbe_color ==
                1))
        range_tree_LLRB_MOVL(root);
      root = &(*root)->entry.rbe_left;
    } else {
      if ((((*root)->entry.rbe_left) &&
           (((*root)->entry.rbe_left))->entry.rbe_color == 1)) {
        range_tree_LLRB_ROTR(root);
        comp = (intcmp)(elm, *root);
      }
      if (!comp && !(*root)->entry.rbe_right) {
        deleted = *root;
        *root = 0;
        break;
      }
      if ((*root)->entry.rbe_right &&
          !(((*root)->entry.rbe_right) &&
            (((*root)->entry.rbe_right))->entry.rbe_color == 1) &&
          !((((*root)->entry.rbe_right)->entry.rbe_left) &&
            ((((*root)->entry.rbe_right)->entry.rbe_left))->entry.rbe_color ==
                1)) {
        range_tree_LLRB_MOVR(root);
        comp = (intcmp)(elm, *root);
      }
      if (!comp) {
        struct node *orphan =
            range_tree_DELETEMIN(head, &(*root)->entry.rbe_right);
        (orphan)->entry.rbe_color = (*root)->entry.rbe_color;
        (orphan)->entry.rbe_parent = (*root)->entry.rbe_parent;
        if (((orphan)->entry.rbe_right = (*root)->entry.rbe_right))
          ((orphan)->entry.rbe_right)->entry.rbe_parent = orphan;
        if (((orphan)->entry.rbe_left = (*root)->entry.rbe_left))
          ((orphan)->entry.rbe_left)->entry.rbe_parent = orphan;
        deleted = *root;
        *root = orphan;
        parent = *root;
        break;
      } else
        root = &(*root)->entry.rbe_right;
    }
  }
  while (parent) {
    root = (((parent) == (head)->rbh_root) ? &(head)->rbh_root
            : ((parent) == (((parent))->entry.rbe_parent)->entry.rbe_left)
                ? &(((parent))->entry.rbe_parent)->entry.rbe_left
                : &(((parent))->entry.rbe_parent)->entry.rbe_right);
    parent = (parent)->entry.rbe_parent;
    range_tree_LLRB_FIXUP(root);
  }
  if ((head)->rbh_root) ((head)->rbh_root)->entry.rbe_color = 0;
  struct node *p = 0;
  p = (deleted)->entry.rbe_parent;
  while (p) {
    char left_max[4 + 1] = "";
    char right_max[4 + 1] = "";
    if ((p)->entry.rbe_left) {
      strncpy(left_max,
              (strncmp(((p)->entry.rbe_left)->end_key,
                       ((p)->entry.rbe_left)->max, 4) > 0
                   ? ((p)->entry.rbe_left)->end_key
                   : ((p)->entry.rbe_left)->max),
              4);
    }
    if ((p)->entry.rbe_right) {
      strncpy(right_max,
              (strncmp(((p)->entry.rbe_right)->end_key,
                       ((p)->entry.rbe_right)->max, 4) > 0
                   ? ((p)->entry.rbe_right)->end_key
                   : ((p)->entry.rbe_right)->max),
              4);
    }
    char maxc[4 + 1] = "";
    char max[4 + 1] = "";
    strncpy(maxc, (strncmp(left_max, right_max, 4) > 0 ? left_max : right_max),
            4 + 1);
    strncpy(max, (strncmp(maxc, (p)->end_key, 4) > 0 ? maxc : (p)->end_key),
            4 + 1);
    if (strncmp((p)->max, max, 4) < 0) strncpy((p)->max, max, 4 + 1);
    p = (p)->entry.rbe_parent;
  }
  return deleted;
}
struct node *range_tree_LLRB_FIND(struct range_tree *head, struct node *key) {
  struct node *elm = (head)->rbh_root;
  while (elm) {
    int comp = (intcmp)(key, elm);
    if (comp < 0)
      elm = (elm)->entry.rbe_left;
    else if (comp > 0)
      elm = (elm)->entry.rbe_right;
    else
      return elm;
  }
  return 0;
}
struct node *range_tree_LLRB_MIN(struct node *elm) {
  while (elm && (elm)->entry.rbe_left) elm = (elm)->entry.rbe_left;
  return elm;
}
struct node *range_tree_LLRB_MAX(struct node *elm) {
  while (elm && (elm)->entry.rbe_right) elm = (elm)->entry.rbe_right;
  return elm;
}
struct node *range_tree_LLRB_NEXT(struct node *elm) {
  if ((elm)->entry.rbe_right) {
    return range_tree_LLRB_MIN((elm)->entry.rbe_right);
  } else if ((elm)->entry.rbe_parent) {
    if (elm == ((elm)->entry.rbe_parent)->entry.rbe_left)
      return (elm)->entry.rbe_parent;
    while ((elm)->entry.rbe_parent &&
           elm == ((elm)->entry.rbe_parent)->entry.rbe_right)
      elm = (elm)->entry.rbe_parent;
    return (elm)->entry.rbe_parent;
  } else
    return 0;
}
struct node *range_tree_LLRB_PREV(struct node *elm) {
  if ((elm)->entry.rbe_left) {
    return range_tree_LLRB_MAX((elm)->entry.rbe_left);
  } else if ((elm)->entry.rbe_parent) {
    if (elm == ((elm)->entry.rbe_parent)->entry.rbe_right)
      return (elm)->entry.rbe_parent;
    while ((elm)->entry.rbe_parent &&
           elm == ((elm)->entry.rbe_parent)->entry.rbe_left)
      elm = (elm)->entry.rbe_parent;
    return (elm)->entry.rbe_parent;
  } else
    return 0;
}
typedef void (*merger)(struct node *, struct range_group *);
void range_tree_LLRB_RANGE_MATCHER(struct node *s, struct node *elm,
                                   struct range_group *sll, merger merge) {
  if ((s)->entry.rbe_left) {
    if (strncmp((elm)->start_key, ((s)->entry.rbe_left)->max, 4) <= 0) {
      range_tree_LLRB_RANGE_MATCHER((s)->entry.rbe_left, elm, sll, merge);
    }
  }
  if ((s)->entry.rbe_right) {
    if ((strncmp((elm)->start_key, ((s)->entry.rbe_right)->max, 4) <= 0) &&
        (strncmp((elm)->end_key, ((s)->entry.rbe_right)->start_key, 4) >= 0)) {
      range_tree_LLRB_RANGE_MATCHER((s)->entry.rbe_right, elm, sll, merge);
    }
  }
  if ((strncmp((elm)->start_key, (s)->end_key, 4) <= 0) &&
      (strncmp((elm)->end_key, (s)->start_key, 4) >= 0)) {
    merge(s, sll);
  }
}
int range_tree_LLRB_RANGE_OVERLAPS(struct range_tree *head, struct node *elm) {
  if ((head)->rbh_root == 0) return 0;
  if ((strncmp((elm)->start_key, ((head)->rbh_root)->max, 4) <= 0) &&
      (strncmp((elm)->end_key,
               (range_tree_LLRB_MIN(((head))->rbh_root))->start_key, 4) >= 0)) {
    return 1;
  }
  return 0;
}
int range_tree_LLRB_RANGE_GROUP_ADD(struct range_tree *head, struct node *elm,
                                    struct range_group *sll, merger merge) {
  int no = range_tree_LLRB_RANGE_OVERLAPS(head, elm);
  if (no < 1) {
    return 1;
  }
  do {
    (sll)->slh_first = ((void *)0);
  } while (0);
  range_tree_LLRB_RANGE_MATCHER((head)->rbh_root, elm, sll, merge);
  if (!(((sll)->slh_first)->next.sle_next) &&
      ((strncmp(((sll)->slh_first)->start_key, (elm)->start_key, 4) <= 0) &&
       (strncmp(((sll)->slh_first)->end_key, (elm)->end_key, 4) >= 0))) {
    return 0;
  }
  return 1;
}

#define LLRB_INSERT(name, head, elm) name##_LLRB_INSERT((head), (elm))
#define LLRB_DELETE(name, head, elm) name##_LLRB_DELETE((head), (elm))
#define LLRB_REMOVE(name, head, elm) name##_LLRB_DELETE((head), (elm))
#define LLRB_FIND(name, head, elm) name##_LLRB_FIND((head), (elm))
#define LLRB_MIN(name, head) name##_LLRB_MIN(LLRB_ROOT((head)))
#define LLRB_MAX(name, head) name##_LLRB_MAX(LLRB_ROOT((head)))
#define LLRB_NEXT(name, head, elm) name##_LLRB_NEXT((elm))
#define LLRB_PREV(name, head, elm) name##_LLRB_PREV((elm))

#define LLRB_FOREACH(elm, name, head) \
  for ((elm) = LLRB_MIN(name, head); (elm); (elm) = name##_LLRB_NEXT((elm)))
#define LLRB_FOREACH_REVERSE(elm, name, head) \
  for ((elm) = LLRB_MAX(name, head); (elm); (elm) = name##_LLRB_PREV((elm)))

#define LLRB_RANGE_OVERLAPS(name, head, elm) \
  name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) \
  name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
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

void merge(struct node *to_merge, struct range_group *sll) {
  struct node *c, *prev, *nxt;
  nxt = SLL_FIRST(sll);
  SLL_INSERT_HEAD(sll, to_merge, next);
  if (nxt == 0) return;
  prev = SLL_FIRST(sll);
  while (nxt) {
    c = nxt;
    nxt = SLL_NEXT(nxt, next);
    if ((strncmp(to_merge->start_key, c->end_key, 4) <= 0) &&
        (strncmp(to_merge->end_key, c->start_key, 4) >= 0)) {
      /**overlap**/
      if (strncmp(to_merge->start_key, c->start_key, 4) > 0) {
        strncpy(to_merge->start_key, c->start_key, 5);
      }
      if (strncmp(to_merge->end_key, c->end_key, 4) < 0) {
        strncpy(to_merge->end_key, c->end_key, 5);
      }
      SLL_REMOVE_AFTER(prev, next);
      free(LLRB_DELETE(range_tree, &head, c));
    } else {
      prev = c;
    }
  }
}

int add_to_range(char *start, char *end) {
  struct node nn;
  strncpy(nn.start_key, start, 5);
  strncpy(nn.end_key, end, 5);
  strncpy(nn.max, "0000", 5);
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
  SLL_INIT(merge_list);
  size_t i;
  struct node *n;
  LLRB_INIT(&head);

  for (i = 0; i < sizeof(testdata) / sizeof(testdata[0]); i++) {
    if ((n = malloc(sizeof(struct node))) == NULL) err(1, NULL);
    strncpy(n->start_key, testdata[i][0], 5);
    strncpy(n->end_key, testdata[i][1], 5);
    memset(n->max, 0, 4);
    //  printf("max %s\n", n->max);
    LLRB_INSERT(range_tree, &head, n);
  }
  LLRB_FOREACH(n, range_tree, &head) { printf("%s\n", n->start_key); }
  printf("reverse\n");
  LLRB_FOREACH_REVERSE(n, range_tree, &head) { printf("%s\n", n->start_key); }
  print_tree(LLRB_ROOT(&head));
  struct node d;
  strncpy(d.start_key, "0013", 5);
  struct node *deleted;
  deleted = LLRB_DELETE(range_tree, &head, &d);
  free(deleted);
  print_tree(LLRB_ROOT(&head));
  printf("\n");
  struct node *in;
  if ((in = malloc(sizeof(struct node))) == NULL) err(1, NULL);
  strncpy(in->start_key, "0020", 5);
  strncpy(in->end_key, "0030", 5);
  strncpy(in->max, "0000", 5);
  struct node *r = LLRB_INSERT(range_tree, &head, in);
  if (r) {
    printf("insert failed for %s\n", in->start_key);
    assert(r != 0);
    free(in);
  }
  print_tree(LLRB_ROOT(&head));
  delete_tree();

  LLRB_INIT(&head);
  SLL_INIT(merge_list);

  add_to_range("0012", "0046");
  add_to_range("0001", "0002");
  add_to_range("0006", "0100");
  add_to_range("0022", "0078");
  add_to_range("0222", "0788");
  add_to_range("0300", "0800");
  add_to_range("0222", "0788");
  add_to_range("0002", "0005");
  add_to_range("0050", "0069");
  add_to_range("0030", "0089");
  // add_to_range(last);
  //
  add_to_range("0001", "0004");

  print_tree(LLRB_ROOT(&head));

  delete_tree();
  return (0);
}
