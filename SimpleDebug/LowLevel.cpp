#include <proto/exec.h>
#include "LowLevel.hpp"

extern struct MMUIFace *IMMU;


static bool Memory_Readable(APTR suspect)
{
	bool result = false;
	uint32 aflags, tflags;
	APTR   stack;

	do
	{
		/*
		**  We must be in superstate for this call.
		*/
		stack  = IExec->SuperState();
		aflags = IMMU->GetMemoryAttrs(suspect,0);

		if (stack)
		{
			IExec->UserState(stack);
		}

		tflags = IExec->TypeOfMem(suspect);


		if( tflags )
		{
			result = true;
		}
	}
	while(0);


	return( result );
}

bool is_readable_address_st (uint32_t addr)
{
	// return Memory_Readable((APTR)addr);

    uint32 attr, masked;
    APTR stack;
    bool result = true;

      /* Go supervisor */
    stack = IExec->SuperState();
    
	attr = IMMU->GetMemoryAttrs((APTR)addr, 0);

    /* Return to old state */
    if (stack)
        IExec->UserState(stack);

	masked = attr & (MEMATTRF_NOT_MAPPED | MEMATTRF_RW_MASK);
	if(masked != MEMATTRF_SUPER_RW_USER_RW
	&& masked != MEMATTRF_SUPER_RW_USER_RO
	&& masked != MEMATTRF_SUPER_RO_USER_RO
	)
		result = false;
	// masked = attr & MEMATTRF_NOT_MAPPED;
    // if(masked)
    //     result = false;

    return result;
}

#define MAX_STRING 1024
bool is_readable_string(uint32_t addr) {
	int i = 0;
	char *s = (char *)addr;
	bool readable;
	while(i++ < MAX_STRING && (readable = is_readable_address((uint32_t)s)) && *s++)
		;
	return readable;
}

bool is_readable_address (uint32_t addr)
{
	// return Memory_Readable((APTR)addr);

    uint32 attr, masked;
    APTR stack;
    bool result = true;

      /* Go supervisor */
    stack = IExec->SuperState();
    
	attr = IMMU->GetMemoryAttrs((APTR)addr, 0);

    /* Return to old state */
    if (stack)
        IExec->UserState(stack);

	if((attr & (MEMATTRF_NOT_MAPPED | MEMATTRF_RW_MASK)) != MEMATTRF_SUPER_RW_USER_RW)
		result = false;
    // // masked = attr & MEMATTRF_RW_MASK;
    // // if(masked)
    // //     ret = TRUE;

    return result;
}

bool is_writable_address (uint32_t addr)
{
    uint32 attr, oldattr, masked;
    APTR stack;
    BOOL ret = FALSE;

      /* Go supervisor */
    stack = IExec->SuperState();
    
    oldattr = IMMU->GetMemoryAttrs((APTR)addr, 0);
	IMMU->SetMemoryAttrs((APTR)addr, 4, MEMATTRF_READ_WRITE);
	attr = IMMU->GetMemoryAttrs((APTR)addr, 0);
	IMMU->SetMemoryAttrs((APTR)addr, 4, oldattr);

    /* Return to old state */
    if (stack)
        IExec->UserState(stack);

    masked = attr & MEMATTRF_RW_MASK;
    if(masked == MEMATTRF_READ_WRITE)
        ret = TRUE;

    return ret;
}

ppctype PPC_DisassembleBranchInstr(uint32 instr, int32 *reladdr)
{
	unsigned char *p = (unsigned char *)&instr;
	uint32 in = p[0]<<24 | p[1]<<16 | p[2]<<8 | p[3];
	switch (PPCGETIDX(in))
	{
		case 16:
		{
			int d = (int)(in & 0xfffc);

			if (d >= 0x8000)
				d -= 0x10000;
			*reladdr = d;
			return PPC_BRANCHCOND;
		}
		case 18:
		{
			int d = (int)(in & 0x3fffffc);

			if (d >= 0x2000000)
				d -= 0x4000000;
    		*reladdr = d;
		    return PPC_BRANCH;
		}
		case 19:
			switch (PPCGETIDX2(in))
			{
				case 16:
				{
					//return branch(dp,in,"lr",0,0);  /* bclr */
					int bo = (int)PPCGETD(in);
					if((bo & 4) && (bo & 16))
						return PPC_BRANCHTOLINK;
					else
						return PPC_BRANCHTOLINKCOND;
				}

				case 528:
				{
					//return branch(dp,in,"ctr",0,0);  /* bcctr */
					int bo = (int)PPCGETD(in);
					if((bo & 4) && (bo & 16))
						return PPC_BRANCHTOCTR;
					else
						return PPC_BRANCHTOCTRCOND;
				}	
				default:
					return PPC_OTHER;
			}
			break;
		case 31:
			switch(PPCGETIDX2(in))
			{
				case 20:
					return PPC_LWARX;
				default:
					return PPC_OTHER;
			}
		default:
			return PPC_OTHER;
	}
}