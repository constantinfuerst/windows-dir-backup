#pragma once
#include "pch.h"

namespace dcr {
	enum change_type {
		add, del, mov, mod
	};
	
	bool replicate(change_type change, std::wstring* name, std::wstring* old_name);
}