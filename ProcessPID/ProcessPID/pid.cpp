#undef UNICODE
#include <windows.h>
#include <tlhelp32.h>
#include "resource.h"
#include <stdio.h>
#include <psapi.h>

#define MAX_ENV 2000
#pragma warning (disable : 4996)

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg,
	WPARAM wParam, LPARAM lParam);

BOOL dlgActive = FALSE;
HWND hwndMain;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static char szAppName[] = "Dialog";
	HWND        hwnd;
	MSG         msg;
	WNDCLASSEX  wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW); 
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);

	hwnd = CreateWindow(szAppName, //numele clasei inregistrat cu RegisterClass
		"Un program simplu",    // text pentru bara de titlu a ferestrei
		WS_OVERLAPPEDWINDOW,   // stilul ferestrei
		CW_USEDEFAULT,      // pozitia orizontala implicitã
		CW_USEDEFAULT,      // pozitia verticala implicita
		CW_USEDEFAULT,       // latimea implicita
		CW_USEDEFAULT,       // inaltimea implicita
		NULL,               // handle-ul ferestrei parinte
		NULL,               // handle-ul meniului ferestrei
		hInstance,          // proprietara ferestrei 
		NULL);


	SetWindowPos(hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE);
	// schimba dimensiunea, pozitia si ordinea z a ferestrei copil, a ferestrei pop-up
	ShowWindow(hwnd, SW_HIDE);
	UpdateWindow(hwnd);
	hwndMain = hwnd;



	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	static HINSTANCE hInstance;
	switch (iMsg)
	{
	case WM_CREATE: // operatiile ce se executa la crearea ferestrei
		// se creaza caseta de dialog
		hInstance = ((LPCREATESTRUCT)lParam)->hInstance;
		if (!dlgActive) {
			DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG),
				hwnd, DlgProc);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			// insereaza un nou mesaj nou in coada de asteptare
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0); // insereaza un mesaj de incheiere 
		return 0;
	}

	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}

/////////////////////////////////////////////////////////////

PROCESSENTRY32 proces;

HANDLE hProcessSnap;
HANDLE hProcess;
BOOL f;
HWND hwnd;
int ct = 0;


BOOL CALLBACK DlgProc(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam) {
	
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	proces.dwSize = sizeof(PROCESSENTRY32);

	char buffer[10000] = {};
	char VarPros[100];
	char lpBuffer_mesaj[2000];
	char lpBuffer_titlu[2000];
	TCHAR nameProc[MAX_PATH];
	

	switch (iMsg) {

	case WM_INITDIALOG:
		
		f = Process32First(hProcessSnap, &proces);
		
		//strcat_s(buffer, TEXT(proces.szExeFile));
		//strcat_s(buffer, "\r\n");
		ct++;
		
		SetDlgItemText(hDlg, IDC_EDIT_LIST, TEXT(proces.szExeFile));
		while (f)
		{
			f = Process32Next(hProcessSnap, &proces);
			strcat_s(buffer, TEXT(proces.szExeFile));
			strcat_s(buffer, "\r\n");
			ct++;
	
		}

		SetDlgItemText(hDlg, IDC_EDIT_LIST, buffer);
		SetDlgItemInt(hDlg, ID_CT, ct,TRUE);
		
		CloseHandle(hProcessSnap);
		return TRUE;

	case WM_CLOSE:
		dlgActive = FALSE;
		EndDialog(hDlg, 0);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_OK:
			GetDlgItemText(hDlg, IDC_EDIT, VarPros, 100); 

			f = Process32First(hProcessSnap, &proces);
			
			while (f)
				{
					if (strcmp(VarPros, TEXT(proces.szExeFile)) == 0)
					{
						sprintf_s(lpBuffer_mesaj, "PID: %d \nExecutabil: %s\nID Parinte %d\n", proces.th32ProcessID, proces.szExeFile, proces.th32ParentProcessID);
						sprintf_s(lpBuffer_titlu, "PROCESS");
						MessageBox(hDlg, lpBuffer_mesaj, lpBuffer_titlu, MB_OK);	
						break;
					}
					else 
					{
						f = Process32Next(hProcessSnap, &proces);
					}
				}
			return TRUE;
		
		case ID_CANCEL:
			dlgActive = FALSE;
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
