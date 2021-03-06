/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Process resources
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _PROC_RESOURCE_H_
#define _PROC_RESOURCE_H_

#include "cond.h"


typedef struct _resource_t {
	rbnode_t linkage;

	unsigned int id;
	unsigned int refs;

	unsigned int lmaxgap;
	unsigned int rmaxgap;

	enum { rtLock = 0, rtCond, rtFile } type;

	union {
		lock_t lock;

		thread_t *waitq;

		/* file descriptor */
		struct {
			oid_t oid;
			offs_t offs;
		};
	};
} resource_t;


extern resource_t *resource_alloc(process_t *process, unsigned int *id);


extern int resource_free(resource_t *r);


extern void proc_resourcesFree(process_t *proc);


extern int proc_resourcesCopy(process_t *src);


extern resource_t *resource_get(process_t *process, unsigned int id);


extern void resource_put(process_t *process, resource_t *r);


extern int proc_resourceFree(unsigned int h);


extern void resource_init(process_t *process);


#endif
