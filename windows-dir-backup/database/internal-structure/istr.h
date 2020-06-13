#pragma once
#include "pch.h"

//structure providing required information for versioning of datapoints
struct istr {
	int m_version; //calculated from the last change and the locale
	int m_fsize;
	int m_hash;
	std::string m_parent;
	std::string m_fname;

	istr(disk_placeholder);
	bool operator==(const istr&);
	bool operator==(disk_placeholder);
};