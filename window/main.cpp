#include <windows.h>
#include "resource.h"
#include <string>

HINSTANCE g_hInst = NULL;

INT_PTR CALLBACK CustomMsgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_INITDIALOG:
        SetWindowTextA(hDlg, "Reminder");
        SetDlgItemTextA(hDlg, IDC_TEXT, (LPCSTR)lParam);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

int ShowCustomMessageBox(HWND parent, const std::string& text)
{
    return DialogBoxParamA(
        g_hInst,
        MAKEINTRESOURCEA(IDD_CUSTOMMSG),
        parent,
        CustomMsgProc,
        (LPARAM)text.c_str()
    );
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int)
{
    g_hInst = hInst;

    ShowCustomMessageBox(NULL, "This is your fully custom message box!");

    return 0;
}
