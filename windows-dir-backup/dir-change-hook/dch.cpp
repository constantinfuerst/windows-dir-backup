#include "pch.h"
#include "dch.h"

void watchCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired) {

}

bool dch::addWatch(const std::wstring& dirname) {
	p_watchlist.emplace_back();
	
	//obtain change watch handle
	p_watchlist.back().hChangeNotification = 
		FindFirstChangeNotificationW(
			dirname.c_str(), 
			true, p_filter
		);

	if (p_watchlist.back().hChangeNotification == INVALID_HANDLE_VALUE ||
		p_watchlist.back().hChangeNotification == NULL)
		return false;
	
	//register wait handle
	p_watchlist.back().dir = dirname;
	if (0 !=
		RegisterWaitForSingleObject(
			p_watchlist.back().phWaitObject,
			p_watchlist.back().hChangeNotification,
			watchCallback,
			&p_watchlist.back(),
			INFINITE,
			WT_EXECUTELONGFUNCTION
		))
		return false;

	return true;
}

dch::~dch() {
	for (auto e : p_watchlist) {
		UnregisterWait(e.phWaitObject);
		FindCloseChangeNotification(e.hChangeNotification);
	}
	p_watchlist.clear();
}
