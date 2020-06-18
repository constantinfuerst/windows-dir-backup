#pragma once
#include "pch.h"

#include "../dir-watch-descriptor/dwd.h"

namespace dcr {
	enum change_type {
		none, add, del, mov, mod
	};

	bool replicate(dwd* data, change_type change, std::wstring* name, std::wstring* old_name);
}