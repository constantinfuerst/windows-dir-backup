#pragma once
#include "pch.h"

struct watchCollection {
	std::wstring dir;
	HANDLE hChangeNotification;
	PHANDLE phWaitObject;
};

class dch {
public:
	const DWORD p_filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE;
	std::vector<watchCollection> p_watchlist;
	bool addWatch(const std::wstring& dirname);
	~dch();
};

void watchCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired);