/*
 * Phoenix-RTOS
 *
 * Operating system kernel
 *
 * CPU related routines
 *
 * Copyright 2014, 2017, 2018 Phoenix Systems
 * Author: Jacek Popko, Aleksander Kaminski, Pawel Pisarczyk
 *
 * This file is part of Phoenix-RTOS.
 *
 * %LICENSE%
 */

#include "cpu.h"
#include "pmap.h"
#include "string.h"
#include "spinlock.h"

#include "../../../include/errno.h"


/* Function creates new cpu context on top of given thread kernel stack */
int hal_cpuCreateContext(cpu_context_t **nctx, void *start, void *kstack, size_t kstacksz, void *ustack, void *arg)
{
	cpu_context_t *ctx;
	int i;

	*nctx = 0;
	if (kstack == NULL)
		return -EINVAL;
	
	if (kstacksz < sizeof(cpu_context_t))
		return -EINVAL;

	kstacksz &= ~0x3;
	
	/* Prepare initial kernel stack */
	ctx = (cpu_context_t *)(kstack + kstacksz - sizeof(cpu_context_t));

	/* Set all registers to sNAN */
	for (i = 0; i < 64; i += 2) {
		ctx->freg[i] = 0;
		ctx->freg[i + 1] = 0xfff10000;
	}

	ctx->fpsr = 0;

	ctx->padding = 0;

	ctx->r0	= (u32)arg;
	ctx->r1 = 0x11111111;
	ctx->r2 = 0x22222222;
	ctx->r3 = 0x33333333;
	ctx->r4 = 0x44444444;
	ctx->r5 = 0x55555555;
	ctx->r6 = 0x66666666;
	ctx->r7 = 0x77777777;
	ctx->r8 = 0x88888888;
	ctx->r9 = 0x99999999;
	ctx->r10 = 0xaaaaaaaa;

	ctx->ip = 0xcccccccc;
	ctx->lr = 0xeeeeeeee;

	ctx->pc = (u32)start;
	ctx->r0	= (u32)arg;
	
	/* Enable interrupts, set normal execution mode */
	if (ustack != NULL) {
		ctx->psr = USR_MODE;
		ctx->sp	 = (u32)ustack;
	}
	else {
		ctx->psr = SYS_MODE;
		ctx->sp	 = (u32)kstack + kstacksz;
	}

	/* Thumb mode? */
	if (ctx->pc & 1)
		ctx->psr |= 1 << 5;

	ctx->fp	 = ctx->sp;
	*nctx = ctx;
	
	return 0;
}


#if 0
int hal_cpuGetFault(const hal_cpuFault_t type, void **fault_addr)
{
	int fault_src;
	
	switch (type) {
		case FAULT_INST:
			/* Read IFAR */
			__asm__ volatile ("MRC p15, 0, %0, c6, c0, 2" : "=r" (*fault_addr));
			/* Read IFSR */
			__asm__ volatile ("MRC p15, 0, %0, c5, c0, 1" : "=r" (fault_src));
			break;

		case FAULT_DATA:
			/* Read DFAR */
			__asm__ volatile ("MRC p15, 0, %0, c6, c0, 0" : "=r" (*fault_addr));
			/* Read DFSR */
			__asm__ volatile ("MRC p15, 0, %0, c5, c0, 0" : "=r" (fault_src));
			break;

		case FAULT_AUX:
			/* Read DFAR */
			__asm__ volatile ("MRC p15, 0, %0, c6, c0, 0" : "=r" (*fault_addr));
			/* Read DFSR */
			__asm__ volatile ("MRC p15, 0, %0, c5, c0, 0" : "=r" (fault_src));
			return fault_src;

		default:
			return -EINVAL;
	}
	return (fault_src & 0xf) | ((fault_src >> 6) & 0x10);
}
#endif

char *hal_cpuInfo(char *info)
{
	size_t n = 0;
	u32 midr;

#ifdef CPU_IMX
	hal_strcpy(&info[n], "i.MX 6ULL ");
	n += 10;
#elif CPU_IMX6UL
	hal_strcpy(&info[n], "i.MX 6UL ");
	n += 9;
#else
	hal_strcpy(&info[n], "unknown ");
	n += 8;
#endif

	midr = hal_cpuGetMIDR();

	if (((midr >> 16) & 0xf) == 0xf) {
		hal_strcpy(&info[n], "ARMv7 ");
		n += 6;
	}

	if (((midr >> 4) & 0xfff) == 0xc07) {
		hal_strcpy(&info[n], "Cortex-A7 ");
		n += 10;
	}

	info[n++] = 'r';
	info[n++] = '0' + ((midr >> 20) & 0xf);
	info[n++] = 'p';
	info[n++] = '0' + (midr & 0xf);

	info[n] = '\0';

	return info;
}


char *hal_cpuFeatures(char *features, unsigned int len)
{
	unsigned int n = 0;
	u32 pfr0 = hal_cpuGetPFR0(), pfr1 = hal_cpuGetPFR1();

	if (!len)
		return features;

	if ((pfr0 >> 12) & 0xf && len - n > 8) {
		hal_strcpy(&features[n], "ThumbEE,");
		n += 8;
	}

	if ((pfr0 >> 8) & 0xf && len - n > 8) {
		hal_strcpy(&features[n], "Jazelle,");
		n += 8;
	}

	if ((pfr0 >> 4) & 0xf && len - n > 6) {
		hal_strcpy(&features[n], "Thumb,");
		n += 6;
	}

	if (pfr0 & 0xf && len - n > 4) {
		hal_strcpy(&features[n], "ARM,");
		n += 4;
	}

	if ((pfr1 >> 16) & 0xf && len - n > 14) {
		hal_strcpy(&features[n], "Generic Timer,");
		n += 14;
	}

	if ((pfr1 >> 12) & 0xf && len - n > 15) {
		hal_strcpy(&features[n], "Virtualization,");
		n += 15;
	}

	if ((pfr1 >> 8) & 0xf && len - n > 4) {
		hal_strcpy(&features[n], "MCU,");
		n += 4;
	}

	if ((pfr1 >> 4) & 0xf && len - n > 9) {
		hal_strcpy(&features[n], "Security,");
		n += 9;
	}

	if (n > 0)
		features[n - 1] = '\0';
	else
		features[0] = '\0';

	return features;
}


void _hal_cpuInit(void)
{
//	_hal_cpuInitCores();
}
