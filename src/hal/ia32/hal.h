/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * Hardware Abstraction Layer (IA32 PC)
 *
 * Copyright 2012, 2017 Phoenix Systems
 * Copyright 2006 Pawel Pisarczyk
 * Author: Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#ifndef _HAL_HAL_H_

#ifndef __ASSEMBLY__


#include "cpu.h"
#include "string.h"
#include "syspage.h"
#include "console.h"
#include "pmap.h"
#include "spinlock.h"
#include "interrupts.h"
#include "exceptions.h"


extern int hal_started(void);


extern void _hal_start(void);


extern void _hal_init(void);


#endif

#endif
