#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dwd* data = new dwd;
	data->dir_name = L"C:\\Users\\Admin\\backup-test\\folder";
	data->dest_name = L"C:\\Users\\Admin\\backup-test\\destination";
	data->createBin(L"bin");

	dch dir_hook(data);
	dir_hook.launch_watch(1000);
	
	return 1;
}