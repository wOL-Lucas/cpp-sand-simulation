#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <vector>
#include <random>

const int sandSize = 5;
int gridWidth;
int gridHeight;

std::vector<std::vector<bool>> grid;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(0, 1);


void initializeGrid(int width, int height) {
  gridWidth = width / sandSize;
  gridHeight = height / sandSize;
  grid.resize(gridHeight, std::vector<bool>(gridWidth, false));
}

void updateGrid() {
  for (int y = gridHeight - 1; y >= 0; y--) {
    for (int x = 0; x < gridWidth; x++) {
      
      if (grid[y][x] && y + 1 < gridHeight) {
        if (!grid[y + 1][x]) {
          grid[y + 1][x] = true;
          grid[y][x] = false;
        }

        else if(y + 1 < gridHeight && grid[y + 1][x]){
          
          if(x - 1 >= 0 && x + 1 < gridWidth && !grid[y + 1][x - 1] && !grid[y + 1][x + 1] && y + 1 < gridHeight){
    
            int random =  dis(gen);
            if(random > 0.5){
              grid[y][x] = false;
              grid[y + 1][x - 1] = true;
            } else {
              grid[y][x] = false;
              grid[y + 1][x + 1] = true;
            }

          }

          else if(x + 1 < gridWidth && !grid[y + 1][x + 1]) {
            grid[y][x] = false;
            grid[y + 1][x + 1] = true;

          } else if (x - 1 >= 0 && !grid[y + 1][x - 1]){
            grid[y][x] = false;
            grid[y + 1][x - 1] = true;
          }
        }
      }
    }
  }
}

int createSand(HWND hwnd, LPARAM lParam){
  int x = GET_X_LPARAM(lParam);
  int y = GET_Y_LPARAM(lParam);

  if(x < 0 || y < 0){
    return 0;
  }

  int gridX = x / sandSize;
  int gridY = y / sandSize;

  if (gridX < 0 || gridX > gridWidth || gridY < 0 || gridY > gridHeight) {
    return 0;
  }

  grid[gridY][gridX] = true;
  updateGrid();

  InvalidateRect(hwnd, NULL, FALSE);
  return 0;

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    updateGrid();
    switch (uMsg)
    {

    case WM_CREATE:
    {
        SetTimer(hwnd, 100, 1, NULL);
        RECT rect;
        GetClientRect(hwnd, &rect);
        initializeGrid(rect.right, rect.bottom);
        return 0;
    }

    case WM_TIMER:
    {
        updateGrid();
        InvalidateRect(hwnd, NULL, FALSE);
        return 0;
    }

    case WM_LBUTTONDOWN:
    {
        createSand(hwnd, lParam);
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        if (wParam & MK_LBUTTON) {
          createSand(hwnd, lParam);
        }
        return 0;
    }

    case WM_PAINT:
    {
        //This init the drawing context...
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        HDC hdcMem = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        
        SelectObject(hdcMem, hbmMem);

        for (int y = 0; y < gridHeight; y++) {
          for (int x = 0; x < gridWidth; x++) {
            RECT rect;
            rect.left = x * sandSize;
            rect.top = y * sandSize;
            rect.right = rect.left + sandSize;
            rect.bottom = rect.top + sandSize;

            if (grid[y][x]) {
                FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else {
              FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
            }
          }
        }

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hdcMem, 0, 0, SRCCOPY);

        DeleteObject(hbmMem);
        DeleteDC(hdcMem);

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}


int createWindow(HINSTANCE hInstance, int nCmdShow) {
    const char CLASS_NAME[] = "SandSimulationWindow";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Sand Simulation",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 600,     
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
