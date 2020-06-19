#pragma once
#include "pch.h"

#include "../dir-watch-descriptor/dwd.h"
#include "../dir-change-replicate/dcr.h"

class dch {
	HANDLE drive_handle;
	USN last_recorded_change;
	DWORD output_buffer_size = 4096;
	DWORD bytes_returned = 0;
	byte* output_buffer;
	std::wstring* drive_descriptor;
	std::wstring* directory;

	USN obtain_latest_usn_record();
	void obtain_usn_record_list(USN start, USN end);
	void process_usn_record_list();
	static void process_usn_record(const PUSN_RECORD& record);
	
public:

	dch(const std::wstring& directory);
	~dch();
};