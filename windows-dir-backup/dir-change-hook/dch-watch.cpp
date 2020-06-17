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
		NULL, TRUE, NULL, dirname.c_str()
	);

	return true;
}

void dch::watchThread(int i) {
	OVERLAPPED* lpOverlapped;
	DWORD* lpNOBT = new DWORD(0);
	byte* bBuffer = new byte[BUFFER_SIZE];

	while (true) {
		memset(bBuffer, 0x00, BUFFER_SIZE);
		
		//prime the file change event
		lpOverlapped = new OVERLAPPED();
		lpOverlapped->hEvent = p_watchlist.back()->hChangeEvent;

		//initialize directory change reader
		ReadDirectoryChangesW(
			p_watchlist.back()->hDirHandle, bBuffer,
			BUFFER_SIZE, true,
			p_filter, lpNOBT, lpOverlapped, NULL
		);
		
		//obtain result
		GetOverlappedResult(
			p_watchlist[i]->hDirHandle, lpOverlapped,
			lpNOBT, true
		);

		//process the recorded change
		processChange(bBuffer);

		//prepare for restart
		delete lpOverlapped;
		*lpNOBT = 0;
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