//
//
// breakmarkshandler.cpp - handle break markings in running code (Debug 101)
//
#include <proto/exec.h>
#include <proto/dos.h>
#include <string>

#include "Breaks.hpp"
#include "LowLevel.hpp"

#include <iostream>
using namespace std;
#include <unistd.h>

extern struct MMUIFace *IMMU;

bool Breaks::isBreak(uint32_t address)
{
	for (list <Break *>::iterator it = breaks.begin (); it != breaks.end (); it++)
		if (address == (*it)->address)
			return true;
	return false;
}

void Breaks::activate()
{
	if(activated) return;
	for (list <Break *>::iterator it = breaks.begin(); it != breaks.end(); it++)
		memory_insert_break_instruction((*it)->address, &(*it)->buffer);
	
	activated = true;
}

void Breaks::deactivate()
{
	if(!activated) return;
	for (list<Break *>::iterator it = breaks.begin(); it != breaks.end(); it++)
		memory_remove_break_instruction((*it)->address, &(*it)->buffer);
	
	activated = false;
}

bool Breaks::insert(uint32_t address)
{
	if (isBreak(address)) {
		cout << "Failed to set break. address = " << (void *)address << "\n";
		return false;
	}
	// if(!is_writable_address(address)) return false;

	breaks.push_back(new Break(address));
	return true;
}

void Breaks::remove(uint32_t address)
{
	for (list<Break *>::iterator it = breaks.begin(); it != breaks.end(); it++)
		if (address == (*it)->address) {
			delete *it;
			it = breaks.erase(it);
		}
}

void Breaks::clear()
{
	deactivate();
	while(!breaks.empty()) delete breaks.front(), breaks.pop_front();

	// for (list<Break *>::iterator it = breaks.begin(); it != breaks.end(); it++) {
	// 	cout << "delete break : " << (void *)(*it)->address << "\n";
	// 	delete *it;
	// }
	// breaks.clear();
}

// ------------------------------------------------------------------- //

#ifdef __amigaos4__
////

// Look out - there is a trap !

// /

#define BIG_ENDIAN

asm (
"	.globl meth_start	\n"
"	.globl meth_end		\n"
"meth_start:			\n"
"	trap				\n"
"meth_end:				\n"
);

asm (
"	.globl setbreak		\n"
"	.set MSR_DR,0x0010	\n"
"	.set MSR_EE,0x8000	\n"
"setbreak:				\n"

"	mr    %r5, %r3		\n"

"	mfmsr %r11			\n"
"	ori   %r12, %r11, MSR_DR|MSR_EE	\n" //         # Turn off address  translation
"	xori  %r12, %r12, MSR_DR|MSR_EE	\n" //         # and interrupts
"	sync				\n"
"	mtmsr %r12			\n"
"	isync				\n"

//# do modification here, leave r11 intact
"	lwz   %r3, 0(%r5)		\n"
"	stw   %r4, 0(%r5)		\n"

// "	sync				\n"
// "	icbi 0, %r5			\n"
// "	isync				\n"

"	sync           		\n"             //          # make sure store is complete
"	mtmsr         %r11	\n"
"	isync				\n"

"	blr					\n"
);

extern unsigned int meth_start, meth_end;
extern "C" uint32_t setbreak(uint32_t, uint32_t); //uint32_t, uint32_t);
//

// breakpoint insertion

//

bool Breaks::memory_insert_break_instruction (uint32_t address, uint32_t *buffer)
{
	cout << "memory_insert_break_instruction : " << (void *)address << "\n";
	// if(!is_writable_address(address)) return false;

	/* Go supervisor */
	APTR stack = IExec->SuperState();

	/* Make sure to unprotect the memory area */
	// uint32 oldAttr = IMMU->GetMemoryAttrs ((APTR)address, 0);
	// IMMU->SetMemoryAttrs ((APTR)address, 4, MEMATTRF_READ_WRITE);

	uint32_t realAddress = (uint32_t)IMMU->GetPhysicalAddress ((APTR)address);
	if (realAddress == 0x0)
		realAddress = address;

	int hallo = meth_start;
	*buffer = setbreak (realAddress, meth_start);

	// uint32_t *a = (uint32_t *)address;
    // // // *** Install the breakpoint :
    // *buffer = *a;
    // *a = meth_start;

	// Make sure, that it is in memory :
	IExec->CacheClearE((APTR)address, 4, CACRF_ClearI | CACRF_ClearD );

	/* Set old attributes again */
	// IMMU->SetMemoryAttrs ((APTR)address, 4, oldAttr);

	/* Return to old state */
	if (stack) IExec->UserState (stack);

	return true;
}

bool Breaks::memory_remove_break_instruction (uint32_t address, uint32_t *buffer)
{
	cout << "memory_remove_break_instruction : " << (void *)address << "\n";
	// if(!is_writable_address(address)) return false;

	/* Go supervisor */
	APTR stack = IExec->SuperState();
		
	/* Make sure to unprotect the memory area */
	// uint32 oldAttr = IMMU->GetMemoryAttrs ((APTR)address, 0);
	// IMMU->SetMemoryAttrs ((APTR)address, 4, MEMATTRF_READ_WRITE);

	uint32_t realAddress = (uint32_t)IMMU->GetPhysicalAddress ((APTR)address);
	if (realAddress == 0x0)
		realAddress = address;
	setbreak (realAddress, *buffer);

	// uint32_t *a = (uint32_t *)address;
    // // // *** Wipe the trap instruction from code :
    // *a = *buffer;

	// Make sure, that it is in memory :
	IExec->CacheClearE((APTR)address, 4, CACRF_ClearI | CACRF_ClearD );

	/* Set old attributes again */
	// IMMU->SetMemoryAttrs ((APTR)address, 4, oldAttr);

	/* Return to old state */
	if (stack) IExec->UserState(stack);

	return true;
}
#endif