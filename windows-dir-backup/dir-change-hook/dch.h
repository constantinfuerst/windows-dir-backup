#pragma once
#include "pch.h"

#include "../dir-watch-descriptor/dwd.h"
#include "../dir-change-replicate/dcr.h"

class dch {
	dwd* watch_data;
	USN last_recorded_change;
	DWORDLONG journal_id;

	HANDLE drive_handle;
	DWORD output_buffer_size = 4096;
	DWORD bytes_returned;
	unsigned char* output_buffer;

	USN_JOURNAL_DATA_V2 obtain_latest_usn_record_number() const;
	void obtain_usn_record_list();
	void process_usn_record_list();
	void process_usn_record(const PUSN_RECORD& record);
	
public:
	void test_init() {
		auto jdata = obtain_latest_usn_record_number();
		last_recorded_change = jdata.NextUsn;
		journal_id = jdata.UsnJournalID;
	}
	void test_start() {
		auto usn = obtain_latest_usn_record_number();
		obtain_usn_record_list();
		if (bytes_returned > 0) process_usn_record_list();
		std::cin.get();
	}
	
	dch(dwd* data_);
	~dch();
};