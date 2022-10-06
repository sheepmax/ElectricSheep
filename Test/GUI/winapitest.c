#include <windows.h>
#include <math.h>
#include <wingdi.h>
#include <stdio.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))


LRESULT CALLBACK WindowProc(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
);

LRESULT CALLBACK WindowProcChild(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
);

LRESULT CALLBACK WindowProcGrid(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
);

BOOL CALLBACK EnumChildProc(
    HWND hwnd,
    LPARAM lParamvoid
);

void AddMenus(HWND hwnd);

HINSTANCE hInst;

#define NUM_BUTTONS 20
HWND buttons[NUM_BUTTONS];
BOOL scrollBarShowing = FALSE;
int scrollBarWidth; 
int currentScrollPos = 0;
int currentWindowHeight;
int currentWindowWidth;

#define SELECTION_AREA_ID 10
#define GRID_AREA_ID 20

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    scrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
    hInst = hInstance;
    WNDCLASS wc = {0};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = "TestClass";
    wc.hCursor = LoadCursor(NULL, IDC_HAND);

    RegisterClass(&wc);

    WNDCLASS wc2 = {0};

    wc2.lpfnWndProc = WindowProcChild;
    wc2.hInstance = hInstance;
    wc2.lpszClassName = "TestClass2";
    wc2.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc2);

    WNDCLASS wcGrid = {0};

    wcGrid.lpfnWndProc = WindowProcGrid;
    wcGrid.hInstance = hInstance;
    wcGrid.lpszClassName = "GridClass";

    RegisterClass(&wcGrid);

    HWND hwnd = CreateWindowEx(
        0,
        "TestClass", 
        "Test Window API Doc",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        NULL,
        NULL,
        hInst,
        NULL);

    if (!hwnd) 
    {
        printf("Failed to create window.\n");
        return 0;
    }
   
    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) 
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
) 
{
    switch (uMsg)
    {
        case WM_CREATE:
        {
            RECT r = {};
            GetWindowRect(hwnd, &r);

            currentWindowHeight = r.bottom - r.top;
            currentWindowWidth = r.right - r.left;

         // printf("%d, %d, %d, %d\n", r.left, r.top, r.right, r.bottom);
            HWND selectionArea = CreateWindowEx(
                0,
                "TestClass2",
                NULL,
                WS_BORDER | WS_VISIBLE | WS_CHILD,
                0, 0, 150, currentWindowHeight,
                hwnd,
                (HMENU)SELECTION_AREA_ID,
                hInst,
                NULL);

            HWND gridArea = CreateWindowEx(
                0,
                "GridClass",
                NULL,
                WS_BORDER | WS_VISIBLE | WS_CHILD,
                150, 0, (currentWindowWidth - 150), currentWindowHeight,
                hwnd,
                (HMENU)GRID_AREA_ID,
                hInst,
                NULL);

            ShowWindow(gridArea, SW_SHOW);

            char number_string[2];

            for (int i = 0; i < NUM_BUTTONS; i++)
            {
                sprintf(number_string, "%d", (i + 1));
                buttons[i] = CreateWindowEx(
                    0,
                    "BUTTON",
                    number_string,
                    WS_CHILD | WS_VISIBLE,
                    10, (60 * i + 10), 130, 50,
                    selectionArea,
                    NULL,
                    hInst,
                    NULL);
            }

            AddMenus(hwnd);
            break;
        }
        case WM_SIZE:
        {
            UINT height = HIWORD(lParam);
            UINT width = LOWORD(lParam);
            int deltaHeight = currentWindowHeight - height;
            currentWindowHeight = height;
            currentWindowWidth = width;
            EnumChildWindows(hwnd, EnumChildProc, (LPARAM)deltaHeight);
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CLOSE:
            if (MessageBox(hwnd, "Quit for realz?", "Bye :(", MB_OKCANCEL) == IDOK)
            {
                DestroyWindow(hwnd);
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        default: 
            DefWindowProc(hwnd, uMsg, wParam, lParam);
            break;
    }
}

LRESULT CALLBACK WindowProcChild(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
) 
{
    switch (uMsg)
    {
        case WM_VSCROLL:
        {
            UINT request = LOWORD(wParam);
            int prevScrollPos = currentScrollPos;
            switch (request)
            {
                case SB_LINEUP:
                    currentScrollPos--; break;
                case SB_LINEDOWN:
                    currentScrollPos++; break;
                case SB_PAGEUP:
                    currentScrollPos -= currentWindowHeight; break;
                case SB_PAGEDOWN:
                    currentScrollPos += currentWindowHeight; break;
                case SB_THUMBPOSITION: case SB_THUMBTRACK:
                    currentScrollPos = HIWORD(wParam); break;
            }

            currentScrollPos = MAX(MIN(currentScrollPos, (NUM_BUTTONS * 60 + 10)), 0);
            SCROLLINFO info = {sizeof(SCROLLINFO), SIF_POS, 0, 0, 0, currentScrollPos, 0};
            SetScrollInfo(hwnd, SB_VERT, &info, TRUE);

            ScrollWindowEx(hwnd, 0, prevScrollPos - currentScrollPos, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE);
            UpdateWindow(hwnd);

        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)CreateSolidBrush(RGB(100, 100, 100)));
            EndPaint(hwnd, &ps);
            return 0;
        }
        default: 
            DefWindowProc(hwnd, uMsg, wParam, lParam);
            break;
    }
}

LRESULT CALLBACK WindowProcGrid(
  HWND   hwnd,
  UINT   uMsg,
  WPARAM wParam,
  LPARAM lParam
)
{
    switch(uMsg)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;

            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 3));
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            DefWindowProc(hwnd, uMsg, wParam, lParam);
            break;
    }
}

BOOL CALLBACK EnumChildProc(
    HWND hwndChild,
    LPARAM lParam
)
{
    int idChild = GetDlgCtrlID(hwndChild);
    int new_height = currentWindowHeight;

    if (idChild == SELECTION_AREA_ID)
    {
        BOOL height_greater = new_height > (NUM_BUTTONS * 60 + 10);

        if (height_greater && scrollBarShowing)
        {
            ShowScrollBar(hwndChild, SB_VERT, scrollBarShowing = FALSE);
            ScrollWindowEx(hwndChild, 0, currentScrollPos, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE);
            currentScrollPos = 0;
            UpdateWindow(hwndChild);
        }
        else if (!height_greater && !scrollBarShowing)
        {
            ShowScrollBar(hwndChild, SB_VERT, scrollBarShowing = TRUE);
        }

        SetWindowPos(hwndChild, NULL, 0, 0, 150 + scrollBarWidth * scrollBarShowing, new_height, SWP_NOZORDER | SWP_NOMOVE | SWP_DEFERERASE);

        // Show scrollbar when elements are offscreen
       
        if (scrollBarShowing)
        {
            int deltaHeight = MAX((int)lParam, -currentScrollPos);

            // Readjust scroll when resizing window
            {
                currentScrollPos += deltaHeight;
                ScrollWindowEx(hwndChild, 0, -deltaHeight, NULL, NULL, NULL, NULL, SW_SCROLLCHILDREN | SW_INVALIDATE);
                UpdateWindow(hwndChild);
            }
            // Change scroll bar info and graphic
            SCROLLINFO info = {sizeof(SCROLLINFO), SIF_ALL, 0, (NUM_BUTTONS * 60 + 10), new_height, currentScrollPos, 0};
            SetScrollInfo(hwndChild, SB_VERT, &info, TRUE);
        }
    }
    else if (idChild == GRID_AREA_ID)
    {
        SetWindowPos(hwndChild, NULL, 150 + scrollBarWidth * scrollBarShowing, 0, currentWindowWidth - scrollBarWidth * scrollBarShowing - 150, new_height, SWP_NOZORDER | SWP_DEFERERASE);
    }
    return TRUE;
}

void AddMenus(HWND hwnd)
{
    HMENU hmenu = CreateMenu();
    AppendMenu(hmenu, MF_STRING, 1, "File");
    SetMenu(hwnd, hmenu);
}