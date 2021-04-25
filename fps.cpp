#include <iostream>
#include <cmath>
#include <chrono>
#include <vector>
#include <string>
#include <algorithm>
#include <Windows.h>
using namespace std;

float PI = 2 * acosf(0.0f);

int iScreenHeight = 40;
int iScreenWidth = 120;

// Player view angle in radians
float fPlayerAngle = 0.0f;
float fPlayerX = 8.0f;
float fPlayerY = 8.0f;

float fFOV = PI / 4;
float fDepth = 16.0f;
int iMapHeight = 16;
int iMapWidth = 16;

int main(void)
{
    // wchar allows unicode string
    wchar_t *screen = new wchar_t[iScreenHeight * iScreenWidth + 1];
    wchar_t *title = (wchar_t *)L"FPS ViewModel";

    // HANDLE is a pointer. This handle points to the console window
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    if (hConsole == INVALID_HANDLE_VALUE)
    {
        cout << "Unable to create a screen handle. Exiting...";
        exit(0);
    }

    // Sets hConsole to be the active buffer for the console to be displayed
    SetConsoleActiveScreenBuffer(hConsole);
    SetConsoleTitleW(title);

    DWORD dwBytesWritten = 0;

    wstring map;
    // L strings use unicode instead of ASCII
    map += L"################";
    map += L"#..............#";
    map += L"###............#";
    map += L"#.......####...#";
    map += L"#..........#...#";
    map += L"#..........#...#";
    map += L"##.........#...#";
    map += L"#..............#";
    map += L"#..............#";
    map += L"#......#.......#";
    map += L"#.#....#.......#";
    map += L"#.#....#.......#";
    map += L"#.#....#.....###";
    map += L"#.#....#.......#";
    map += L"#.#....#.......#";
    map += L"################";

    auto time1 = chrono::system_clock::now();
    auto time2 = chrono::system_clock::now();

    int iFPS = 1000;

    // Game Loop
    while (true)
    {
        time2 = chrono::system_clock::now();
        chrono::duration<float> elapsedTime = time2 - time1;
        time1 = time2;
        float fElapsedTime = elapsedTime.count() + 0.0001f;
        if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
        {
            fPlayerAngle -= 0.8f * fElapsedTime;
        }
        else if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
        {
            fPlayerAngle += 0.8f * fElapsedTime;
        }
        else if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
        {
            fPlayerX += sinf(fPlayerAngle) * fElapsedTime * 5.0f;
            fPlayerY += cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            if (map[iMapWidth * (int)fPlayerY + (int)fPlayerX] == L'#')
            {
                fPlayerX -= sinf(fPlayerAngle) * fElapsedTime * 5.0f;
                fPlayerY -= cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            }
        }
        else if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerAngle) * fElapsedTime * 5.0f;
            fPlayerY -= cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            if (map[iMapWidth * (int)fPlayerY + (int)fPlayerX] == L'#')
            {
                fPlayerX += sinf(fPlayerAngle) * fElapsedTime * 5.0f;
                fPlayerY += cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            }
        }
        else if (GetAsyncKeyState((unsigned short)'Q') & 0x8000)
        {
            fPlayerX -= sinf(fPlayerAngle) * fElapsedTime * 5.0f;
            fPlayerY += cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            if (map[iMapWidth * (int)fPlayerY + (int)fPlayerX] == L'#')
            {
                fPlayerX += sinf(fPlayerAngle) * fElapsedTime * 5.0f;
                fPlayerY -= cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            }
        }
        else if (GetAsyncKeyState((unsigned short)'E') & 0x8000)
        {
            fPlayerX += sinf(fPlayerAngle) * fElapsedTime * 5.0f;
            fPlayerY -= cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            if (map[iMapWidth * (int)fPlayerY + (int)fPlayerX] == L'#')
            {
                fPlayerX -= sinf(fPlayerAngle) * fElapsedTime * 5.0f;
                fPlayerY += cosf(fPlayerAngle) * fElapsedTime * 5.0f;
            }
        }

        for (int col = 0; col < iScreenWidth; col++)
        {
            float fCurrentRayAngle = (fPlayerAngle - (fFOV / 2.0f)) + ((float)col / (float)iScreenWidth) * fFOV;

            //
            float fEyeX = sinf(fCurrentRayAngle);
            float fEyeY = cosf(fCurrentRayAngle);

            float fDistanceToWall = 0.0f;
            bool bHitWall = false;
            bool bBoundary = false;

            while (!bHitWall && fDistanceToWall < fDepth)
            {
                fDistanceToWall += 0.1f;

                int iCurrentX = (int)(fPlayerX + fEyeX * fDistanceToWall);
                int iCurrentY = (int)(fPlayerY + fEyeY * fDistanceToWall);

                if (iCurrentX < 0 || iCurrentX >= iScreenWidth || iCurrentY < 0 || iCurrentY >= iScreenHeight)
                {
                    bHitWall = true;
                    fDistanceToWall = fDepth;
                }
                else
                {
                    if (map[iMapWidth * iCurrentY + iCurrentX] == L'#')
                    {
                        bHitWall = true;
                        vector<pair<float, float>> p;
                        for (int tx = 0; tx < 2; tx++)
                        {
                            for (int ty = 0; ty < 2; ty++)
                            {
                                float fCornerX = (float)iCurrentX + tx - fPlayerX;
                                float fCornerY = (float)iCurrentY + ty - fPlayerY;
                                float fDist = sqrt(fCornerX * fCornerX + fCornerY * fCornerY);
                                float fDotProd = (fEyeX * fCornerX / fDist) + (fEyeY * fCornerY / fDist);
                                p.push_back(make_pair(fDist, fDotProd));
                            }
                        }

                        sort(p.begin(), p.end(), [](const pair<float, float> &left, const pair<float, float> &right) { return left.first < right.first; });

                        float fBound = 0.01f;
                        if (acos(p[0].second) < fBound)
                            bBoundary = true;
                        if (acos(p[1].second) < fBound)
                            bBoundary = true;
                    }
                }
            }

            // Distance of walls from top and bottom of screen
            int iCeiling = (float)(iScreenHeight / 2.0f) - (float)(iScreenHeight / fDistanceToWall);
            int iFloor = iScreenHeight - iCeiling;

            short iShade = ' ';
            if (fDistanceToWall <= fDepth / 4.0f)
                iShade = 0x2588;
            else if (fDistanceToWall < fDepth / 3.0f)
                iShade = 0x2593;
            else if (fDistanceToWall < fDepth / 2.0f)
                iShade = 0x2592;
            else if (fDistanceToWall < fDepth)
                iShade = 0x2591;

            if (bBoundary)
                iShade = ' ';
            for (int row = 0; row < iScreenHeight; row++)
            {
                if (row < iCeiling)
                {
                    screen[iScreenWidth * row + col] = L' ';
                }
                else if (row >= iCeiling && row <= iFloor)
                {
                    screen[iScreenWidth * row + col] = iShade;
                }
                else
                {
                    float floorDistance = 1.0f - (((float)row - iScreenHeight / 2.0f) * 2.0f / (float)iScreenHeight);
                    if (floorDistance < 0.25)
                        screen[iScreenWidth * row + col] = L'#';
                    else if (floorDistance < 0.5)
                        screen[iScreenWidth * row + col] = L'x';
                    else if (floorDistance < 0.75)
                        screen[iScreenWidth * row + col] = L'.';
                    else if (floorDistance < 0.9)
                        screen[iScreenWidth * row + col] = L'-';
                    else
                        screen[iScreenWidth * row + col] = L' ';
                }
            }
        }
        if (fElapsedTime != 0)
            iFPS = (int)(1 / fElapsedTime);
        // title = ;
        // SetConsoleTitleW(title);
        swprintf(screen, 50, L"X = %3.2f, Y = %3.2f, A = %3.2f, FPS = %d     ", fPlayerX, fPlayerY, fPlayerAngle, iFPS);

        for (int x = 0; x < iMapWidth; x++)
        {
            for (int y = 0; y < iMapHeight; y++)
            {
                screen[(y + 1) * iScreenWidth + x] = map[y * iMapWidth + x];
            }
        }
        screen[((int)fPlayerY + 1) * iScreenWidth + (int)fPlayerX] = L'*';

        // Write the screen string to console pointed to by hConsole
        // {0, 0} specifies where to start printing hence console doesnt scroll since we overwrite everytime from starting position
        screen[iScreenHeight * iScreenWidth] = '\0';
        WriteConsoleOutputCharacterW(hConsole, screen, iScreenHeight * iScreenWidth, {0, 0}, &dwBytesWritten);
    }

    return 0;
}