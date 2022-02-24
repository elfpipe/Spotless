#include <stdio.h>
#include <proto/exec.h>

#include <stdint.h>

asm (
"	.globl meth_start	\n"
"	.globl meth_end		\n"
"meth_start:			\n"
"	trap				\n"
"meth_end:				\n"
);

// asm (
// "	.globl setbreak		\n"
// "	.set MSR_DR,0x0010	\n"
// "	.set MSR_EE,0x8000	\n"
// "setbreak:				\n"

// "	mr    %r5, %r3		\n"

// "	mfmsr %r11			\n"
// "	ori   %r12, %r11, MSR_DR|MSR_EE	\n" //         # Turn off address  translation
// "	xori  %r12, %r12, MSR_DR|MSR_EE	\n" //         # and interrupts
// "	sync				\n"
// "	mtmsr %r12			\n"
// "	isync				\n"

// //# do modification here, leave r11 intact
// "	lwz   %r3, 0(%r5)		\n"
// "	stw   %r4, 0(%r5)		\n"

// "	sync				\n"
// "	icbi 0, %r5			\n"
// "	isync				\n"

// "	sync           		\n"             //          # make sure store is complete
// "	mtmsr         %r11	\n"
// "	isync				\n"

// "	blr					\n"
// );

extern unsigned int meth_start, meth_end;
extern /*"C"*/ uint32_t setbreak(uint32_t, uint32_t); //uint32_t, uint32_t);

struct MMUIFace *IMMU;

int main()
{
    printf("Traptest.\n");
    IMMU = (struct MMUIFace *)IExec->GetInterface ((struct Library *)SysBase, "mmu", 1, 0);
	if (!IMMU) {
		return 0;
	}

    uint32_t *address = (uint32_t *)&main;

    APTR stack = IExec->SuperState();

	/* Make sure to unprotect the memory area */
	uint32 oldAttr = IMMU->GetMemoryAttrs ((APTR)address, 0);
	IMMU->SetMemoryAttrs ((APTR)address, 4, MEMATTRF_READ_WRITE);

    // Don't do this :
	// uint32_t realAddress = (uint32_t)IMMU->GetPhysicalAddress ((APTR)address);
	// if (realAddress == 0x0)
	// 	realAddress = address;

	// /* *buffer = */setbreak (realAddress, meth_start);

    // *** Install the breakpoint :
    uint32_t buffer = *address;
    *address = meth_start;

    // *** Do the cache flush to memory :
    IExec->CacheClearE((APTR)address, 32, CACRF_ClearI | CACRF_ClearD );

	/* Set old attributes again */
	IMMU->SetMemoryAttrs ((APTR)address, 4, oldAttr);

    // *** Do the debugging here ---> |

	/* Make sure to unprotect the memory area */
	oldAttr = IMMU->GetMemoryAttrs ((APTR)address, 0);
	IMMU->SetMemoryAttrs ((APTR)address, 4, MEMATTRF_READ_WRITE);

    // *** Wipe the trap instruction from code :
    *address = buffer;

    // *** Do the cache flush to memory :
    IExec->CacheClearE((APTR)address, 32, CACRF_ClearI | CACRF_ClearD );

	/* Set old attributes again */
	IMMU->SetMemoryAttrs ((APTR)address, 4, oldAttr);

	/* Return to old state */
	if (stack) IExec->UserState (stack);

    // *** Done.

	if (IMMU)
		IExec->DropInterface((struct Interface *)IMMU);
	IMMU = 0;

    return 0;
}