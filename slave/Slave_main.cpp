#include <iostream>
#include <Windows.h>

int main()
{
	char message[256];
	bool to_exit = false;
	DWORD read = 0;
	HANDLE pipe = CreateFileA("\\\\.\\pipe\\SPOVM_lab3", GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (pipe == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		system("pause");
		return 0;
	}
	HANDLE wrote = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, false, "SPOVM_lab3_wrote");
	if (wrote == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		CloseHandle(pipe);
		system("pause");
		return 0;
	}
	HANDLE notify_child = OpenSemaphoreA(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, false, "SPOVM_lab3_notify_child");
	if (notify_child == INVALID_HANDLE_VALUE)
	{
		std::cout << GetLastError() << std::endl;
		CloseHandle(pipe);
		CloseHandle(wrote);
		system("pause");
		return 0;
	}
	ReleaseSemaphore(wrote, 1, NULL);
	while (!to_exit)
	{
		WaitForSingleObject(notify_child, INFINITE);
		while (true)
		{
			if (ReadFile(pipe, message, 255, &read, NULL))
			{
				if (message[read - 1] == '\0')
				{
					if (strcmp(message, "SPOVM lab3 very specific end message") == 0)
					{
						to_exit = true;
						break;
					}
					std::cout << message << std::endl;
					break;
				}
				else
				{
					message[read] = '\0';
					std::cout << message;
				}
			}
			else
			{
				std::cout << "ERROR: " << GetLastError() << std::endl;
			}
		}
		ReleaseSemaphore(wrote, 1, NULL);
	}

	CloseHandle(pipe);
	CloseHandle(wrote);
	CloseHandle(pipe);
	return 0;
}