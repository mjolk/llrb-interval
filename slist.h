/*	$OpenBSD: queue.h,v 1.45 2018/07/12 14:22:54 sashan Exp $	*/
/*	$NetBSD: queue.h,v 1.11 1996/05/16 05:17:14 mycroft Exp $	*/

/*
 * Copyright (c) 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHASLL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)queue.h	8.5 (Berkeley) 8/20/94
 */

#ifndef SLLIST
#define SLLIST

#define SLL_HEAD(name, type)		\
struct name {	struct type *slh_first; }		

#define	SLL_HEAD_INITIALIZER(head) { 0 }

#define SLL_ENTRY(type)		\
struct { struct type *sle_next; }

/*
 * Singly-linked List access methods.
 */
#define	SLL_FIRST(head) (head)->slh_first
#define	SLL_END(head)		((void*)0) 
#define	SLL_EMPTY(head)	(SLL_FIRST(head) == SLL_END(head))
#define	SLL_NEXT(elm, field)	((elm)->field.sle_next)

#define	SLL_FOREACH(var, head, field)					\
	for((var) = SLL_FIRST(head);					\
	    (var) != SLL_END(head);					\
	    (var) = SLL_NEXT(var, field))

#define	SLL_FOREACH_SAFE(var, head, field, tvar)			\
	for ((var) = SLL_FIRST(head);				\
	    (var) && ((tvar) = SLL_NEXT(var, field), 1);		\
	    (var) = (tvar))

#define SLL_INIT(head) do { SLL_FIRST(head) = 0; } while (0)

#define	SLL_INSERT_AFTER(slistelm, elm, field) do {			\
	(elm)->field.sle_next = (slistelm)->field.sle_next;		\
	(slistelm)->field.sle_next = (elm);				\
} while (0)

#define	SLL_INSERT_HEAD(head, elm, field) do {			\
	(elm)->field.sle_next = (head)->slh_first;			\
	(head)->slh_first = (elm);					\
} while (0)

#define	SLL_REMOVE_AFTER(elm, field) do {				\
	(elm)->field.sle_next = (elm)->field.sle_next->field.sle_next;	\
} while (0)

#define	SLL_REMOVE_HEAD(head, field) do {				\
	(head)->slh_first = (head)->slh_first->field.sle_next;		\
} while (0)

#define SLL_REMOVE(head, elm, type, field) do {			\
	if ((head)->slh_first == (elm)) {				\
		SLL_REMOVE_HEAD((head), field);			\
	} else {							\
		struct type *curelm = (head)->slh_first;		\
									\
		while (curelm->field.sle_next != (elm))			\
			curelm = curelm->field.sle_next;		\
		curelm->field.sle_next =				\
		    curelm->field.sle_next->field.sle_next;		\
	}								\
} while (0)

#endif
