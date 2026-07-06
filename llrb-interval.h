#ifndef LLRB_INTERVAL
#define LLRB_INTERVAL
#define LLRB_RANGE(name, type, field, head)
#define LLRB_RANGE_MAX(elm) (elm)->max_node
#define LLRB_RANGE_END(elm) (elm)->end_key
#define LLRB_RANGE_START(elm) (elm)->start_key

#define LLRB_RANGE_GROUP_GEN(name, type, field, cmp, sll_type, sll_field)      \
  static inline void type##_LLRB_AUGMENT(struct type *elm) {                   \
    struct type *p = elm;                                                      \
    while (p) {                                                                \
      struct type *max_node = p;                                               \
      if (LLRB_LEFT(p, field)) {                                               \
        if (cmp(LLRB_RANGE_END(LLRB_RANGE_MAX(LLRB_LEFT(p, field))),           \
                LLRB_RANGE_END(max_node)) > 0)                                 \
          max_node = LLRB_RANGE_MAX(LLRB_LEFT(p, field));                      \
      }                                                                        \
      if (LLRB_RIGHT(p, field)) {                                              \
        if (cmp(LLRB_RANGE_END(LLRB_RANGE_MAX(LLRB_RIGHT(p, field))),          \
                LLRB_RANGE_END(max_node)) > 0)                                 \
          max_node = LLRB_RANGE_MAX(LLRB_RIGHT(p, field));                     \
      }                                                                        \
      LLRB_RANGE_MAX(p) = max_node;                                            \
      p = LLRB_PARENT(p, field);                                               \
    }                                                                          \
  }                                                                            \
  typedef void (*merger)(struct name * head, struct type *,                    \
                         struct sll_type *);                                   \
  static inline void name##_LLRB_RANGE_MATCHER(struct name *head, struct type *s, \
                                 struct type *elm, struct sll_type *sll,       \
                                 merger merge) {                               \
    if (LLRB_LEFT(s, field)) {                                                 \
      if (cmp(LLRB_RANGE_START(elm),                                           \
              LLRB_RANGE_END(LLRB_RANGE_MAX(LLRB_LEFT(s, field)))) <= 0) {     \
        name##_LLRB_RANGE_MATCHER(head, LLRB_LEFT(s, field), elm, sll, merge); \
      }                                                                        \
    }                                                                          \
    if (LLRB_RIGHT(s, field)) {                                                \
      if ((cmp(LLRB_RANGE_START(elm),                                          \
               LLRB_RANGE_END(LLRB_RANGE_MAX(LLRB_RIGHT(s, field)))) <= 0) &&  \
          (cmp(LLRB_RANGE_END(elm),                                            \
               LLRB_RANGE_START(LLRB_RIGHT(s, field))) >= 0)) {                \
        name##_LLRB_RANGE_MATCHER(head, LLRB_RIGHT(s, field), elm, sll,        \
                                  merge);                                      \
      }                                                                        \
    }                                                                          \
    if ((cmp(LLRB_RANGE_START(elm), LLRB_RANGE_END(s)) <= 0) &&                \
        (cmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(s)) >= 0)) {                \
      merge(head, s, sll);                                                     \
    }                                                                          \
  }                                                                            \
  static inline int name##_LLRB_RANGE_OVERLAPS(struct name *head, struct type *elm) { \
    if (LLRB_ROOT(head) == 0) return 0;                                        \
    if ((cmp(LLRB_RANGE_START(elm),                                            \
             LLRB_RANGE_END(LLRB_RANGE_MAX(LLRB_ROOT(head)))) <= 0) &&         \
        (cmp(LLRB_RANGE_END(elm),                                              \
             LLRB_RANGE_START(LLRB_MIN(name, head))) >= 0)) {                  \
      return 1;                                                                \
    }                                                                          \
    return 0;                                                                  \
  }                                                                            \
  static inline int name##_LLRB_RANGE_GROUP_ADD(struct name *head, struct type *elm, \
                                  struct sll_type *sll, merger merge) {        \
    if (name##_LLRB_RANGE_OVERLAPS(head, elm) == 0) return 1;                  \
    SLIST_INIT(sll);                                                           \
    name##_LLRB_RANGE_MATCHER(head, LLRB_ROOT(head), elm, sll, merge);         \
    if (!SLIST_NEXT(SLIST_FIRST(sll), next) &&                                 \
        ((cmp(LLRB_RANGE_START(SLIST_FIRST(sll)), LLRB_RANGE_START(elm)) <=    \
          0) &&                                                                \
         (cmp(LLRB_RANGE_END(SLIST_FIRST(sll)), LLRB_RANGE_END(elm)) >=        \
          0))) {                                                               \
      return 0;                                                                \
    }                                                                          \
    return 1;                                                                  \
  }                                                                            \
  static inline void name##_LLRB_RANGE_GROUP_FIND(struct name *head, struct type *elm, \
                                    struct sll_type *sll, merger merge) {      \
    if (name##_LLRB_RANGE_OVERLAPS(head, elm) == 0) return;                    \
    SLIST_INIT(sll);                                                           \
    name##_LLRB_RANGE_MATCHER(head, LLRB_ROOT(head), elm, sll, merge);         \
  }

#ifdef LLRB_AUGMENT
#undef LLRB_AUGMENT
#endif
#define LLRB_AUGMENT(elm, field, type) type##_LLRB_AUGMENT(elm);

#define LLRB_RANGE_OVERLAPS(name, head, elm) \
  name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) \
  name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
#define LLRB_RANGE_GROUP_FIND(name, head, elm, sll, merge) \
  name##_LLRB_RANGE_GROUP_FIND((head), (elm), (sll), (merge))
#endif

