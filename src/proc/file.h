/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Files
 *
 * Copyright 2017 Phoenix Systems
 * Author: Pawel Pisarczyk, Aleksander Kaminski
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _PROC_FILE_H_
#define _PROC_FILE_H_

#include HAL
#include "threads.h"


extern int proc_fileAdd(unsigned int *h, oid_t *oid);


extern int proc_fileSet(unsigned int h, char flags, oid_t *oid, offs_t offs);


extern int proc_fileGet(unsigned int h, char flags, oid_t *oid, offs_t *offs);


extern int proc_fileRemove(unsigned int h);


#endif
