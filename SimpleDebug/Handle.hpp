//
//
// elfhandle.hpp - keep track of elf handles in application runtime flow (Debug 101)
//
//

#ifndef DB101_ELFHANDLE_HPP
#define DB101_ELFHANDLE_HPP

#include <proto/exec.h>
#include <proto/elf.h>
// #define OET_OpenInput   (TAG_USER+10) /* Reopen filehandle to do read operations
//                                          on a living process. Used together wil
//                                          OET_ElfHandle */

#include <string>

using namespace std;

class ElfHandle
{
private:
    string name;
	APTR handle;  //problem with c++ and Elf32_Handle typedef in AmigaOS
	APTR handleCopy;
	bool isOpen;

public:
	ElfHandle (APTR handle, string name, bool isOpen = false);
	~ElfHandle();
	
	APTR getHandle () { return handleCopy; }
    string getName() { return name; }

	bool open () {	
		if(isOpen) return false;
		handleCopy = IElf->OpenElfTags(OET_ReadOnlyCopy, TRUE, OET_ElfHandle, handle, /*OET_OpenInput, TRUE,*/ TAG_DONE);
        isOpen = true;
		return isOpen;
	}
	void close () {
		if(!isOpen) return;
		IElf->CloseElfTags ((Elf32_Handle)handleCopy, CET_ReClose, TRUE, /*CET_CloseInput, TRUE, CET_FreeUnneeded, FALSE,*/ TAG_DONE);
		handleCopy = 0;
		isOpen = false;
	}
	
	bool performRelocation () {
		IElf->RelocateSectionTags((Elf32_Handle)handleCopy, GST_SectionName, ".stabstr", GST_Load, TRUE, TAG_DONE );
		return IElf->RelocateSectionTags((Elf32_Handle)handleCopy, GST_SectionName, ".stab", GST_Load, TRUE, TAG_DONE );
	}
	
	char *getStabstrSection () { return (char *)IElf->GetSectionTags((Elf32_Handle)handleCopy, GST_SectionName, ".stabstr", TAG_DONE); }
	uint32_t *getStabSection () { return (uint32_t *)IElf->GetSectionTags((Elf32_Handle)handleCopy, GST_SectionName, ".stab", TAG_DONE); }
	Elf32_Shdr *getStabSectionHeader () { return IElf->GetSectionHeaderTags ((Elf32_Handle)handleCopy, GST_SectionName, ".stab", TAG_DONE); }
	uint32_t getStabsSize () {
		Elf32_Shdr *header = IElf->GetSectionHeaderTags ((Elf32_Handle)handleCopy, GST_SectionName, ".stab", TAG_DONE);
		return header->sh_size;
	}
	void lock();
};
#endif //DB101_ELFHANDLE_HPP
