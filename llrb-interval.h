/**
 * File   : llrb-interval.h
 * License: MIT/X11
 * Author : Dries Pauwels <2mjolk@gmail.com>
 * Date   : di 11 sep 2018 07:55
 */
#ifndef KEY_SIZE
#define KEY_SIZE 4
#endif
#ifndef LLRB_INTERVAL
#define LLRB_INTERVAL
#define LLRB_RANGE(name, type, field, head)
#define LLRB_RANGE_MAX(elm) (elm)->max
#define LLRB_RANGE_END(elm) (elm)->end_key
#define LLRB_RANGE_START(elm) (elm)->start_key
#define LLRB_PARENT_MAX(elm, field) \
    char left_max[KEY_SIZE+1] = ""; \
    char right_max[KEY_SIZE+1] = ""; \
    if(LLRB_LEFT(elm, field)){ strncpy(left_max, (strncmp(LLRB_RANGE_END(LLRB_LEFT(elm, field)), LLRB_RANGE_MAX(LLRB_LEFT(elm, field)), KEY_SIZE)>0?LLRB_RANGE_END(LLRB_LEFT(elm, field)):LLRB_RANGE_MAX(LLRB_LEFT(elm, field))), 4);} \
    if(LLRB_RIGHT(elm, field)){ strncpy(right_max, (strncmp(LLRB_RANGE_END(LLRB_RIGHT(elm, field)), LLRB_RANGE_MAX(LLRB_RIGHT(elm, field)), KEY_SIZE)>0?LLRB_RANGE_END(LLRB_RIGHT(elm, field)):LLRB_RANGE_MAX(LLRB_RIGHT(elm, field))), 4);} \
    char maxc[KEY_SIZE+1] = ""; \
    char max[KEY_SIZE+1] = ""; \
    strncpy(maxc, (strncmp(left_max, right_max, KEY_SIZE) > 0?left_max:right_max), KEY_SIZE+1); \
    strncpy(max,  (strncmp(maxc , LLRB_RANGE_END(elm), KEY_SIZE) > 0?maxc:LLRB_RANGE_END(elm)), KEY_SIZE+1); \
    if(strncmp(LLRB_RANGE_MAX(elm),  max, KEY_SIZE)<0) strncpy(LLRB_RANGE_MAX(elm), max, KEY_SIZE+1);
#ifdef LLRB_AUGMENT
#undef LLRB_AUGMENT
#endif
#define LLRB_AUGMENT(elm, field, type) \
    struct type *p = 0; \
    p = elm; \
    while(p){ \
        /**strncpy(LLRB_RANGE_MAX(p), "0000", 5);**/ \
        LLRB_PARENT_MAX(p, field) \
        p = LLRB_PARENT(p, field); \
    }
#define LLRB_RANGE_GROUP_GEN(name, type, field, sll_type, sll_field) \
    typedef void (*merger)(struct type*, struct sll_type*); \
    void name##_LLRB_RANGE_MATCHER(struct type *s, struct type *elm, struct sll_type *sll, merger merge) { \
        if(LLRB_LEFT(s, field)){ \
            if(strncmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_LEFT(s, field)), KEY_SIZE) <= 0) { \
                name##_LLRB_RANGE_MATCHER(LLRB_LEFT(s, field), elm, sll, merge); \
            } \
        } \
        if(LLRB_RIGHT(s, field)) { \
            if((strncmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_RIGHT(s, field)), KEY_SIZE) <= 0) && (strncmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(LLRB_RIGHT(s, field)), 4) >= 0)) { \
                name##_LLRB_RANGE_MATCHER(LLRB_RIGHT(s, field), elm, sll, merge); \
            } \
        } \
        if((strncmp(LLRB_RANGE_START(elm), LLRB_RANGE_END(s), KEY_SIZE) <= 0) && (strncmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(s), KEY_SIZE) >= 0)) { \
            merge(s, sll); \
        } \
    } \
    int name##_LLRB_RANGE_OVERLAPS(struct name *head, struct type *elm) { \
        if(LLRB_ROOT(head) == 0) return 0; \
        if((strncmp(LLRB_RANGE_START(elm), LLRB_RANGE_MAX(LLRB_ROOT(head)), KEY_SIZE) <= 0) && (strncmp(LLRB_RANGE_END(elm), LLRB_RANGE_START(LLRB_MIN(name, head)), 4) >= 0)) { \
            return 1; \
        } \
        return 0; \
    } \
    int name##_LLRB_RANGE_GROUP_ADD(struct name *head, struct type *elm, struct sll_type *sll, merger merge) { \
        int no = name##_LLRB_RANGE_OVERLAPS(head, elm); \
        if(no < 1){ \
            return 1; \
        } \
        SLL_INIT(sll); \
        name##_LLRB_RANGE_MATCHER(LLRB_ROOT(head), elm, sll, merge); \
        if(!SLL_NEXT(SLL_FIRST(sll), next) && ((strncmp(LLRB_RANGE_START(SLL_FIRST(sll)), LLRB_RANGE_START(elm), KEY_SIZE) <= 0) && (strncmp(LLRB_RANGE_END(SLL_FIRST(sll)), LLRB_RANGE_END(elm), KEY_SIZE) >= 0))) { \
            return 0; \
        } \
        return 1; \
    }

#define LLRB_RANGE_OVERLAPS(name, head, elm) name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
#endif
