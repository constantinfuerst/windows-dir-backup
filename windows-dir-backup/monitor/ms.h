#pragma once
#include "pch.h"

#include "../directory-data/dir-data.h"
#include "../dir-change-replicate/dcr.h"
#include "../dir-monitor/dirmon.h"

class montitor {
private:
	dirmon* m_dirmon;
	std::vector<HANDLE> wait_handles;
	
public:
	bool add_directory(dwd* data);
	void launch_monitor(DWORD delay_after_action);
	
	montitor(dwd* data);
	~montitor();
};