#pragma once
#include "pch.h"

namespace dcr {
	enum change_type {
		add, del, mov, mod
	};
	bool replicate(change_type change, const std::wstring& p_new, const std::wstring& p_old = L"");
};