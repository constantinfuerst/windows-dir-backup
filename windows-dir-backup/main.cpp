#include "pch.h"
#include "monitor/ms.h"

int main() {
	dir_data* testfolder_one = new dir_data;
	testfolder_one->dir_name = L"C:\\Users\\Admin\\backup-test\\folder_one";
	testfolder_one->dest_name = L"C:\\Users\\Admin\\backup-test\\destination_one";
	testfolder_one->createBin(L"bin");

	dir_data* testfolder_two = new dir_data;
	testfolder_one->dir_name = L"C:\\Users\\Admin\\backup-test\\folder_two";
	testfolder_one->dest_name = L"C:\\Users\\Admin\\backup-test\\destination_two";
	testfolder_one->createBin(L"bin");
	
	montitor m(testfolder_one);
	m.add_directory(testfolder_two);
	m.launch_monitor(1000);
	
	return 1;
}