#pragma once
#include "pch.h"

#include "../directory-data/dir-data.h"

namespace dcr {
	enum change_type {
		none, add, del, mov, mod
	};

	bool replicate(dwd* data, change_type change, std::wstring* name, std::wstring* old_name);
}