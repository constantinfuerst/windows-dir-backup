#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dwd* data = new dwd;
	data->dir_name = L"C:\\Users\\Admin\\backup-test\\folder";
	data->dest_name = L"C:\\Users\\Admin\\backup-test\\destination";
	data->createBin(L"bin");

	dch dir_hook(data->dir_name);

	dir_hook.test_init();

	Sleep(1000);

	dir_hook.test_start();
	
	return 1;
}