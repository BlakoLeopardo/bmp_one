#include <windows.h>

// Protótipo da função que processa as mensagens da janela.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

// Variável global para armazenar o handle do bitmap.
HBITMAP hBitmap = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // 1. Registro da classe de janela.
    WNDCLASS wc = {0};
    wc.lpfnWndProc   = WndProc;                      // Função que processará as mensagens.
    wc.hInstance     = hInstance;                    // Instância atual do programa.
    wc.lpszClassName = "BitmapWindowClass";          // Nome da classe.
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);    // Cursor padrão.
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);       // Cor de fundo da janela.

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Falha ao registrar a classe da janela!", "Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // 2. Criação da janela.
    HWND hwnd = CreateWindowEx(
        0,                                // Estilos estendidos.
        "BitmapWindowClass",              // Nome da classe registrada.
        "Exemplo de Bitmap",              // Título da janela.
        WS_OVERLAPPEDWINDOW,              // Estilo da janela.
        CW_USEDEFAULT, CW_USEDEFAULT,     // Posição inicial.
        640, 480,                         // Largura e altura da janela.
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) {
        MessageBox(NULL, "Falha na criação da janela!", "Erro", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Exibe e atualiza a janela.
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // 3. Loop de mensagens.
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

// Função que processa as mensagens enviadas para a janela.
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_CREATE: {
            // WM_CREATE: Mensagem enviada durante a criação da janela.
            // Carrega o bitmap do arquivo "bitmap.bmp". Certifique-se de que o arquivo esteja no mesmo diretório do executável.
            hBitmap = (HBITMAP)LoadImage(NULL, "bitmap.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
            if (hBitmap == NULL) {
                MessageBox(hwnd, "Não foi possível carregar o bitmap!", "Erro", MB_OK | MB_ICONERROR);
            }
        }
        break;

        case WM_PAINT: {
            // WM_PAINT: Mensagem enviada quando a janela precisa ser redesenhada.
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            if (hBitmap) {
                // Cria um contexto de dispositivo (DC) de memória compatível com o DC da janela.
                HDC hdcMem = CreateCompatibleDC(hdc);
                // Seleciona o bitmap no DC de memória.
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);

                // Obtém as dimensões do bitmap.
                BITMAP bitmap;
                GetObject(hBitmap, sizeof(BITMAP), &bitmap);

                // Copia (bit-blt) o bitmap do DC de memória para o DC da janela.
                BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

                // Restaura o bitmap antigo e deleta o DC de memória.
                SelectObject(hdcMem, hOldBitmap);
                DeleteDC(hdcMem);
            }

            EndPaint(hwnd, &ps);
        }
        break;

        case WM_DESTROY: {
            // WM_DESTROY: Mensagem enviada quando a janela está sendo destruída.
            // Libera os recursos alocados para o bitmap.
            if (hBitmap) {
                DeleteObject(hBitmap);
            }
            PostQuitMessage(0); // Encerra o loop de mensagens.
        }
        break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}