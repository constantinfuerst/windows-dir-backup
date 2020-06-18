#include "pch.h"
#include "dch.h"

void dch::processChange(watch* data) {
	FILE_NOTIFY_INFORMATION* f = nullptr;
	unsigned int buffer_pos = 0;
	std::wstring* rename = nullptr;
	bool call = false;
	dcr::change_type change;

	//lamda for renaming cases
	auto re = [&](std::wstring* fname) {
		if (rename != nullptr) {
			change = dcr::mov;
			call = true;
		}
		else rename = new std::wstring(*fname);
	};
	
	do {
		//obtain file change information structure
		//ugly type juggling, no idea how you're supposed to handle
		//the windows api in this case, meh
		char* bufChar = reinterpret_cast<char*>(data->bBuffer);
		bufChar += buffer_pos;
		void* bufVoid = static_cast<void*>(bufChar);
		f = static_cast<FILE_NOTIFY_INFORMATION*>(bufVoid);

		//increment the buffer position
		buffer_pos += f->NextEntryOffset;

		auto* fname = new std::wstring(f->FileName, f->FileNameLength);

		//evaluate the obtained information
		switch (f->Action) {
		case FILE_ACTION_ADDED:
			change = dcr::add; call = true; break;
		case FILE_ACTION_REMOVED:
			change = dcr::del; call = true; break;
		case FILE_ACTION_MODIFIED:
			change = dcr::mod; call = true; break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			re(fname); break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			re(fname); break;
		default: break;
		}

		//spawn an independent thread to handle the replication of the change
		if (call == true) {
			dcr::replicate(data->sData, change, fname, rename);
			rename = nullptr;
			call = false;
		}
	} while (f->NextEntryOffset != 0);
}