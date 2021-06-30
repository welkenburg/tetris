#define UNICODE

#include <iostream>
using namespace std;

#include <Windows.h>
#include <cstdlib>

wstring tetromino[7];
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char *pField = nullptr;

int nScreenWidth = 120;
int nScreenHeight = 30;

int Rotate(int px, int py, int r){
	switch (r % 4)
	{
	case 0: return py * 4 + px;			// 0
	case 1: return 12 + py - (px * 4);	// 90
	case 2: return 15 - (py * 4) - px;	// 180
	case 3: return 3 - py + (px * 4);	// 270
	}
	return 0;
}

bool DoesPieceFit(int nTeromino, int nRotation, int nPosX, int nPosY){
	for(int px = 0; px < 4; px++)
		for(int py = 0; py < 4; py++){
			int pi = Rotate(px, py, nRotation);
			int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

			if (nPosX + px >= 0 && nPosX + px < nFieldWidth)
				if(nPosY + py >= 0 && nPosY + py < nFieldHeight)
				{
					if (tetromino[nTeromino][pi] == L'X' && pField[fi] != 0)
						return false;
				}
		}
	
	return true;
}

int main(){

	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");
	tetromino[0].append(L"..X.");

	tetromino[1].append(L"..X.");
	tetromino[1].append(L".XX.");
	tetromino[1].append(L".X..");
	tetromino[1].append(L"....");

	tetromino[2].append(L".X..");
	tetromino[2].append(L".XX.");
	tetromino[2].append(L"..X.");
	tetromino[2].append(L"....");

	tetromino[3].append(L"....");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L".XX.");
	tetromino[3].append(L"....");

	tetromino[4].append(L"..X.");
	tetromino[4].append(L".XX.");
	tetromino[4].append(L"..X.");
	tetromino[4].append(L"....");

	tetromino[5].append(L"....");
	tetromino[5].append(L".XX.");
	tetromino[5].append(L"..X.");
	tetromino[5].append(L"..X.");

	tetromino[6].append(L"....");
	tetromino[6].append(L".XX.");
	tetromino[6].append(L".X..");
	tetromino[6].append(L".X..");

	pField = new unsigned char[nFieldHeight * nFieldWidth];
	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;
	

	wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
	for( int i = 0; i < nScreenWidth * nScreenHeight; i++) screen[i] = L' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	bool bGameOver = false;

	int nCurrentPiece = 0;
	int nCurrentRotation = 0;
	int nCurrentX = nFieldWidth / 2;
	int nCurrentY = 0;

	bool bKey[4];
	bool bRotationHold = false;

	int nSpeed = 20;
	int nSpeedCounter = 0;
	bool bForceDown = false;

	// vector<int> vLines; 

	while(!bGameOver){
		Sleep(50); // this doesnt work ! this_thread::sleep_for(50ms)
		nSpeedCounter++;
		bForceDown = (nSpeedCounter == nSpeed);

		for(int k = 0; k < 4; k++)
			bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;
		
		if(bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY))
			nCurrentX--;
		
		if(bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY))
			nCurrentX++;
		
		if(bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
			nCurrentY++;
		
		if(bKey[3] && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY) && !bRotationHold){
			nCurrentRotation++;
			bRotationHold = true;
		}else
			bRotationHold = false;
		

		if(bForceDown){
			if(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
				nCurrentY++;
			else{

				for (int px = 0; px < 4; px++)
					for (int py = 0; py < 4; py++)
						if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
							pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

				for(int py = 0; py < 4; py++)
					if(nCurrentY + py < nFieldHeight - 1){
						bool bLine = true;
						for(int px = 1; px < nFieldWidth - 1; px++)
							bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;
						
						if(bLine){
							for(int px = 1; px < nFieldWidth - 1; px++)
								pField[(nCurrentY + py) * nFieldWidth + px] = 8;
							// vLines.push_back(nCurrentY + py);
						}
					}

				nCurrentX = nFieldWidth / 2;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
			}

			nSpeedCounter = 0;
		}
			



		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				screen[(y+2) * nScreenWidth + (x+2)] = L" ABCDEFG=#"[pField[y*nFieldWidth + x]];

		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
					screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65;

		// if(!vLines.empty()){
		// 	WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		// 	Sleep(400);

		// }

		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
		
	}

	return 0;
}

