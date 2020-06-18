#include "pch.h"
#include "dch.h"

bool dch::addWatch(dwd* data) {
	//create new watch object
	p_watchlist.emplace_back(new watch);
	p_watchlist.back()->sData = data;

	//obtain a handle to the directory watched
	p_watchlist.back()->hDirHandle = CreateFileW(
		data->dir_name.c_str(),
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);

	//set overlapped structure for asynchronous operation
	p_watchlist.back()->hChangeEvent = CreateEventW(
		NULL, TRUE, NULL, data->dir_name.c_str()
	);

	return true;
}

void dch::watchThread(watch* data) {
	OVERLAPPED* lpOverlapped;
	DWORD* lpNOBT = new DWORD(0);

	while (true) {
		//prime the file change event
		lpOverlapped = new OVERLAPPED();
		lpOverlapped->hEvent = data->hChangeEvent;

		//initialize directory change reader
		ReadDirectoryChangesW(
			p_watchlist.back()->hDirHandle, data->bBuffer,
			BUFFER_SIZE, true,
			p_filter, lpNOBT, lpOverlapped, NULL
		);
		
		//obtain result
		GetOverlappedResult(
			data->hDirHandle, lpOverlapped,
			lpNOBT, true
		);

		//process the recorded change
		//TODO: consider doing the branch-off into a separate thread right here
		//instead of branching at the end of the process change function
		//as a possible test location if performance is not at the required level
		processChange(data);

		//prepare for restart
		data->refreshBuffer();
		delete lpOverlapped;
		*lpNOBT = 0;
		ResetEvent(data->hChangeEvent);
	}
}

void dch::launchWatch() {
	std::vector<std::thread> tpool;
	for (watch* data : p_watchlist) {
		tpool.emplace_back(std::thread(&dch::watchThread, this, data));
	}
	for (auto& e : tpool) {
		e.join();
	}
}