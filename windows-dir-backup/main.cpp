#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dch* dirHook = new dch;
	
	std::wstring folder = L"C:\\Users\\Admin\\backup-test";
	dirHook->addWatch(folder);

	dirHook->launchWatch();

	delete dirHook;
	return 1;
}