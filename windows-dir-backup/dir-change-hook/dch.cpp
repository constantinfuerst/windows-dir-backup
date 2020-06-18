#include "pch.h"
#include "dch.h"

dch::~dch() {
	p_watchlist.clear();
}

watch::~watch() {
	delete[] bBuffer;
	CloseHandle(hDirHandle);
	CloseHandle(hChangeEvent);
}

void watch::refreshBuffer() {
	memset(bBuffer, 0x00, BUFFER_SIZE);
}

watch::watch() {
	bBuffer = new byte[BUFFER_SIZE];
	refreshBuffer();
	hDirHandle = NULL;
	hChangeEvent = NULL;
}