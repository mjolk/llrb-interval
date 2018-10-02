/**
 * File   : llrb-interval.h
 * License: MIT/X11
 * Author : Dries Pauwels <2mjolk@gmail.com>
 * Date   : di 11 sep 2018 07:55
 */
#ifndef LLRB_INTERVAL
#define LLRB_INTERVAL
#define LLRB_RANGE_MAX(elm) (elm)->max
#define LLRB_RANGE_END(elm) (elm)->end_key
#define LLRB_RANGE_START(elm) (elm)->start_key
#define LLRB_PARENT_MAX(elm, field) \
    uint64_t left_max = 0; \
    uint64_t right_max = 0; \
    if(LLRB_LEFT(elm, field)){ left_max = (LLRB_RANGE_END(LLRB_LEFT(elm, field)) < LLRB_RANGE_MAX(LLRB_LEFT(elm, field))?LLRB_RANGE_MAX(LLRB_LEFT(elm, field)):LLRB_RANGE_END(LLRB_LEFT(elm, field)));} \
    if(LLRB_RIGHT(elm, field)){ right_max = (LLRB_RANGE_END(LLRB_RIGHT(elm, field)) < LLRB_RANGE_MAX(LLRB_RIGHT(elm, field))?LLRB_RANGE_MAX(LLRB_RIGHT(elm, field)):LLRB_RANGE_END(LLRB_RIGHT(elm, field)));} \
    uint64_t maxc = (left_max >= right_max)?left_max:right_max; \
    uint64_t max = (maxc > LLRB_RANGE_END(elm))?maxc:LLRB_RANGE_END(elm); \
    LLRB_RANGE_MAX(elm) = (LLRB_RANGE_MAX(elm) <  max)?max:LLRB_RANGE_MAX(elm);
#define LLRB_AUGMENT(elm, field, type) \
    struct type *p = 0; \
    p = elm; \
    while(p){ \
        LLRB_RANGE_MAX(p) = 0; \
        LLRB_PARENT_MAX(p, field) \
        p = LLRB_PARENT(p, field); \
    }
#define LLRB_RANGE_GROUP_GEN(name, type, field, sll_type, sll_field) \
    typedef void (*merger)(struct type*, struct sll_type*); \
    void name##_LLRB_RANGE_MATCHER(struct type *s, struct type *elm, struct sll_type *sll, merger merge) { \
        printf("inside range matcher start: %lu end: %lu\n", s->start_key, s->end_key); \
        printf("inside range matcher start matching: %lu end: %lu\n", elm->start_key, elm->end_key); \
        if(LLRB_RANGE_START(elm) <= LLRB_RANGE_END(s) && LLRB_RANGE_END(elm) >= LLRB_RANGE_START(s)) { \
        printf("overlap -->> merge\n"); \
            merge(s, sll); \
        } \
        printf("continue left/right\n"); \
        if(LLRB_LEFT(s, field)){ \
            printf("continue left elm start: %lu, left max: %lu\n", LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_LEFT(s, field))); \
            if(LLRB_RANGE_START(elm) <= LLRB_RANGE_MAX(LLRB_LEFT(s, field))) { \
            printf("continue left\n"); \
                name##_LLRB_RANGE_MATCHER(LLRB_LEFT(s, field), elm, sll, merge); \
            } \
        } \
        if(LLRB_RIGHT(s, field)) { \
            printf("continue right inspecting start: %lu, max: %lu\n", LLRB_RANGE_START(LLRB_RIGHT(s, field)),LLRB_RANGE_MAX(LLRB_RIGHT(s, field))); \
            printf("continue right subject start: %lu, max: %lu\n", LLRB_RANGE_START(elm),LLRB_RANGE_END(elm)); \
            if(LLRB_RANGE_START(elm) <= LLRB_RANGE_MAX(LLRB_RIGHT(s, field)) && LLRB_RANGE_END(elm) >= LLRB_RANGE_START(LLRB_RIGHT(s, field))) { \
                printf("continue right\n"); \
                name##_LLRB_RANGE_MATCHER(LLRB_RIGHT(s, field), elm, sll, merge); \
            } \
        } \
    } \
    int name##_LLRB_RANGE_OVERLAPS(struct name *head, struct type *elm) { \
        if(LLRB_ROOT(head) == 0) return 0; \
        uint64_t end = LLRB_RANGE_MAX(LLRB_ROOT(head)); \
        uint64_t start = LLRB_RANGE_START(LLRB_MIN(name, head)); \
        if(LLRB_RANGE_START(elm) <= end && LLRB_RANGE_END(elm) >= start) { \
            return 1; \
        } \
        return 0; \
    } \
    int name##_LLRB_RANGE_GROUP_ADD(struct name *head, struct type *elm, struct sll_type *sll, merger merge) { \
        printf("range group add\n"); \
        int no = name##_LLRB_RANGE_OVERLAPS(head, elm); \
        if(no < 1){ \
            printf("doesn;t overlap.."); \
            struct type *nr = malloc(sizeof(struct type)); \
            if(nr == 0) return 0; \
            *nr = *elm; \
            name##_LLRB_INSERT(head, nr); \
            return 1; \
        } \
        name##_LLRB_RANGE_MATCHER(LLRB_ROOT(head), elm, sll, merge); \
        if(!SLL_NEXT(SLL_FIRST(sll), next) && (LLRB_RANGE_START(SLL_FIRST(sll)) <= LLRB_RANGE_START(elm) && (LLRB_RANGE_END(SLL_FIRST(sll)) >= LLRB_RANGE_END(elm)))) { \
            return 0; \
        } \
        struct type *nr = malloc(sizeof(struct type)); \
        if(nr == 0) return 0; \
        *nr = *elm; \
        name##_LLRB_INSERT(head, nr); \
        return 1; \
    }

#define LLRB_RANGE_OVERLAPS(name, head, elm) name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
#endif
