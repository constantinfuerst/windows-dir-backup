#include "pch.h"
#include "dch.h"

dch::~dch() {
	p_watchlist.clear();
}

void* watch::swapBuffer() {
	auto* old_buffer = bBuffer;
	auto* new_buffer = new byte[BUFFER_SIZE];
	memset(new_buffer, 0x00, BUFFER_SIZE);

	bBuffer = new_buffer;

	return old_buffer;
}

watch::~watch() {
	CloseHandle(hDirHandle);
	CloseHandle(hChangeEvent);
	delete lpOverlapped;
	delete lpNOBT;
	delete[] bBuffer;
}

watch::watch() {
	hDirHandle = NULL;
	hChangeEvent = NULL;
	bBuffer = new byte[BUFFER_SIZE];
	memset(bBuffer, 0x00, BUFFER_SIZE);
	lpOverlapped = new OVERLAPPED();
	lpNOBT = new DWORD(0);
}