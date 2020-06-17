#include "pch.h"
#include "dcr.h"

bool dcr::replicate(change_type change, const std::wstring& p_new, const std::wstring& p_old) {
	switch (change) {
	case del: std::wcout << L"Deleted file: " << p_new << std::endl; break;
	case add: std::wcout << L"Created file: " << p_new << std::endl; break;
	case mod: std::wcout << L"Modified file: " << p_new << std::endl; break;
	case mov: std::wcout << L"Moved or Renamed file from: " << p_old << " to: " << p_new << std::endl; break;
	default: break;
	}
	return true;
}
