#include "pch.h"
#include "dch.h"

DWORDLONG dch::obtain_latest_usn_record_number() const {
	USN_JOURNAL_DATA_V2 journal_data_output;

	DeviceIoControl(
		drive_handle,
		FSCTL_QUERY_USN_JOURNAL,
		NULL,
		0,
		&journal_data_output,
		sizeof(journal_data_output),
		NULL,
		NULL
	);
	
	return journal_data_output.UsnJournalID;
}

void dch::obtain_usn_record_list(USN start, DWORDLONG end) {
	DWORD filter = USN_REASON_DATA_EXTEND | USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_RENAME_NEW_NAME | USN_REASON_RENAME_OLD_NAME;
	DWORDLONG end_cast = end;
	READ_USN_JOURNAL_DATA_V0 journal_read_data = { 0, 0xFFFFFFFF, FALSE, 0, 0,end };

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

	while (r_bytes > 0 && r_bytes < output_buffer_size) {
		process_usn_record(usn_record);

		r_bytes -= usn_record->RecordLength;
		usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PUCHAR>(output_buffer) + usn_record->RecordLength);
	}

	memset(output_buffer, 0x00, output_buffer_size);
	bytes_returned = 0;
}

void dch::process_usn_record(const PUSN_RECORD& record) {
	if (record->FileNameLength == 0) return;
	std::wstring name; name.resize(record->FileNameLength);
	for (int i = 0; i < record->FileNameLength; i++)
		name[i] = record->FileName[i];
	std::wcout << name << std::endl;
}

dch::dch(const std::wstring& directory_) {
	bytes_returned = 0;
	output_buffer = new unsigned char[output_buffer_size];
	memset(output_buffer, 0x00, output_buffer_size);
	last_recorded_change = 0;
	directory = new std::wstring(directory_);
	drive_descriptor = new std::wstring(L"\\\\.\\" + directory->substr(0, 2));
	
	//obtain drive handle
	drive_handle = CreateFileW(
		drive_descriptor->c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);
}

dch::~dch() {
	CloseHandle(drive_handle);
	delete[] output_buffer;
}