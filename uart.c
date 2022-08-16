/*------------------------------------------------------------------------
-
-	NOME:	uart.dll
-
-	DATA:	13/11/2015
-
-	AUTORE:	Errico Giuliano
-
------------------------------------------------------------------------*/

#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MODBUS_GENERATOR 0xA001
#define MAX_PORT 100

BOOL Ok;
DCB dcb;

DWORD NumberOfBytesRead;
DWORD NumberOfBytesWritten = 0;

HWND APIENTRY __declspec(dllexport) CreateComm(char *, int, char *);
int APIENTRY __declspec(dllexport) SendCommand(HWND, unsigned char *, unsigned int);
int APIENTRY __declspec(dllexport) ReadCommand(HWND, unsigned char *, unsigned int);
int APIENTRY __declspec(dllexport) SRCommand(HWND, unsigned char *, unsigned int, unsigned char *, unsigned int, unsigned int);
unsigned long APIENTRY __declspec(dllexport) ModbusCalcCRC(unsigned char *, int);
void APIENTRY __declspec(dllexport) CloseComm(HWND);
int APIENTRY __declspec(dllexport) GetAvailableCom(char *);
void ShowErrorMessage(int);

/*------------------------------------------------------------------------
 Procedure:     LibMain ID:1
 Purpose:       Dll entry point.Called when a dll is loaded or
				unloaded by a process, and when new threads are
				created or destroyed.
 Input:         hDllInst: Instance handle of the dll
				fdwReason: event: attach/detach
				lpvReserved: not used
 Output:        The return value is used only when the fdwReason is
				DLL_PROCESS_ATTACH. True means that the dll has
				sucesfully loaded, False means that the dll is unable
				to initialize and should be unloaded immediately.
 Errors:
------------------------------------------------------------------------*/
BOOL WINAPI __declspec(dllexport) LibMain(HINSTANCE hDLLInst, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// The DLL is being loaded for the first time by a given process.
		// Perform per-process initialization here.  If the initialization
		// is successful, return TRUE; if unsuccessful, return FALSE.

		break;
	case DLL_PROCESS_DETACH:
		// The DLL is being unloaded by a given process.  Do any
		// per-process clean up here, such as undoing what was done in
		// DLL_PROCESS_ATTACH.  The return value is ignored.

		break;
	case DLL_THREAD_ATTACH:
		// A thread is being created in a process that has already loaded
		// this DLL.  Perform any per-thread initialization here.  The
		// return value is ignored.

		break;
	case DLL_THREAD_DETACH:
		// A thread is exiting cleanly in a process that has already
		// loaded this DLL.  Perform any per-thread clean up here.  The
		// return value is ignored.

		break;
	}
	return TRUE;
}

HWND APIENTRY __declspec(dllexport) CreateComm(char *nCom, int baud, char *conf)
{
	HWND hPort;
	char strConf[128];
	char portName[128];

	strcpy(portName, "\\\\.\\");
	strcat(portName, nCom);

	switch (baud)
	{
	case 110:
		strcpy(strConf, "baud=110");
		break;
	case 300:
		strcpy(strConf, "baud=300");
		break;
	case 600:
		strcpy(strConf, "baud=600");
		break;
	case 1200:
		strcpy(strConf, "baud=1200");
		break;
	case 2400:
		strcpy(strConf, "baud=2400");
		break;
	case 4800:
		strcpy(strConf, "baud=4800");
		break;
	case 9600:
		strcpy(strConf, "baud=9600");
		break;
	case 14400:
		strcpy(strConf, "baud=14400");
		break;
	case 19200:
		strcpy(strConf, "baud=19200");
		break;
	case 38400:
		strcpy(strConf, "baud=38400");
		break;
	case 56000:
		strcpy(strConf, "baud=56000");
		break;
	case 57600:
		strcpy(strConf, "baud=57600");
		break;
	case 115200:
		strcpy(strConf, "baud=115200");
		break;
	case 128000:
		strcpy(strConf, "baud=128000");
		break;
	case 256000:
		strcpy(strConf, "baud=256000");
		break;
	case 500000:
		strcpy(strConf, "baud=500000");
		break;
	case 1000000:
		strcpy(strConf, "baud=1000000");
		break;
	default:
		return (0);
		break;
	}

	if (strlen(conf) != 3)
		return (0);

	switch (conf[0])
	{
	case '8':
		strcat(strConf, " data=8");
		break;
	case '7':
		strcat(strConf, " data=7");
		break;
	case '6':
		strcat(strConf, " data=6");
		break;
	case '5':
		strcat(strConf, " data=5");
		break;
	default:
		return (0);
	}

	switch (conf[1])
	{
	case 'N':
	case 'n':
		strcat(strConf, " parity=n");
		break;
	case 'E':
	case 'e':
		strcat(strConf, " parity=e");
		break;
	case 'O':
	case 'o':
		strcat(strConf, " parity=o");
		break;
	default:
		return (0);
		break;
	}

	switch (conf[2])
	{
	case '1':
		strcat(strConf, " stop=1");
		break;
	case '2':
		strcat(strConf, " stop=2");
		break;
	default:
		return (0);
		break;
	}

	hPort = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
	if (hPort == INVALID_HANDLE_VALUE)
	{
		ShowErrorMessage(GetLastError());
		return 0;
	}
	if (!GetCommState(hPort, &dcb))
	{
		ShowErrorMessage(GetLastError());
		return 0;
	}

	if (!BuildCommDCBA(strConf, &dcb))
	{
		ShowErrorMessage(GetLastError());
		CloseHandle(hPort);
		return 0;
	}

	if (!SetCommState(hPort, &dcb))
	{
		ShowErrorMessage(GetLastError());
		return 0;
	}

	COMMTIMEOUTS Cptimeouts;

	Cptimeouts.ReadIntervalTimeout = MAXDWORD;
	Cptimeouts.ReadTotalTimeoutMultiplier = 0;
	Cptimeouts.ReadTotalTimeoutConstant = 0;
	Cptimeouts.WriteTotalTimeoutMultiplier = 0;
	Cptimeouts.WriteTotalTimeoutConstant = 0;

	if (!SetCommTimeouts(hPort, &Cptimeouts))
	{
		ShowErrorMessage(GetLastError());
		return 0;
	}

	return hPort;
}

int APIENTRY __declspec(dllexport) SendCommand(HWND hPort, unsigned char *send, unsigned int lenSend)
{

	Ok = WriteFile(
		hPort,				   // handle to file to write to
		send,				   // pointer to data to write
		lenSend,			   // number of bytes to write
		&NumberOfBytesWritten, // ptr.to n.bytes written
		NULL				   // pointer to structure for
	);
	sleep(80);
	if (!Ok)
		return 0;
	return NumberOfBytesWritten;
}

int APIENTRY __declspec(dllexport) ReadCommand(HWND hPort, unsigned char *recv, unsigned int lenFrame)
{

	for (int i = 0; i < lenFrame; i++)
		recv[i] = 0xFF;

	Ok = ReadFile(
		hPort,				// handle to file to write to
		recv,				// pointer to buffer that receives data
		strlen(recv),		// number of bytes to read
		&NumberOfBytesRead, // ptr. to n. of bytes read
		NULL				// pointer to structure for
	);

	for (int i = NumberOfBytesRead; i < lenFrame; i++)
		recv[i] = '\0';
	return NumberOfBytesRead;
}

int APIENTRY __declspec(dllexport) SRCommand(HWND hPort, unsigned char *send, unsigned int lenS, unsigned char *recv, unsigned int lenR, unsigned int delay)
{
	SendCommand(hPort, send, lenS);
	sleep(delay);
	return ReadCommand(hPort, recv, lenR);
}

unsigned long APIENTRY __declspec(dllexport) ModbusCalcCRC(unsigned char *Frame, int lenFrame)
{
	unsigned char CntByte;
	unsigned char j;
	unsigned char bitVal;
	unsigned int CRC;

	lenFrame &= 65535;
	CRC = 0xFFFF;

	if (lenFrame < 255)
	{
		for (CntByte = 0; CntByte < lenFrame; CntByte++)
		{
			CRC ^= Frame[CntByte];
			for (j = 0; j < 8; j++)
			{
				bitVal = CRC & 0x0001;
				CRC = CRC >> 1;
				if (bitVal == 1)
					CRC ^= MODBUS_GENERATOR;
			}
		}
	}
	return CRC & 0xFFFF;
}

void APIENTRY __declspec(dllexport) CloseComm(HWND hPort)
{
	CloseHandle(hPort);
}

const char *SERIAL_PORTS[] = {"\\\\.\\COM1", "\\\\.\\COM2", "\\\\.\\COM3", "\\\\.\\COM4", "\\\\.\\COM5",
							  "\\\\.\\COM6", "\\\\.\\COM7", "\\\\.\\COM8", "\\\\.\\COM9", "\\\\.\\COM10",
							  "\\\\.\\COM11", "\\\\.\\COM12", "\\\\.\\COM13", "\\\\.\\COM14", "\\\\.\\COM15",
							  "\\\\.\\COM16", "\\\\.\\COM17", "\\\\.\\COM18", "\\\\.\\COM19", "\\\\.\\COM20",
							  "\\\\.\\COM21", "\\\\.\\COM22", "\\\\.\\COM23", "\\\\.\\COM24", "\\\\.\\COM25",
							  "\\\\.\\COM26", "\\\\.\\COM27", "\\\\.\\COM28", "\\\\.\\COM29", "\\\\.\\COM30",
							  "\\\\.\\COM31", "\\\\.\\COM32", "\\\\.\\COM33", "\\\\.\\COM34", "\\\\.\\COM35",
							  "\\\\.\\COM36", "\\\\.\\COM37", "\\\\.\\COM38", "\\\\.\\COM39", "\\\\.\\COM40",
							  "\\\\.\\COM41", "\\\\.\\COM42", "\\\\.\\COM43", "\\\\.\\COM44", "\\\\.\\COM45",
							  "\\\\.\\COM46", "\\\\.\\COM47", "\\\\.\\COM48", "\\\\.\\COM49", "\\\\.\\COM50",
							  "\\\\.\\COM51", "\\\\.\\COM52", "\\\\.\\COM53", "\\\\.\\COM54", "\\\\.\\COM55",
							  "\\\\.\\COM56", "\\\\.\\COM57", "\\\\.\\COM58", "\\\\.\\COM59", "\\\\.\\COM60",
							  "\\\\.\\COM61", "\\\\.\\COM62", "\\\\.\\COM63", "\\\\.\\COM64", "\\\\.\\COM65",
							  "\\\\.\\COM66", "\\\\.\\COM67", "\\\\.\\COM68", "\\\\.\\COM69", "\\\\.\\COM70",
							  "\\\\.\\COM71", "\\\\.\\COM72", "\\\\.\\COM73", "\\\\.\\COM74", "\\\\.\\COM75",
							  "\\\\.\\COM76", "\\\\.\\COM77", "\\\\.\\COM78", "\\\\.\\COM79", "\\\\.\\COM80",
							  "\\\\.\\COM81", "\\\\.\\COM82", "\\\\.\\COM83", "\\\\.\\COM84", "\\\\.\\COM85",
							  "\\\\.\\COM86", "\\\\.\\COM87", "\\\\.\\COM88", "\\\\.\\COM89", "\\\\.\\COM90",
							  "\\\\.\\COM91", "\\\\.\\COM92", "\\\\.\\COM93", "\\\\.\\COM94", "\\\\.\\COM95",
							  "\\\\.\\COM96", "\\\\.\\COM97", "\\\\.\\COM98", "\\\\.\\COM99", "\\\\.\\COM100"};

int APIENTRY __declspec(dllexport) GetAvailableCom(char *ncom)
{
	HWND handle;
	int i = 0, j = 0;

	for (i = 0; i < sizeof(SERIAL_PORTS) / sizeof(char *); i++)
	{
		handle = CreateFile(SERIAL_PORTS[i], GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
		if (handle != INVALID_HANDLE_VALUE)
		{
			*(ncom + j) = i + 1;
			j++;
			CloseHandle(handle);
		}
	}

	return j;
}

void ShowErrorMessage(int hresult)
{
	LPTSTR errorText = NULL;
	char errNumber[10];
	char textErrNumber[128];

	strcpy(textErrNumber, "ErrNumber: ");

	itoa(hresult, errNumber, 10);

	strcat(textErrNumber, errNumber);

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
				  NULL,
				  hresult,
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				  (LPTSTR)&errorText,
				  0,
				  NULL);

	if (NULL != errorText)
	{
		MessageBox(NULL, errorText, textErrNumber, MB_ICONERROR);
		LocalFree(errorText);
		errorText = NULL;
	}
}
