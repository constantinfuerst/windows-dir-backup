#include "pch.h"
#include "dcr.h"

bool dcr::replicate(change_type change, std::wstring* name, std::wstring* old_name) {
	switch (change) {
	case del: std::wcout << L"Deleted file: " << *name << std::endl; break;
	case add: std::wcout << L"Created file: " << *name << std::endl; break;
	case mod: std::wcout << L"Modified file: " << *name << std::endl; break;
	case mov: std::wcout << L"Moved or Renamed file from: " << *old_name << " to: " << *name << std::endl; break;
	default: break;
	}
	return true;
}
