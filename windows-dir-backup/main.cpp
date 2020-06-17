#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dch dirHook;
	
	std::wstring folder = L"C:\\Users\\Admin\\Desktop\\foldermon";
	dirHook.addWatch(folder);

	dirHook.launchWatch();

	return 1;
}