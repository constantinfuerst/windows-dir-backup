#include "pch.h"
#include "dcr.h"

bool dcr::replicate(dwd* data, change_type change, std::wstring* name, std::wstring* old_name) {
	if (change == del) {
		std::wcout << L"del of " << *name << L" detected" << std::endl;
		const std::wstring* old_dest = new std::wstring(data->dest_name + L"\\" + *name);
		if (data->enable_bin) {
			const std::wstring* bin = new std::wstring(data->dest_name + L"\\bin\\" + *name);
			MoveFileW(old_dest->c_str(), bin->c_str());
		}
		else DeleteFileW(old_dest->c_str());
	}
	else if (change == add || change == mod) {
		std::wcout << L"add/mod of " << *name << L" detected" << std::endl;
		const std::wstring* source = new std::wstring(data->dir_name + L"\\" + *name);
		const std::wstring* destination = new std::wstring(data->dest_name + L"\\" + *name);
		CopyFileW(source->c_str(), destination->c_str(), false);
	}
	else if (change == mov) {
		std::wcout << L"mov of " << *old_name << L" to " << *name << L" detected" << std::endl;
		const std::wstring* old_dest = new std::wstring(data->dest_name + L"\\" + *old_name);
		const std::wstring* destination = new std::wstring(data->dest_name + L"\\" + *name);
		MoveFileW(old_dest->c_str(), destination->c_str());
	}
	else return false;
	return true;
}
