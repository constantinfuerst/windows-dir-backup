#include "pch.h"
#include "istr.h"

istr::istr(disk_placeholder) {
	
}

bool istr::operator==(const istr&) {
	return false;
}

bool istr::operator==(disk_placeholder)
{
	return false;
}
