#pragma once
#include "pch.h"

#include "../directory-data/dir-data.h"
#include "../dir-monitor/dirmon.h"
#include "../dir-change-replicate/dcr.h"

class dirmon {
	std::wstring drive_letter;
	std::vector<dwd*> watch_data;
	DWORDLONG journal_id;

	HANDLE drive_handle;
	DWORD output_buffer_size = 65536;
	char* output_buffer;

	HANDLE change_handle;
	
	void process_usn_record(const PUSN_RECORD& record);
	
public:
	DWORD obtain_usn_record_list(USN last_recorded_change);
	USN obtain_latest_usn_record_number();
	void process_usn_record_list(DWORD bytes_returned);

	//adds another directory to monitor
	//must be on same device as the original
	bool add_watch(dwd* data_);

	dirmon(dwd* data_);
	~dirmon();
};