#include <windows.h>
#include <commctrl.h>
#include "resource.h"


#define IDC_MAIN_EDIT 103

typedef struct
{
    char data[10];
} StateInfo;

StateInfo *GetAppState(HWND hwnd)
{
    LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
    StateInfo *pState = (StateInfo *)(ptr);
    return pState;
}

LPSTR ReadTextFromEdit(HWND hEdit)
{
    DWORD dwTextLength;
    dwTextLength = GetWindowTextLength(hEdit);
    if (dwTextLength > 0)
    {
        LPSTR editText;
        DWORD dwBufferSize = dwTextLength + 1;

        editText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
        if (editText != NULL)
        {
            if (GetWindowText(hEdit, editText, dwBufferSize))
            {
                return editText;
            }
        }
    }
    return NULL;
}
BOOL SaveTextFile(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile;
    BOOL bSuccess = FALSE;

    hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwTextLength;

        dwTextLength = GetWindowTextLength(hEdit);
        if (dwTextLength > 0)
        {
            LPSTR pszText;
            DWORD dwBufferSize = dwTextLength + 1;

            pszText = (LPSTR)GlobalAlloc(GPTR, dwBufferSize);
            if (pszText != NULL)
            {
                if (GetWindowText(hEdit, pszText, dwBufferSize))
                {
                    DWORD dwWritten;

                    if (WriteFile(hFile, pszText, dwTextLength, &dwWritten, NULL))
                        bSuccess = TRUE;
                }
                GlobalFree(pszText);
            }
        }
        CloseHandle(hFile);
    }
    return bSuccess;
}

void OpenFileSaveDialog(HWND hwnd)
{
    OPENFILENAME ofn;
    char szFileName[MAX_PATH] = "";

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = szFileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = "txt";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if (GetSaveFileName(&ofn))
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SaveTextFile(hEdit, szFileName);
    }
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    StateInfo *pState;
    if (msg == WM_CREATE)
    {
        CREATESTRUCT *pCreate = (CREATESTRUCT *)(lParam);
        pState = (StateInfo *)(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);

        // Add Edit Control
        HFONT hfDefault;
        HWND hEdit;
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, // Optional window styles.
            "EDIT",           // Predefined class; Edit.
            "Default text",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, LOWORD(lParam), HIWORD(lParam),
            hwnd,
            (HMENU)IDC_MAIN_EDIT,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);
        if (hEdit == NULL)
            MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

        HFONT hFont = CreateFont(
            20, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

        // // Add Button control
        // HWND hButton = CreateWindowEx(
        //     0,                                                     // Optional window styles.
        //     "BUTTON",                                              // Predefined class; Button.
        //     "Save",                                            // Button text.
        //     WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, // Styles.
        //     350,                                                   // x position.
        //     50,                                                    // y position.
        //     100,                                                   // Button width.
        //     50,                                                    // Button height.
        //     hwnd,                                                  // Parent window.
        //     (HMENU)ID_MYBUTTON,                                    // Button ID.
        //     (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        //     NULL); // Pointer not needed.
        // if (hButton == NULL)
        //     MessageBox(hwnd, "Could not create button.", "Error", MB_OK | MB_ICONERROR);
    }
    else
    {
        pState = GetAppState(hwnd);
    }

    switch (msg)
    {
    case WM_SIZE:
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        if (hEdit)
        {
            MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
        }
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_SAVE_BUTTON:
            OpenFileSaveDialog(hwnd);
            break;
        case ID_FILE_ABOUT:
            MessageBox(hwnd, "About menu item clicked", "Notice", MB_OK);
            break;
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_CLOSE:
        if (MessageBox(hwnd, "Really quit?", "My application", MB_YESNO) == IDYES)
        {
            DestroyWindow(hwnd);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// void addMenu(HWND hwnd){
//     // create menu bar
//     HMENU hMenubar = CreateMenu();
//     // main menu
//     HMENU hMenu = CreateMenu();

//     // menu items
//     AppendMenu(hMenu, MF_STRING, ID_FILE_ABOUT, "About");
//     AppendMenu(hMenu, MF_STRING, ID_FILE_EXIT, "Exit");
//     AppendMenu(hMenubar, MF_POPUP, (UINT_PTR)hMenu, "File");

//     // attach menu bar to the window
//     SetMenu(hwnd, hMenubar);

// }

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIstance, LPSTR lpCmdLine, int nCmdShow)
{

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON));
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = MAKEINTRESOURCE(IDR_MYMENU);
    wc.lpszClassName = "WindowClass";
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MYICON), IMAGE_ICON, 32, 32, 0);

    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    StateInfo *pState = (StateInfo *)GlobalAlloc(GMEM_FIXED, sizeof(StateInfo));
    CopyMemory(pState->data, "myAppData", 9);
    if (pState == NULL)
    {
        return 0;
    }

    HWND hwnd;
    hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        "WindowClass",       // Window class
        "My window",         // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT,       // Position X
        CW_USEDEFAULT,       // Position Y
        800,                 // Width
        600,                 // Height
        NULL,                // Parent window
        NULL,                // Menu
        hInstance,           // Instance handle
        pState               // Additional application data
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, "Create Window Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES; // Enables a set of common controls.
    InitCommonControlsEx(&icex);

    // addMenu(hwnd);
    ShowWindow(hwnd, nCmdShow);

    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}