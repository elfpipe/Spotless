#include "Handle.hpp"
#include <vector>

ElfHandle::ElfHandle (APTR handle, string name, bool isOpen) {
    this->name = name;
    this->handle = handle;
    this->isOpen = isOpen;

	this->handleCopy = 0;
	// if (!isOpen)
	// 	open();
    //    lock();
}

ElfHandle::~ElfHandle() {
	// if(isOpen) close();
}

typedef struct {
	string name;
	Elf32_Handle handle;
} SOLib;

int32 solib_callback(struct Hook *hook, APTR handle, struct GetSOLibMsg *msg) {
	vector<SOLib> *v = (vector<SOLib>*)hook->h_Data;
	SOLib sl;
	sl.name = string(msg->SOLibName);
	sl.handle = msg->SOLibHandle;
	v->push_back(sl);
	return 0;
}

vector<ElfHandle *> ElfHandle::getSOLibHandles() {
	vector<SOLib> v;
	Hook *hook = (Hook *)IExec->AllocSysObjectTags(
		ASOT_HOOK,
		ASOHOOK_Entry, solib_callback,
		TAG_END);
	hook->h_Data = (APTR)&v;

	// open();

	if (hook) {
		IElf->GetSOHandles((Elf32_Handle)handle, hook);
		IExec->FreeSysObject(ASOT_HOOK, hook);
	}

	vector<ElfHandle *> result;
	for(vector<SOLib>::iterator it = v.begin(); it != v.end(); it++) {
		ElfHandle *h = new ElfHandle((*it).handle, (*it).name, false);
		result.push_back(h);
	}
	return result;
}

void ElfHandle::lock()
{
	int numsections;
	int index;
	char *strtable;
	IElf->GetElfAttrsTags ((Elf32_Handle)handle,  EAT_NumSections, &numsections, TAG_DONE);

	int i;
	for (i = 0; i < numsections; i++)
	{
		APTR section = IElf->GetSectionTags((Elf32_Handle)handle, GST_SectionIndex, i, TAG_DONE);
		Elf32_Shdr *header = IElf->GetSectionHeaderTags ((Elf32_Handle)handle, GST_SectionIndex, i, TAG_DONE);
		if (!section)
			continue;
		int size = header->sh_size;
		printf( "Locking memory: 0x%x %d", section, size);
		if(!IExec->LockMem(section, size))
			printf("Failed to lock memory!");
	}
}
