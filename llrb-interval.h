/**
 * File   : llrb-interval.h
 * License: MIT/X11
 * Author : Dries Pauwels <2mjolk@gmail.com>
 * Date   : di 11 sep 2018 07:55
 */
#ifndef LLRB_INTERVAL
#define LLRB_INTERVAL
#define LLRB_RANGE(name, type, field, head)
#define LLRB_RANGE_MAX(elm) (elm)->max
#define LLRB_RANGE_END(elm) (elm)->end_key
#define LLRB_RANGE_START(elm) (elm)->start_key
#define LLRB_PARENT_MAX(elm, field) \
    char left_max[KEY_SIZE+1] = ""; \
    char right_max[KEY_SIZE+1] = ""; \
    if(LLRB_LEFT(elm, field)){ strcpy(left_max, (strcmp(LLRB_RANGE_END(LLRB_LEFT(elm, field)), LLRB_RANGE_MAX(LLRB_LEFT(elm, field)))>0?LLRB_RANGE_END(LLRB_LEFT(elm, field)):LLRB_RANGE_MAX(LLRB_LEFT(elm, field))));} \
    if(LLRB_RIGHT(elm, field)){ strcpy(right_max, (strcmp(LLRB_RANGE_END(LLRB_RIGHT(elm, field)), LLRB_RANGE_MAX(LLRB_RIGHT(elm, field)))>0?LLRB_RANGE_END(LLRB_RIGHT(elm, field)):LLRB_RANGE_MAX(LLRB_RIGHT(elm, field))));} \
    char maxc[KEY_SIZE+1] = ""; \
    char max[KEY_SIZE+1] = ""; \
    strcpy(maxc, (strcmp(left_max, right_max) > 0?left_max:right_max)); \
    strcpy(max,  (strcmp(maxc , LLRB_RANGE_END(elm)) > 0?maxc:LLRB_RANGE_END(elm))); \
    if(strcmp(LLRB_RANGE_MAX(elm),  max)<0) strcpy(LLRB_RANGE_MAX(elm), max);
#ifdef LLRB_AUGMENT
#undef LLRB_AUGMENT
#endif
#define LLRB_AUGMENT(elm, field, type) \
    struct type *p = 0; \
    p = elm; \
    while(p){ \
        LLRB_PARENT_MAX(p, field) \
        p = LLRB_PARENT(p, field); \
    }
#define LLRB_RANGE_GROUP_GEN(name, type, field, sll_type, sll_field) \
    typedef void (*merger)(struct type*, struct sll_type*); \
    void name##_LLRB_RANGE_MATCHER(struct type *s, struct type *elm, struct sll_type *sll, merger merge) { \
        if(LLRB_LEFT(s, field)){ \
            if(strcmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_LEFT(s, field))) <= 0) { \
                name##_LLRB_RANGE_MATCHER(LLRB_LEFT(s, field), elm, sll, merge); \
            } \
        } \
        if(LLRB_RIGHT(s, field)) { \
            if((strcmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_RIGHT(s, field))) <= 0) && (strcmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(LLRB_RIGHT(s, field))) >= 0)) { \
                name##_LLRB_RANGE_MATCHER(LLRB_RIGHT(s, field), elm, sll, merge); \
            } \
        } \
        if((strcmp(LLRB_RANGE_START(elm), LLRB_RANGE_END(s)) <= 0) && (strcmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(s)) >= 0)) { \
            merge(s, sll); \
        } \
    } \
    int name##_LLRB_RANGE_OVERLAPS(struct name *head, struct type *elm) { \
        if(LLRB_ROOT(head) == 0) return 0; \
        if((strcmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_ROOT(head))) <= 0) && (strcmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(LLRB_MIN(name, head))) >= 0)) { \
            return 1; \
        } \
        return 0; \
    } \
    int name##_LLRB_RANGE_GROUP_ADD(struct name *head, struct type *elm, struct sll_type *sll, merger merge) { \
        if(name##_LLRB_RANGE_OVERLAPS(head, elm) == 0) return 1; \
        SLL_INIT(sll); \
        name##_LLRB_RANGE_MATCHER(LLRB_ROOT(head), elm, sll, merge); \
        if(!SLL_NEXT(SLL_FIRST(sll), next) && ((strcmp(LLRB_RANGE_START(SLL_FIRST(sll)), LLRB_RANGE_START(elm)) <= 0) && (strcmp(LLRB_RANGE_END(SLL_FIRST(sll)), LLRB_RANGE_END(elm)) >= 0))) { \
            return 0; \
        } \
        return 1; \
    }

#define LLRB_RANGE_OVERLAPS(name, head, elm) name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
#endif
