#include "pch.h"
#include "monitor/ms.h"

int main() {
	dwd* testfolder_one = new dwd;
	testfolder_one->dir_name = L"C:\\Users\\Admin\\backup-test\\folder_one";
	testfolder_one->dest_name = L"C:\\Users\\Admin\\backup-test\\destination_one";
	testfolder_one->createBin(L"bin");

	dwd* testfolder_two = new dwd;
	testfolder_one->dir_name = L"C:\\Users\\Admin\\backup-test\\folder_two";
	testfolder_one->dest_name = L"C:\\Users\\Admin\\backup-test\\destination_two";
	testfolder_one->createBin(L"bin");
	
	montitor m(testfolder_one);
	m.add_directory(testfolder_two);
	m.launch_monitor(1000);
	
	return 1;
}