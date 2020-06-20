#include "pch.h"
#include "ms.h"

bool montitor::add_directory(dir_data* data) {
	//register handle to detect change
	const DWORD p_filter = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_SIZE;
	auto c_handle = FindFirstChangeNotificationW(data->dest_name.c_str(), true, p_filter);
	wait_handles.emplace_back(c_handle);
	//register directory to react to change
	return m_dirmon->add_watch(data);
}

void montitor::launch_monitor(DWORD delay_after_action) {
	auto last_recorded_change = m_dirmon->obtain_latest_usn_record_number();
	
	while (TRUE) {
		WaitForMultipleObjects(wait_handles.size(), &wait_handles[0], FALSE, INFINITE);
		Sleep(delay_after_action);
		last_recorded_change = m_dirmon->obtain_latest_usn_record_number();
		
		const auto bytes_returned = m_dirmon->obtain_usn_record_list(last_recorded_change);
		if (bytes_returned > sizeof(USN)) m_dirmon->process_usn_record_list(bytes_returned);

		for (auto h : wait_handles) FindNextChangeNotification(h);
	}
}

montitor::montitor(dir_data* data) {
	m_dirmon = new dirmon(data);
}

montitor::~montitor() {
	for (auto h : wait_handles)
		FindCloseChangeNotification(h);
	delete m_dirmon;
}