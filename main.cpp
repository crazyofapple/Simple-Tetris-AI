#include "teris.h"
#include <string.h>
#include <locale.h> 
#include "resource.h"
#include <Strsafe.h>
using namespace std;

#define ID_TIME 0

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT("Teris");
	MSG          msg;
	HWND hWnd;
	HMENU hMenu;

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc = (WNDPROC)WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(GetStockObject(WHITE_BRUSH));
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(hInstance, (LPCTSTR)IDI_APPLICATION);
	hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
	if (!RegisterClassEx(&wcex)) return FALSE;
	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE,
		szAppName,
		TEXT("Teris Demo"),
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		900,
		NULL,
		hMenu,
		hInstance,
		NULL);
	if (!hWnd) return FALSE;
	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
RECT rect;
void MoveAI(HWND hwnd, Teris &teris) {
	AI_OFFSET pos;
	pos = teris.AI(teris);
	while (teris.getNowShape() != pos.shape) {
		int shape = teris.getNowShape();
		SendMessage(hwnd, WM_KEYDOWN, VK_UP, 0);
		if (shape == teris.getNowShape()) return;
	}
	int X;
	while (teris.getNowPtX() != pos.offsetX) {
		X = teris.getNowPtX();
		if (X < pos.offsetX) {
			SendMessage(hwnd, WM_KEYDOWN, VK_RIGHT, 0);
		} else {
			SendMessage(hwnd, WM_KEYDOWN, VK_LEFT, 0);
		}
		if (X == teris.getNowPtX())//表示无法移动到计算的位置
			break;
	}
	int Y;
	while (teris.getNowPtY() != pos.offsetY)
	{
		Y = teris.getNowPtY();
		SendMessage(hwnd, WM_KEYDOWN, VK_DOWN, 0);
		if (Y == teris.getNowPtY())//表示无法移动到计算的位置，翻转后Y轴位置改变
			break;
	}

}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	rect.bottom = 18;
	rect.left = 0;
	rect.right = 12;
	rect.top = 0;
	HDC hdc;
	PAINTSTRUCT ps;
	static Teris teris(rect);
	static bool fState = FALSE;
	static bool fCheck = FALSE;
	static int cX, cY;

	switch (message) {
	case WM_CREATE:
		SetTimer(hwnd, ID_TIME, 500, NULL);
		MoveAI(hwnd, teris);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_FILE_RESTART:
			teris.InitGame();
			InvalidateRect(hwnd, NULL, TRUE);
			SetTimer(hwnd, ID_TIME, 500, NULL);
			MoveAI(hwnd, teris);
			return 0;
		}
		return 0;
	case WM_ERASEBKGND:
		return TRUE;
	case WM_SIZE:
		cX = LOWORD(lParam);
		cY = HIWORD(lParam);
		return 0;
	case WM_TIMER:
		SendMessage(hwnd, WM_KEYDOWN, VK_DOWN, 0);
		return 0;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_DOWN:
			if (teris.Down()) {
				fState = true;
			} else {
				SendMessage(hwnd, WM_KEYUP, 0, 0);
				if (fCheck) {
					teris.Check();
					fState = true;
					if (!teris.GetGameState()) {
						MoveAI(hwnd, teris);
					}
				}
			}
			break;
		case VK_UP:
			if (teris.Up()) {
				fState = true;
			}
			break;
		case VK_LEFT:
			if (teris.Left()) {
				fCheck = false;
				fState = true;
			}
			break;
		case VK_RIGHT:
			if (teris.Right())
			{
				fCheck = false;
				fState = true;
			}
			break;
		default:
			break;
		}
		if (fState)
		{
			InvalidateRect(hwnd, NULL, TRUE);
			fState = false;
		}
		return 0;
	case WM_KEYUP:
		if (!fCheck)
		{
			fCheck = true;
		}
		return 0;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		HBITMAP hBitMap;
		HDC hdcBack = CreateCompatibleDC(hdc);
		hBitMap = CreateCompatibleBitmap(hdc, cX, cY);
		SelectObject(hdcBack, hBitMap);
		if (teris.GetGameState()) {
			SetBkMode(hdcBack, TRANSPARENT);
			SetTextColor(hdcBack, RGB(250, 135, 250));
			TextOut(hdcBack, cX / 2, cY / 2, TEXT("Game Over"), 9);
			KillTimer(hwnd, ID_TIME);
		} else {
			SelectObject(hdcBack, CreatePen(PS_SOLID, 1, RGB(24, 105, 173)));
			SelectObject(hdcBack, CreateSolidBrush(RGB(255, 255, 255)));
			Rectangle(hdcBack, 0, 0, (rect.right) * 40, (rect.bottom) * 40);

			DeleteObject(SelectObject(hdcBack, CreateSolidBrush(RGB(13, 135, 239))));
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (teris.ptr->pos[i][j]) {
						Rectangle(hdcBack, (teris.nowPt.x + j) * 40, (teris.nowPt.y + i) * 40,
							(teris.nowPt.x + j) * 40 + 40, (teris.nowPt.y + i) * 40 + 40);
					}
				}
			}
			for (int i = 0; i < rect.bottom - rect.top; i++) {
				for (int j = 0; j < rect.right - rect.left; j++) {
					if (teris.pDC[i][j]) {
						Rectangle(hdcBack, j * 40, i * 40, j * 40 + 40, i * 40 + 40);
					}
				}
			}
			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					if (teris.nextPtr->pos[i][j]) {
						Rectangle(hdcBack, j * 40 + 550, i * 40 + 80, j * 40 + 40 + 550, i * 40 + 40 + 80);
					}
				}
			}
			DeleteObject(SelectObject(hdcBack, (HPEN)GetStockObject(BLACK_PEN)));
			DeleteObject(SelectObject(hdcBack, (HBRUSH)GetStockObject(WHITE_BRUSH)));
			SetBkMode(hdcBack, TRANSPARENT);
			SetTextColor(hdcBack, RGB(250, 135, 250));
			TextOut(hdcBack, 500, 50, TEXT("下一方块："), 5);
			TCHAR score[20];
			size_t cbDest = 20 * sizeof(TCHAR);
			StringCbPrintf(score, cbDest, TEXT("当前状态: %2u分"),(uint32_t)teris.GetScore());
			TextOut(hdcBack, 500, 250, score, strlen((char*)score));
		}
		BitBlt(hdc, 0, 0, cX, cY,
			hdcBack, 0, 0, SRCCOPY);
		DeleteObject(hBitMap);
		DeleteDC(hdcBack);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}
