#include "pch.h"
#include "dch.h"

bool dch::addWatch(const std::wstring& dirname) {
	//create new watch object
	p_watchlist.emplace_back(new watch);
	p_watchlist.back()->dir = dirname;

	//obtain a handle to the directory watched
	p_watchlist.back()->hDirHandle = CreateFileW(
		dirname.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);

	//set overlapped structure for asynchronous operation
	p_watchlist.back()->hChangeEvent = CreateEventW(
		NULL,
		TRUE,
		NULL,
		L"test12"
	);
	p_watchlist.back()->lpOverlapped->hEvent = p_watchlist.back()->hChangeEvent;
	
	//initialize directory change reader
	ReadDirectoryChangesW(
		p_watchlist.back()->hDirHandle,
		p_watchlist.back()->bBuffer,
		BUFFER_SIZE,
		true,
		p_filter,
		p_watchlist.back()->lpNOBT,
		p_watchlist.back()->lpOverlapped,
		NULL
	);

	return true;
}

void dch::watchThread(int i) {
	DWORD free_buffer = BUFFER_SIZE;
	void* old_buffer = nullptr;
	unsigned int buffer_pos = 0;

	while (true) {
		GetOverlappedResult(
			p_watchlist[i]->hDirHandle,
			p_watchlist[i]->lpOverlapped,
			p_watchlist[i]->lpNOBT,
			true
		);

		//if buffer is close to full: call to swap buffer
		//this will (without interruption hopefully) update to a new buffer
		free_buffer -= *p_watchlist[i]->lpNOBT;
		if (free_buffer < 1024)
			old_buffer = p_watchlist[i]->swapBuffer();

		processChange(buffer_pos, p_watchlist[i]->bBuffer, old_buffer);
		buffer_pos += *p_watchlist[i]->lpNOBT;

		*p_watchlist[i]->lpNOBT = 0;
		ResetEvent(p_watchlist[i]->hChangeEvent);
	}
}

void dch::launchWatch() {
	std::vector<std::thread> tpool;
	for (int i = 0; i < p_watchlist.size(); i++) {
		tpool.emplace_back(std::thread(&dch::watchThread, this, i));
	}
	for (auto& e : tpool) {
		e.join();
	}
}