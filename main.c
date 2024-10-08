#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <commctrl.h>
#include "resource.h"

#define IDC_MAIN_EDIT 103
#define BUFFER_SIZE 1024
typedef struct
{
    char filepath[MAX_PATH];
    char filename[MAX_PATH];
    LOGFONT font;
    BOOL hasUnsavedChanges;
} File_State;

typedef struct
{
    HWND hwnd;
    File_State *fileState;
    HINSTANCE hInstance;
    HWND hWndEdit;
    HICON hIcon;
    HFONT hFont;
} Window_State;

Window_State WIN;

void getFileNameWithoutExtension(const char *filePath, char *fileNameWithoutExt, size_t maxLen)
{

    const char *lastBackslash = strrchr(filePath, '\\');
    if (!lastBackslash)
    {
        lastBackslash = filePath;
    }
    else
    {
        lastBackslash++;
    }

    const char *lastPeriod = strrchr(lastBackslash, '.');
    if (!lastPeriod)
    {
        lastPeriod = lastBackslash + strlen(lastBackslash);
    }

    size_t length = lastPeriod - lastBackslash;
    if (length >= maxLen)
    {
        length = maxLen - 1;
    }

    strncpy(fileNameWithoutExt, lastBackslash, length);
    fileNameWithoutExt[length] = '\0';
}
void HandleEditChange(WPARAM wParam)
{
    if (WIN.fileState->filename[0] != '*')
    {
        char newStr[MAX_PATH + 1];
        snprintf(newStr, sizeof(newStr), "*%s", WIN.fileState->filename);
        SetWindowText(WIN.hwnd, newStr);
        strcpy(WIN.fileState->filename, newStr);
        WIN.fileState->hasUnsavedChanges = TRUE;
    }
}
LPSTR ReadTextFromEdit(HWND hEdit)
{
    DWORD dwTextLength;
    dwTextLength = GetWindowTextLength(hEdit);
    if (dwTextLength > 0)
    {
        LPSTR editText = (LPSTR)GlobalAlloc(GPTR, dwTextLength + 1);
        if (editText != NULL)
        {
            if (GetWindowText(hEdit, editText, dwTextLength + 1))
            {
                return editText;
            }
            GlobalFree(editText);
        }
    }
    return NULL;
}

BOOL SaveTextFile(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        LPSTR pszText = ReadTextFromEdit(hEdit);
        if (pszText != NULL)
        {

            DWORD dwWritten;
            BOOL bSuccess = WriteFile(hFile, pszText, strlen(pszText), &dwWritten, NULL);
            GlobalFree(pszText);
            CloseHandle(hFile);

            char filetitle[MAX_PATH];
            strcpy(WIN.fileState->filepath, pszFileName);
            getFileNameWithoutExtension(pszFileName, filetitle, sizeof(filetitle));
            SetWindowText(WIN.hwnd, filetitle);
            strcpy(WIN.fileState->filename, filetitle);
            WIN.fileState->hasUnsavedChanges = FALSE;
            GlobalFree(pszText);
            return bSuccess;
        }
        CloseHandle(hFile);
    }
    return FALSE;
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
int fileExists(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void Savefile(HWND hwnd)
{
    if (fileExists(WIN.fileState->filepath))
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SaveTextFile(hEdit, WIN.fileState->filepath);
    }
    else
    {
        OpenFileSaveDialog(hwnd);
    }
}

BOOL ShowTextFile(HWND hEdit, LPCTSTR pszFileName)
{
    HANDLE hFile = CreateFile(pszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE)
    {
        DWORD dwBytesRead;
        char buffer[BUFFER_SIZE] = {0};
        if (ReadFile(hFile, buffer, BUFFER_SIZE - 1, &dwBytesRead, NULL))
        {
            SetWindowText(hEdit, buffer);
            CloseHandle(hFile);

            char filetitle[MAX_PATH];
            getFileNameWithoutExtension(pszFileName, filetitle, sizeof(filetitle));
            strcpy(WIN.fileState->filepath, pszFileName);
            SetWindowText(WIN.hwnd, filetitle);
            strcpy(WIN.fileState->filename, filetitle);
            WIN.fileState->hasUnsavedChanges = FALSE;
            return TRUE;
        }
        CloseHandle(hFile);
    }
    return FALSE;
}
void Openfile(HWND hwnd)
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
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn))
    {
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        ShowTextFile(hEdit, szFileName);
    }
}
void Changefont(HWND hwnd)
{
    CHOOSEFONT cf;
    LOGFONT lf;
    HFONT hFont;
    ZeroMemory(&cf, sizeof(cf));
    cf.lStructSize = sizeof(CHOOSEFONT);
    cf.hwndOwner = hwnd;
    cf.lpLogFont = &lf;
    cf.Flags = CF_SCREENFONTS | CF_EFFECTS;

    if(ChooseFont(&cf)){
        hFont = CreateFontIndirect(cf.lpLogFont);
        HWND hEdit = GetDlgItem(hwnd, IDC_MAIN_EDIT);
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));
    }

    
    
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_CREATE:
    {
        // Add Edit Control
        HFONT hfDefault;
        HWND hEdit;
        hEdit = CreateWindowEx(
            WS_EX_CLIENTEDGE, // Optional window styles.
            "EDIT",           // Predefined class; Edit.
            "",
            WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
            0, 0, LOWORD(lParam), HIWORD(lParam),
            hwnd,
            (HMENU)IDC_MAIN_EDIT,
            (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
            NULL);

        if (hEdit == NULL)
            MessageBox(hwnd, "Could not create edit box.", "Error", MB_OK | MB_ICONERROR);

        HFONT hFont = CreateFont(
            20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_MODERN, "Consolas");
        SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));

        WIN.fileState = (File_State *)malloc(sizeof(File_State));
        if (WIN.fileState == NULL)
        {
            MessageBox(hwnd, "Could not allocate memory for file state.", "Error", MB_OK | MB_ICONERROR);
        }
        strcpy(WIN.fileState->filename, "Untitled");
        LOGFONT logfont;
        GetObject(hFont, sizeof(LOGFONT), &logfont);
        WIN.fileState->font = logfont;
        WIN.fileState->hasUnsavedChanges = FALSE;
        WIN.hWndEdit = hEdit;
        break;
    }
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

        case IDC_MAIN_EDIT:
        {
            if (HIWORD(wParam) == EN_CHANGE)
            {
                if (WIN.fileState && !WIN.fileState->hasUnsavedChanges)
                {
                    HandleEditChange(wParam);
                    WIN.fileState->hasUnsavedChanges = TRUE;
                }
            }
        }
        break;
        case ID_FILE:
            Openfile(hwnd);
            break;
        case ID_SAVE_BUTTON:
            Savefile(hwnd);
            break;
        case ID_FONT:
            Changefont(hwnd);
            break;
        case ID_FILE_ABOUT:
            MessageBox(hwnd, "A straightforward, no-frills text editor for quick note-taking and editing.", "TextPad", MB_OK);
            break;
        case ID_FILE_EXIT:
            PostQuitMessage(0);
            break;
        }
        break;
    case WM_CLOSE:
    {
        if (WIN.fileState->hasUnsavedChanges)
        {
            int res = MessageBox(hwnd, "Do you want to save changes?", "Notepad", MB_YESNOCANCEL | MB_ICONQUESTION);
            if (res == IDYES)
            {
                Savefile(hwnd);
                DestroyWindow(hwnd);
            }
            else if (res == IDNO)
            {
                DestroyWindow(hwnd);
            }
            // If res is IDCANCEL, do nothing, just return to avoid closing the window
        }
        else
        {
            DestroyWindow(hwnd);
        }
        break;
    }
    case WM_DESTROY:
        if (WIN.fileState)
        {
            free(WIN.fileState);
        }
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

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

    WIN.hInstance = hInstance;

    WIN.hwnd = CreateWindowEx(
        0,                   // Optional window styles.
        "WindowClass",       // Window class
        "Untitled",          // Window text
        WS_OVERLAPPEDWINDOW, // Window style
        CW_USEDEFAULT,       // Position X
        CW_USEDEFAULT,       // Position Y
        800,                 // Width
        600,                 // Height
        NULL,                // Parent window
        NULL,                // Menu
        hInstance,           // Instance handle
        NULL                 // Additional application data
    );

    if (WIN.hwnd == NULL)
    {
        MessageBox(NULL, "Create Window Failed!", "Error!",
                   MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_STANDARD_CLASSES;
    InitCommonControlsEx(&icex);

    ShowWindow(WIN.hwnd, nCmdShow);

    UpdateWindow(WIN.hwnd);

    // Message Loop:
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}