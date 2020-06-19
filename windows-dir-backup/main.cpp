#include "pch.h"
#include "dir-change-replicate/dcr.h"
#include "dir-change-hook/dch.h"

int main() {
	dwd* data = new dwd;
	data->dir_name = L"C:\\Users\\Admin\\backup-test\\folder";
	data->dest_name = L"C:\\Users\\Admin\\backup-test\\destination";
	data->createBin(L"bin");

	dch dir_hook(data);

	while(true) {
		std::cout << "Test init" << std::endl;
		dir_hook.test_init();
		Sleep(10000);
		std::cout << "Starting USN readout" << std::endl;
		dir_hook.test_start();
	}
	
	return 1;
}