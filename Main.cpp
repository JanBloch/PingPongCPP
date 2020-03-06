#include <Windows.h>
#include <chrono>

#define BAT_SPEED 10
#define TOP 1000
#define RIGHT 1001
#define BOTTOM 1002
#define LEFT 1003
#define BAT_WIDTH 10

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void moveBall();
int ballTouchesBorder();
bool ballTouchesBat();
void gameOver(HWND* hwnd);
double random();

RECT bat;
RECT ball;

double moveX = 1;
double moveY = random()*2-1;
double posX = 1;
double posY = 250;

int score = 1;

const UINT_PTR TMR1 = 1;
const UINT_PTR TMR_GRAPHICS = 2;

bool upPressed = false;
bool downPressed = false;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow) {
	POINT mouseLocation;
	GetCursorPos(&mouseLocation);
	srand(mouseLocation.x*mouseLocation.y);
	const wchar_t CLASS_NAME[] = L"PingPong";
	moveY = random() * 2 - 1;
	bat.top = 10;
	bat.bottom = 200;
	bat.right = 500;
	bat.left = bat.right - BAT_WIDTH;
	ball.top = 250;
	ball.bottom = 280;
	ball.left = 10;
	ball.right = 30;
	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		L"Ping Pong Schulprojekt",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		700,
		700,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (hwnd == NULL) {
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	SetTimer(hwnd,             // handle to main window 
		TMR1,            // timer identifier 
		1,                // five-minute interval 
		(TIMERPROC)NULL);
	SetTimer(hwnd,
		TMR_GRAPHICS,
		20,
		(TIMERPROC)NULL);

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	RECT windowRect;
	GetWindowRect(hwnd, &windowRect);
	wchar_t msg[32];

	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_TIMER:
		if (wParam == TMR1) {
			if (upPressed && bat.top > 0) {
				bat.top -= BAT_SPEED;
				bat.bottom -= BAT_SPEED;
			}
			else if (downPressed && bat.bottom < 500) {
				bat.top += BAT_SPEED;
				bat.bottom += BAT_SPEED;
			}
			moveBall();
			if (ballTouchesBat()) {
				ball.left = 469 - BAT_WIDTH;
				ball.right = 499 - BAT_WIDTH;
				posX = ball.left;
				moveX += 0.5;
				moveX *= -1;
				if (moveY < 0) {
					moveY -= 0.5;
				}
				else {
					moveY += 0.5;
				}
				moveY *= random() * 0.2 + 0.9;
				score++;
			}
			switch (ballTouchesBorder()) {
			case TOP:
				ball.top = 1;
				posY = 1;
				ball.bottom = 30;
				moveY *= -1;
				break;
			case RIGHT:
				if (!ballTouchesBat()) {
					KillTimer(hwnd, TMR1);
					gameOver(&hwnd);
				}
				break;
			case LEFT:
				ball.left = 1;
				ball.right = 30;
				posX = 1;
				moveX *= -1;
				break;
			case BOTTOM:
				ball.bottom = 499;
				ball.top = 470;
				posY = 470;
				moveY *= -1;
				break;
			}
		}
		else if (wParam == TMR_GRAPHICS) {
			RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ERASE);
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_UP:
			upPressed = true;
			downPressed = false;
			break;
		case VK_DOWN:
			upPressed = false;
			downPressed = true;
			break;
		}
		break;
	case WM_KEYUP:
		switch (wParam) {
		case VK_UP:
			upPressed = false;
			break;
		case VK_DOWN:
			downPressed = false;
			break;
		}
		break;
	case WM_PAINT:
		PAINTSTRUCT st;
		HDC hdc = BeginPaint(hwnd, &st);
		RECT rightBorder;
		RECT bottomBorder;
		rightBorder.left = 500;
		rightBorder.right = 502;
		bottomBorder.top = 500;
		bottomBorder.bottom = 502;
		rightBorder.top = 0;
		rightBorder.bottom = 500;
		bottomBorder.left = 0;
		bottomBorder.right = 500;
		FillRect(hdc, &st.rcPaint, (HBRUSH)(COLOR_WINDOW));
		FillRect(hdc, &bat, (HBRUSH)(COLOR_WINDOW - 2));
		FillRect(hdc, &ball, (HBRUSH)(COLOR_WINDOW - 1));
		FillRect(hdc, &rightBorder, (HBRUSH)(COLOR_WINDOW - 3));
		FillRect(hdc, &bottomBorder, (HBRUSH)(COLOR_WINDOW - 3));
		wchar_t buffer[32];
		wsprintfW(buffer, L"Score: %d", score);
		TextOut(hdc,
			10, 510,
			buffer,
			wcslen(buffer));
		break;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void moveBall() {
	posX += moveX;
	posY += moveY;
	ball.top = posY;
	ball.bottom = posY + 30;
	ball.left = posX;
	ball.right = posX + 30;
}
int ballTouchesBorder() {
	if (ball.left <= 0)
		return LEFT;
	else if (ball.right >= 500)
		return RIGHT;
	else if (ball.top <= 0)
		return TOP;
	else if (ball.bottom >= 500)
		return BOTTOM;
	else
		return 0;
}
bool ballTouchesBat() {
	if (ball.right > bat.left
		&& ball.left < bat.right
		&& ball.bottom > bat.top
		&& ball.top < bat.bottom)
		return true;
	else
		return false;
}
void gameOver(HWND * hwnd) {
	wchar_t buffer[64];
	wsprintfW(buffer, L"Game Over\nScore: %d", score);
	MessageBox(*hwnd, buffer, L"Game Over", MB_OK);
	
	bat.top = 10;
	bat.bottom = 200;
	bat.right = 500;
	bat.left = bat.right - BAT_WIDTH;
	ball.top = 250;
	ball.bottom = 280;
	ball.left = 10;
	ball.right = 30;
	posX = 10;
	posY = 250;
	moveX = 1;
	moveY = random()*2-1;
	score = 1;

	SetTimer(*hwnd,             // handle to main window 
		TMR1,            // timer identifier 
		1,                // five-minute interval 
		(TIMERPROC)NULL);
}

double random() {
	return (double)rand() / RAND_MAX;
}