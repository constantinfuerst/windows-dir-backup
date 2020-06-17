#include "pch.h"
#include "dch.h"

dch::~dch() {
	p_watchlist.clear();
}

void* watch::swapBuffer() {
	auto* old_buffer = bBuffer.load();
	auto* new_buffer = new byte[BUFFER_SIZE];

	bBuffer.exchange(new_buffer);

	return old_buffer;
}

watch::~watch() {
	CloseHandle(hDirHandle);
	CloseHandle(hChangeEvent);
	delete lpOverlapped;
	delete lpNOBT;
	delete[] bBuffer;
}