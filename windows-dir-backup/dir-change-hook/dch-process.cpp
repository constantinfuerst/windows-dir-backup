#include "pch.h"
#include "dch.h"

void dch::processChange(unsigned int& buffer_pos, void* buffer, void* old_buffer) {
	void* bufCurrent = nullptr;
	FILE_NOTIFY_INFORMATION* f = nullptr;
	std::wstring rename;
	
restart:
	if (old_buffer != nullptr) bufCurrent = old_buffer;
	else bufCurrent = buffer;
	///wrappers for old buffer before

	do {
		//obtain file change information structure
		//ugly type juggling, no idea how you're supposed to handle
		//the windows api in this case, meh
		char* bufChar = static_cast<char*>(bufCurrent);
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
		case FILE_ACTION_RENAMED_OLD_NAME:
			if (!rename.empty()) {
				dcr::replicate(dcr::mov, fname, rename);
				rename = L"";
			}
			else rename = fname;
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			if (!rename.empty()) {
				dcr::replicate(dcr::mov, rename, fname);
				rename = L"";
			}
			else rename = fname;
			break;
		}
	} while (f->NextEntryOffset != 0);
	
	///wrappers for old buffer following
	if (old_buffer != nullptr) {
		buffer_pos = 0;
		delete[] old_buffer;
		old_buffer = nullptr;
		goto restart;
	}
}