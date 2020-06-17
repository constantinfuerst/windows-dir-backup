#pragma once
#include "pch.h"
#include "../dir-change-replicate/dcr.h"

constexpr unsigned int BUFFER_SIZE = 65636;

//provide single interface for storage of relevant data and handles
//used by the watch functions with automatic cleanup of data after use
struct watch {
	//stored relevant data
	std::wstring dir;
	HANDLE hDirHandle = NULL;
	HANDLE hChangeEvent = NULL;
	std::atomic<void*> bBuffer = new byte[BUFFER_SIZE];
	LPOVERLAPPED lpOverlapped = new _OVERLAPPED;
	LPDWORD lpNOBT = new DWORD(0);

	//to swap to a new buffer, should the supplied one be full but function does not check for free buffer space
	//returns the pointer to the old and now unused buffer for the program to finish working on any new data added
	//as the buffer* is atomic data may be slpit between the old and the current (newly swapped) buffer
	void* swapBuffer();
	
	//destructor to provide cleanup
	~watch();
	watch() = default;
};

class dch {
private:
	static const DWORD p_filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION;
	static const short p_bmemsize = sizeof(_FILE_NOTIFY_INFORMATION);
	//access to a single watchlist element is not shared
	//thereby it does not require any type of lock to prevent concurrent access
	std::vector<watch*> p_watchlist;
	
	void watchThread(int i);
	
	//processes the recorded changes and returns the position of the current
	//replication state in the buffer (incremented by one for the next function call)
	//may take in a second "old" buffer produced by the "swapBuffer" function,
	//fully consuming (including deletion) said buffer
	static void processChange(unsigned int& buffer_pos, void* buffer, void* old_buffer = nullptr);
	
public:
	
	void launchWatch();
	bool addWatch(const std::wstring& dirname);
	~dch();
};