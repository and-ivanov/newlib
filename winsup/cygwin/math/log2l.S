/**
 * This file has no copyright assigned and is placed in the Public Domain.
 * This file is part of the mingw-w64 runtime package.
 * No warranty is given; refer to the file DISCLAIMER.PD within this package.
 */
#include <_mingw_mac.h>

	.file	"log2l.S"
	.text
#ifdef __x86_64__
	.align 8
#else
	.align 4
#endif
one:	.double 1.0
	/* It is not important that this constant is precise.  It is only
	   a value which is known to be on the safe side for using the
	   fyl2xp1 instruction.  */
limit:	.double 0.29

.globl __MINGW_USYMBOL(log2l)
	.def	__MINGW_USYMBOL(log2l);	.scl	2;	.type	32;	.endef
__MINGW_USYMBOL(log2l):
#ifdef __x86_64__
	fldl	one(%rip)
	fldt	(%rdx)		// x : 1
	fxam
	fnstsw
	fld	%st		// x : x : 1
	sahf
	jc	3f		// in case x is NaN or ?Inf
4:	fsub	%st(2), %st	// x-1 : x : 1
	fld	%st		// x-1 : x-1 : x : 1
	fabs			// |x-1| : x-1 : x : 1
	fcompl	limit(%rip)	// x-1 : x : 1
	fnstsw			// x-1 : x : 1
	andb	$0x45, %ah
	jz	2f
	fstp	%st(1)		// x-1 : 1
	fyl2xp1			// log(x)
	movq	%rcx,%rax
	movq	$0,8(%rcx)
	fstpt	(%rcx)
	ret

2:	fstp	%st(0)		// x : 1
	fyl2x			// log(x)
	movq	%rcx,%rax
	movq	$0,8(%rcx)
	fstpt	(%rcx)
	ret

3:	jp	4b		// in case x is ?Inf
	fstp	%st(1)
	fstp	%st(1)
	movq	%rcx,%rax
	movq	$0,8(%rcx)
	fstpt	(%rcx)
	ret
#else
	fldl	one
	fldt	4(%esp)		// x : 1
	fxam
	fnstsw
	fld	%st		// x : x : 1
	sahf
	jc	3f		// in case x is NaN or ?Inf
4:	fsub	%st(2), %st	// x-1 : x : 1
	fld	%st		// x-1 : x-1 : x : 1
	fabs			// |x-1| : x-1 : x : 1
	fcompl	limit		// x-1 : x : 1
	fnstsw			// x-1 : x : 1
	andb	$0x45, %ah
	jz	2f
	fstp	%st(1)		// x-1 : 1
	fyl2xp1			// log(x)
	ret

2:	fstp	%st(0)		// x : 1
	fyl2x			// log(x)
	ret

3:	jp	4b		// in case x is ?Inf
	fstp	%st(1)
	fstp	%st(1)
	ret
#endif
