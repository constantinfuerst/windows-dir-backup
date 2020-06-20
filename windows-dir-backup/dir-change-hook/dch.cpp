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
	auto* usn_record = reinterpret_cast<PUSN_RECORD>(static_cast<PCHAR>(output_buffer) + sizeof(USN));

	while (r_bytes > 0 && r_bytes < output_buffer_size) {
		process_usn_record(usn_record);

		r_bytes -= usn_record->RecordLength;
		usn_record = reinterpret_cast<PUSN_RECORD>(reinterpret_cast<PCHAR>(usn_record) + usn_record->RecordLength);
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

	if (file == INVALID_HANDLE_VALUE) {
		return;
	}

	static const unsigned int buf_size = 512;
	auto* buf = new unsigned char[buf_size];
	
	GetFileInformationByHandleEx(
		file,
		FileNameInfo,
		buf,
		buf_size
	);

	auto* ret = reinterpret_cast<FILE_NAME_INFO*>(buf);

	auto fname = watch_data->dir_name.substr(0, 2) + std::wstring(ret->FileName, ret->FileNameLength / 2);

	delete[] buf;
	CloseHandle(file);

	std::wstring relative_filename;

	//checks if the file is member of a watched directory
	//additionally generates filename relative to the directory observed
	auto member_directory = [&]() {
		auto dn = watch_data->dir_name;
		std::wstringstream wstr;
		
		if (fname.size() < dn.size()) return false;
		for (int i = 0; i < fname.size(); i++) {
			auto c = fname[i];
			if (i < dn.size()) {
				if (dn[i] != c) return false;
				else continue;
			}
			else wstr << c;
		}

		relative_filename = wstr.str();
		return true;
	};
	
	auto& r = record->Reason;
	if (r == USN_REASON_DATA_EXTEND || r == USN_REASON_DATA_OVERWRITE || r == USN_REASON_DATA_TRUNCATION) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::mod, &relative_filename, nullptr);
	}
	else if (r == USN_REASON_FILE_CREATE) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::add, &relative_filename, nullptr);
	}
	else if (r == USN_REASON_FILE_DELETE) {
		if (!member_directory()) return;

		dcr::replicate(watch_data, dcr::del, &relative_filename, nullptr);
	}
	else if (r == USN_REASON_RENAME_NEW_NAME || r == USN_REASON_RENAME_OLD_NAME) {
		if (!member_directory()) return;

		std::wstring rename;
		if (rename.empty()) {
			rename = relative_filename;
		}
		else {
			dcr::replicate(watch_data, dcr::mov, &relative_filename, &rename);
			rename = L"";
		}
	}
	else {
		return;
	}
}

void dch::test_execute() {
	auto jdata = obtain_latest_usn_record_number();
	journal_id = jdata.UsnJournalID;

	while (TRUE) {
		last_recorded_change = jdata.NextUsn;

		std::cout << "[+] waiting 5000ms" << std::endl;
		Sleep(5000);

		std::cout << "[+] obtaining current usn" << std::endl;
		jdata = obtain_latest_usn_record_number();

		obtain_usn_record_list();
		if (bytes_returned > sizeof(USN)) {
			std::cout << "[+] processing " << bytes_returned << "b of changes" << std::endl;
			process_usn_record_list();
		}
		else {
			std::cout << "[x] no changes detected" << std::endl;
		}

		std::cout << "//////////" << std::endl;
	}
}

dch::dch(dwd* data_) {
	watch_data = data_;
	bytes_returned = 0;
	output_buffer = new char[output_buffer_size];
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