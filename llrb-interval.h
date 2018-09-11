#ifndef LLRB_INTERVAL
#define LLRB_INTERVAL
#define LLRB_RANGE_MAX(elm) (elm)->max
#define LLRB_RANGE_END(elm) (elm)->end
#define LLRB_RANGE_START(elm) (elm)->start
#define LLRB_PARENT_MAX(elm, field) \
	int left_max = 0; \
	int right_max = 0; \
	if(LLRB_LEFT(elm, field)){ left_max = (LLRB_RANGE_END(LLRB_LEFT(elm, field)) < LLRB_RANGE_MAX(LLRB_LEFT(elm, field))?LLRB_RANGE_MAX(LLRB_LEFT(elm, field)):LLRB_RANGE_END(LLRB_LEFT(elm, field)));} \
	if(LLRB_RIGHT(elm, field)){ right_max = (LLRB_RANGE_END(LLRB_RIGHT(elm, field)) < LLRB_RANGE_MAX(LLRB_RIGHT(elm, field))?LLRB_RANGE_MAX(LLRB_RIGHT(elm, field)):LLRB_RANGE_END(LLRB_RIGHT(elm, field)));} \
	int maxc = (left_max >= right_max)?left_max:right_max; \
	int max = (maxc > LLRB_RANGE_END(elm))?maxc:LLRB_RANGE_END(elm); \
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
	if(LLRB_RANGE_START(elm) < LLRB_RANGE_END(s) && LLRB_RANGE_END(elm) > LLRB_RANGE_START(s)) { \
		merge(s, sll); \
	} \
	if(LLRB_LEFT(s, field)){ \
		if(LLRB_RANGE_START(elm) < LLRB_RANGE_MAX(LLRB_LEFT(s, field))) { \
			name##_LLRB_RANGE_MATCHER(LLRB_LEFT(s, field), elm, sll, merge); \
		} \
	} \
	if(LLRB_RIGHT(s, field)) { \
		if(LLRB_RANGE_START(elm) < LLRB_RANGE_MAX(LLRB_RIGHT(s, field)) && LLRB_RANGE_END(elm) > LLRB_RANGE_START(LLRB_RIGHT(s, field))) { \
			name##_LLRB_RANGE_MATCHER(LLRB_RIGHT(s, field), elm, sll, merge); \
		} \
	} \
} \
int name##_LLRB_RANGE_OVERLAPS(struct name *head, struct type *elm) { \
	int end = LLRB_RANGE_MAX(LLRB_ROOT(head)); \
	int start = LLRB_RANGE_START(LLRB_MIN(name, head)); \
	if(LLRB_RANGE_START(elm) < end && LLRB_RANGE_END(elm) > start) { \
		return 1; \
	} \
	return 0; \
} \
int name##_LLRB_RANGE_GROUP_ADD(struct name *head, struct type *elm, struct sll_type *sll, merger merge) { \
	int no = name##_LLRB_RANGE_OVERLAPS(head, elm); \
	if(no < 1){ \
		name##_LLRB_INSERT(head, elm); \
		return 1; \
	} \
	name##_LLRB_RANGE_MATCHER(LLRB_ROOT(head), elm, sll, merge); \
	if((!(sll)->slh_first->sll_field.sle_next) && ((sll)->slh_first->start <= elm->start) && ((sll)->slh_first->end >= elm->end)) { \
		return 0; \
	} \
	name##_LLRB_INSERT(head, elm); \
	return 1; \
}

#define LLRB_RANGE_OVERLAPS(name, head, elm) name##_LLRB_RANGE_OVERLAPS((head), (elm))
#define LLRB_RANGE_GROUP_ADD(name, head, elm, sll, merge) name##_LLRB_RANGE_GROUP_ADD((head), (elm), (sll), (merge))
#endif
