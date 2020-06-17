#include "pch.h"
#include "dch.h"

dch::~dch() {
	p_watchlist.clear();
}

watch::~watch() {
	CloseHandle(hDirHandle);
	CloseHandle(hChangeEvent);
}

watch::watch() {
	hDirHandle = NULL;
	hChangeEvent = NULL;
}