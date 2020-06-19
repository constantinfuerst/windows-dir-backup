#pragma once
#include "pch.h"

#include "../dir-watch-descriptor/dwd.h"
#include "../dir-change-replicate/dcr.h"

class dch {
	HANDLE drive_handle;
	USN last_recorded_change;
	DWORD output_buffer_size = 4096;
	DWORD bytes_returned;
	unsigned char* output_buffer;
	std::wstring* drive_descriptor;
	std::wstring* directory;

	DWORDLONG obtain_latest_usn_record_number() const;
	void obtain_usn_record_list(USN start, DWORDLONG end);
	void process_usn_record_list();
	static void process_usn_record(const PUSN_RECORD& record);
	
public:
	void test_init() {
		last_recorded_change = obtain_latest_usn_record_number();
	}
	void test_start() {
		auto usn = obtain_latest_usn_record_number();
		obtain_usn_record_list(last_recorded_change, usn);
		if (bytes_returned > 0) process_usn_record_list();
		std::cin.get();
	}
	
	dch(const std::wstring& directory);
	~dch();
};