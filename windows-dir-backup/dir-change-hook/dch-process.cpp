#include "pch.h"
#include "dch.h"

void dch::processChange(void* buffer) {
	FILE_NOTIFY_INFORMATION* f = nullptr;
	unsigned int buffer_pos = 0;
	std::wstring rename;
	
	do {
		//obtain file change information structure
		//ugly type juggling, no idea how you're supposed to handle
		//the windows api in this case, meh
		char* bufChar = static_cast<char*>(buffer);
		bufChar += buffer_pos;
		void* bufVoid = static_cast<void*>(bufChar);
		f = static_cast<FILE_NOTIFY_INFORMATION*>(bufVoid);

		//increment the buffer position
		buffer_pos += f->NextEntryOffset;

		auto fname = std::wstring(f->FileName, f->FileNameLength);
		switch (f->Action) {
		case FILE_ACTION_ADDED: dcr::replicate(dcr::add, fname); break;
		case FILE_ACTION_REMOVED: dcr::replicate(dcr::del, fname); break;
		case FILE_ACTION_MODIFIED: dcr::replicate(dcr::mod, fname); break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			if (!rename.empty()) {
				dcr::replicate(dcr::mov, fname, rename);
				rename = L"";
			}
			else rename = fname;
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			if (!rename.empty()) {
				dcr::replicate(dcr::mov, rename, fname);
				rename = L"";
			}
			else rename = fname;
			break;
		}
	} while (f->NextEntryOffset != 0);
}