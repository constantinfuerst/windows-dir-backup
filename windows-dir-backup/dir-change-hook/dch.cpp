#include "pch.h"
#include "dch.h"

USN dch::obtain_latest_usn_record() {
	USN_JOURNAL_DATA_V2 journal_data_output;
	LPDWORD bytes_returned_dioc = new DWORD(0);

	DeviceIoControl(
		drive_handle,
		FSCTL_QUERY_USN_JOURNAL,
		NULL,
		0,
		&journal_data_output,
		sizeof(journal_data_output),
		bytes_returned_dioc,
		NULL
	);
	
	return journal_data_output.NextUsn;
}

void dch::obtain_usn_record_list(USN start, USN end) {
	DWORD filter = USN_REASON_DATA_EXTEND | USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_RENAME_NEW_NAME | USN_REASON_RENAME_OLD_NAME;
	READ_USN_JOURNAL_DATA journal_read_data = { 0, filter, FALSE, 0, 0 };

	journal_read_data.StartUsn = start;
	journal_read_data.UsnJournalID = end;

	DeviceIoControl(
		drive_handle,
		FSCTL_READ_USN_JOURNAL,
		&journal_read_data,
		sizeof(journal_read_data),
		output_buffer,
		output_buffer_size,
		&bytes_returned,
		NULL
	);
}

void dch::process_usn_record_list() {
	DWORD r_bytes = bytes_returned - sizeof(USN);
	auto usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PUCHAR>(output_buffer) + sizeof(USN));

	while (r_bytes > 0) {


		r_bytes -= usn_record->RecordLength;
		usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PUCHAR>(output_buffer) + usn_record->RecordLength);
	}

	memset(output_buffer, 0x00, output_buffer_size);
	bytes_returned = 0;
}

void dch::process_usn_record(const PUSN_RECORD& record) {
	
}

dch::dch(const std::wstring& directory_) {
	last_recorded_change = 0;
	directory = new std::wstring(directory_);
	drive_descriptor = new std::wstring(L"\\\\.\\" + directory->substr(0, 2));
	
	//obtain drive handle
	drive_handle = CreateFileW(
		drive_descriptor->c_str(), GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
		NULL
	);
}

dch::~dch() {
	CloseHandle(drive_handle);
	delete[] output_buffer;
}