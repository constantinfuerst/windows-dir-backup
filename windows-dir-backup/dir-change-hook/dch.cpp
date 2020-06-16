#include "pch.h"
#include "dch.h"

bool dch::addWatch(const std::wstring& dirname) {
	//create new watch object
	p_watchlist.emplace_back();
	p_watchlist.back().dir = dirname;
	
	//obtain a handle to the directory watched
	p_watchlist.back().hDirHandle = CreateFileW(
		dirname.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);

	//set overlapped structure for asynchronous operation
	p_watchlist.back().hChangeEvent = CreateEventW(
		NULL,
		TRUE,
		FALSE,
		dirname.c_str()
	);
	p_watchlist.back().lpOverlapped->hEvent = p_watchlist.back().hChangeEvent;
	
	//initialize directory change reader
	ReadDirectoryChangesW(
		p_watchlist.back().hDirHandle,
		&p_watchlist.back().bBuffer,
		BUFFER_SIZE,
		true,
		p_filter,
		NULL,
		p_watchlist.back().lpOverlapped,
		NULL
	);
	
	return true;
}

void dch::watchThread(watch& wData) {
	DWORD current_buffer_size = BUFFER_SIZE;
	
	while(true) {
		GetOverlappedResultEx(
			wData.hDirHandle,
			wData.lpOverlapped,
			wData.lpNOBT,
			INFINITE,
			true
		);

		current_buffer_size -= *wData.lpNOBT;

		
		
		ResetEvent(wData.hChangeEvent());
	}
}

int dch::processChange(int buffer_pos, byte* buffer, byte* old_buffer) {
	return 0;
}

dch::~dch() {
	p_watchlist.clear();
}

byte* watch::swapBuffer() {
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