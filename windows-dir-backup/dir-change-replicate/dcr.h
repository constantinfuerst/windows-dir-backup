#pragma once
#include "pch.h"

class dcr {
public:
	enum change_type {
		rename, move, write
	};

private:

public:
	bool replicate(change_type change, std::filesystem::path p_new, std::filesystem::path p_old);
};