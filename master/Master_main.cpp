#include "Process.h"
#include <Windows.h>
#include <iostream>

//MASTER

int main(int argC, char** argV)
{
	HANDLE wrote = CreateSemaphoreA(NULL, 0, 1, "SPOVM_lab3_wrote");
	if (wrote == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		return 0;
	}
	HANDLE notify_child = CreateSemaphoreA(NULL, 0, 1, "SPOVM_lab3_notify_child");
	if (notify_child == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		CloseHandle(wrote);
		return 0;
	}
	HANDLE pipe = CreateNamedPipeA("\\\\.\\pipe\\SPOVM_lab3", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE, 2, 255, 255, 10000000, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		CloseHandle(wrote);
		CloseHandle(notify_child);
		return 0;
	}
	Process slave{ "slave.exe" };
	DWORD written = 0;

	std::string buf;

	while (true)
	{
		WaitForSingleObject(wrote, INFINITE);
		std::getline(std::cin, buf);
		ReleaseSemaphore(notify_child, 1, NULL);
		if (buf == "exit")
		{
			if (!WriteFile(pipe, "SPOVM lab3 very specific end message", 37, &written, NULL))
			{
				std::cout << GetLastError() << std::endl;
			}
			break;
		}
		if (!WriteFile(pipe, buf.c_str(), buf.size() + 1, &written, NULL))
		{
			std::cout << GetLastError() << std::endl;
		}
	}



	slave.join();
	CloseHandle(wrote);
	CloseHandle(notify_child);
	CloseHandle(pipe);

	return 0;
}