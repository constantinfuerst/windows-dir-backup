#pragma once

//data types
#include <string>
#include <iostream>
#include <vector>
#include <atomic>

//functional
#include <thread>

//win32 api
#include <windows.h>
#include <fileapi.h>

//define the minimum windows nt version
//see last paragraph of https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-registerwaitforsingleobject
//for explanation
#define _WIN32_WINNT 0x0500