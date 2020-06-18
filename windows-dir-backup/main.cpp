#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dch dirHook;
	dwd* data = new dwd;
	
	data->dir_name = L"C:\\Users\\Admin\\backup-test\\folder";
	data->dest_name = L"C:\\Users\\Admin\\backup-test\\destination";

	dirHook.addWatch(data);
	dirHook.launchWatch();
	
	return 1;
}