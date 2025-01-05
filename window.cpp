#include <windows.h>
#include <winuser.h>
#include <windowsx.h>
#include <vector>


const int sandSize = 10;
int gridWidth;
int gridHeight;

std::vector<std::vector<bool>> grid;


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

        else if(y + 2 < gridHeight && grid[y + 2][x]){
          if(x + 1 < gridWidth) {
            if(!grid[y + 1][x + 1]) {
              grid[y][x] = false;
              grid[y + 1][x + 1] = true;
            }
          } else if (x - 1 < gridWidth){
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

  int gridX = x / sandSize;
  int gridY = y / sandSize;

  
  if (gridX < 0 || gridX >= gridWidth || gridY < 0 || gridY >= gridHeight) {
    return 0;
  }

  grid[gridY][gridX] = true;
  updateGrid();

  InvalidateRect(hwnd, NULL, FALSE);

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
        if(wParam & MK_LBUTTON) {
          createSand(hwnd, lParam);
        }

        return 0;
    }

    case WM_PAINT:
    {
        //This init the drawing context...
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        for (int y = 0; y < gridHeight; y++) {
          for (int x = 0; x < gridWidth; x++) {
            if (grid[y][x]) {
              RECT rect;
              rect.left = x * sandSize;
              rect.top = y * sandSize;
              rect.right = rect.left + sandSize;
              rect.bottom = rect.top + sandSize;
              FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            }
            else {
              RECT rect;
              rect.left = x * sandSize;
              rect.top = y * sandSize;
              rect.right = rect.left + sandSize;
              rect.bottom = rect.top + sandSize;
              FillRect(hdc, &rect, (HBRUSH)GetStockObject(WHITE_BRUSH));
            }
          }
        }

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
