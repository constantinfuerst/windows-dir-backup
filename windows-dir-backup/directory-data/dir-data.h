#pragma once
#include "pch.h"

struct dir_data {
	std::wstring dir_name;
	std::wstring dest_name;
	bool enable_bin = false;
	inline void createBin(const std::wstring& bin_name) {
		if (dest_name.empty()) return;
		enable_bin = true;
		const std::wstring* bin = new std::wstring(dest_name + L"\\" + bin_name);
		CreateDirectoryW(bin->c_str(), NULL);
	}
};