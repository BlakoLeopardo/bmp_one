#include <windows.h>
#include <stdio.h>
#include <tchar.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
HBITMAP hBitmap = NULL;

// Função para exibir mensagens de erro detalhadas
void ShowError(HWND hwnd, const char* action) {
    DWORD error = GetLastError();
    LPVOID errorMsg;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&errorMsg,
        0,
        NULL
    );
    char msg[512];
    snprintf(msg, sizeof(msg), "%s falhou.\nCódigo: %lu\nDescrição: %s", action, error, (char*)errorMsg);
    MessageBox(hwnd, msg, "Erro", MB_ICONERROR | MB_OK);
    LocalFree(errorMsg);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "BitmapWindowClass";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    if (!RegisterClass(&wc)) {
        ShowError(NULL, "Registro da classe");
        return 1;
    }

    HWND hwnd = CreateWindowEx(0, "BitmapWindowClass", "Exibidor de Bitmap", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, hInstance, NULL);

    if (!hwnd) {
        ShowError(NULL, "Criação da janela");
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            // Usando o diretório "C:\BitMap" para carregar o bitmap
            const char* bitmapPath = "C:\\BitMap\\bitmap.bmp";
            hBitmap = (HBITMAP)LoadImage(NULL, bitmapPath, IMAGE_BITMAP, 0, 0, 
                LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_VGACOLOR);

            if (!hBitmap) {
                ShowError(hwnd, "Carregar bitmap");
                InvalidateRect(hwnd, NULL, TRUE);
            } else {
                BITMAP bm;
                if (!GetObject(hBitmap, sizeof(BITMAP), &bm)) {
                    ShowError(hwnd, "Ler dimensões");
                    DeleteObject(hBitmap);
                    hBitmap = NULL;
                }
            }
            break;
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            
            if (hBitmap) {
                HDC hdcMem = CreateCompatibleDC(hdc);
                if (hdcMem) {
                    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBitmap);
                    BITMAP bm;
                    if (GetObject(hBitmap, sizeof(BITMAP), &bm)) {
                        BitBlt(hdc, 10, 10, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
                    } else {
                        ShowError(hwnd, "Obter informações");
                    }
                    SelectObject(hdcMem, hOld);
                    DeleteDC(hdcMem);
                } else {
                    ShowError(hwnd, "Criar DC");
                }
            } else {
                TextOut(hdc, 10, 10, "BITMAP NÃO CARREGADO!", 19);
                Rectangle(hdc, 9, 9, 200, 30);
            }
            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY: {
            if (hBitmap && !DeleteObject(hBitmap)) {
                ShowError(hwnd, "Deletar bitmap");
            }
            PostQuitMessage(0);
            break;
        }

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}