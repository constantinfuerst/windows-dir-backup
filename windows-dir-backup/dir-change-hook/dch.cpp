#include "pch.h"
#include "dch.h"

USN_JOURNAL_DATA_V2 dch::obtain_latest_usn_record_number() const {
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
	
	return journal_data_output;
}

void dch::obtain_usn_record_list() {
	DWORD filter = USN_REASON_DATA_EXTEND | USN_REASON_DATA_OVERWRITE | USN_REASON_DATA_TRUNCATION | USN_REASON_FILE_CREATE | USN_REASON_FILE_DELETE | USN_REASON_RENAME_NEW_NAME | USN_REASON_RENAME_OLD_NAME;
	READ_USN_JOURNAL_DATA_V0 journal_read_data = { last_recorded_change, filter, FALSE, 0, 0, journal_id};

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
	auto* usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PUCHAR>(output_buffer) + sizeof(USN));

	while (r_bytes > 0 && r_bytes < output_buffer_size) {
		process_usn_record(usn_record);

		r_bytes -= usn_record->RecordLength;
		usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PUCHAR>(output_buffer) + usn_record->RecordLength);
	}

	memset(output_buffer, 0x00, output_buffer_size);
	bytes_returned = 0;
}

void dch::process_usn_record(const PUSN_RECORD& record) {
	FILE_ID_DESCRIPTOR file_descriptor;
	file_descriptor.Type = FileIdType;
	file_descriptor.FileId.QuadPart = record->FileReferenceNumber;
	file_descriptor.dwSize = sizeof(file_descriptor);
	
	HANDLE file = OpenFileById(
		drive_handle,
		&file_descriptor,
		GENERIC_READ,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		FILE_FLAG_BACKUP_SEMANTICS
	);

	static const unsigned int buf_size = 128;
	auto* buf = new unsigned char[buf_size];
	
	GetFileInformationByHandleEx(
		file,
		FileNameInfo,
		buf,
		buf_size
	);

	auto* ret = reinterpret_cast<FILE_NAME_INFO*>(buf);
	
	auto fname = std::wstring(ret->FileName, ret->FileNameLength);

	delete[] buf;
	CloseHandle(file);

	auto member_directory = [&]() {
		const unsigned int pos = fname.find_last_of(L"\\");
		const std::wstring dirname = fname.substr(0, pos);
		if (dirname == watch_data->dir_name)
			return true;
		else
			return false;
	};
	
	auto& r = record->Reason;
	if (r == USN_REASON_DATA_EXTEND || r == USN_REASON_DATA_OVERWRITE || r == USN_REASON_DATA_TRUNCATION) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::mod, &fname, nullptr);
	}
	else if (r == USN_REASON_FILE_CREATE) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::add, &fname, nullptr);
	}
	else if (r == USN_REASON_FILE_DELETE) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::del, &fname, nullptr);
	}
	else if (r == USN_REASON_RENAME_NEW_NAME || r == USN_REASON_RENAME_OLD_NAME) {
		if (!member_directory()) return;

		std::wstring rename;
		if (rename.empty()) {
			rename = fname;
		}
		else {
			dcr::replicate(watch_data, dcr::mov, &fname, &rename);
			rename = L"";
		}
	}
	else {
		return;
	}
}

dch::dch(dwd* data_) {
	watch_data = data_;
	bytes_returned = 0;
	output_buffer = new unsigned char[output_buffer_size];
	memset(output_buffer, 0x00, output_buffer_size);
	last_recorded_change = 0;
	auto* drive_descriptor = new std::wstring(L"\\\\.\\" + watch_data->dir_name.substr(0, 2));
	
	//obtain drive handle
	drive_handle = CreateFileW(
		drive_descriptor->c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
	);

	delete drive_descriptor;
}

dch::~dch() {
	CloseHandle(drive_handle);
	delete[] output_buffer;
}