#include "pch.h"
#include "dcr.h"

bool dcr::checkDirectory(const std::wstring* path) {
	unsigned int pos = path->find_last_of(L"\\");
	DWORD dwAttrib = GetFileAttributesW(path->substr(0, pos).c_str());
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

void dcr::createDirectoryRecursive(const std::wstring* path) {
	unsigned int pos = 0;
	do {
		pos = path->find_first_of(L"\\", pos + 1);
		if (pos > path->size()) return;
		CreateDirectoryW(path->substr(0, pos).c_str(), NULL);
	} while (pos < path->size());
}

bool dcr::replicate(dwd* data, change_type change, std::wstring* name, std::wstring* old_name) {
	if (change == del) {
		const std::wstring* old_dest = new std::wstring(data->dest_name + L"\\" + *name);
		const std::wstring* bin = new std::wstring(data->dest_name + L"\\bin\\" + *name);
		if (!checkDirectory(bin))
			createDirectoryRecursive(bin);
		MoveFileW(old_dest->c_str(), bin->c_str());
	}
	else if (change == add || change == mod) {
		const std::wstring* source = new std::wstring(data->dir_name + L"\\" + *name);
		const std::wstring* destination = new std::wstring(data->dest_name + L"\\" + *name);
		if (!checkDirectory(destination))
			createDirectoryRecursive(destination);
		CopyFileW(source->c_str(), destination->c_str(), false);
	}
	else if (change == mov) {
		const std::wstring* old_dest = new std::wstring(data->dest_name + L"\\" + *old_name);
		const std::wstring* destination = new std::wstring(data->dest_name + L"\\" + *name);
		if (!checkDirectory(destination))
			createDirectoryRecursive(destination);
		MoveFileW(old_dest->c_str(), destination->c_str());
	}
	else return false;
	return true;
}
