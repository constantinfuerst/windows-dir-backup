#pragma once
#include "pch.h"

#include "directory-data/dir-data.h"
#include "dir-change-replicate/dcr.h"
#include "dir-monitor/dirmon.h"

class montitor {
private:
	dirmon* m_dirmon;
	std::vector<HANDLE> wait_handles;
	
public:
	//only one drive per monitor instance
	//to monitor multiple drives one has to launch multiple monitor instances in different threads
	bool add_directory(dir_data* data);
	void launch_monitor(DWORD delay_after_action);
	
	montitor(dir_data* data);
	~montitor();
};