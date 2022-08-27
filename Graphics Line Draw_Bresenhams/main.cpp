#include<windows.h>

LRESULT WINAPI WindowProcedure(HWND,UINT,WPARAM,LPARAM);//basic function drawings

//The algorithm can be extended to cover gradients between 0 and -1 by checking whether y needs to increase or decrease (i.e. dy < 0)
void PlotLineLow(HDC hdc,int x0,int y0,int x1, int y1,COLORREF c)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int  y = y0;

    for ( int x=x0; x<x1; x++)
    {
        SetPixel(hdc,x,y,c);
        if (D > 0)
        {
            y = y + yi;
            D = D + (2 * (dy - dx)) ;
        }
        else
            D = D + 2*dy;
    }
}
void PlotLineHigh(HDC hdc,int x0, int y0,int x1, int y1,COLORREF c)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y=y0; y<y1; y++)
    {
        SetPixel(hdc,x,y,c);
        if(D > 0)
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;
    }
}
void DrawLine(HDC hdc,int x0,int y0,int x1,int y1,COLORREF c)
{
    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0 > x1)
            PlotLineLow(hdc,x1, y1, x0, y0,c);
        else
            PlotLineLow(hdc,x0, y0, x1, y1,c);
    }
    else
    {
        if (y0 > y1)
            PlotLineHigh(hdc,x1, y1, x0, y0,c);
        else
            PlotLineHigh(hdc,x0, y0, x1, y1,c);
    }
}

int APIENTRY WinMain(HINSTANCE hInst,HINSTANCE hPrevInst,LPSTR args,int nsh)
{
    // MessageBox(NULL,"HELLO WORLD","My Window",MB_OK);
    WNDCLASS wc;
    wc.cbClsExtra=wc.cbWndExtra=0;
    wc.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hIcon=LoadIcon(NULL,IDI_APPLICATION);
    wc.hInstance=hInst;
    wc.lpszClassName="My Window";
    wc.lpfnWndProc=WindowProcedure; //no brackets here as we are passing a pointer
    wc.style=CS_HREDRAW | CS_VREDRAW;

    if(!RegisterClass(&wc))
        return -1;

    HWND hWnd=CreateWindow("My Window","My Window Title",WS_OVERLAPPEDWINDOW,200,200,500,500,NULL,NULL,NULL,NULL);
    ShowWindow(hWnd,nsh);
    UpdateWindow(hWnd);
    MSG msg= {0};
    while(GetMessage((&msg),NULL,NULL,NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
int xs,ys,xe,ye;
LRESULT WINAPI WindowProcedure(HWND hWnd,UINT msg,WPARAM wc,LPARAM lp)
{
    HDC hdc;
    switch(msg)
    {
    case WM_LBUTTONDOWN:
    {
        xs=LOWORD(lp);
        ys=HIWORD(lp);
        break;
    }
    case WM_LBUTTONUP:
    {
        xe=LOWORD(lp);
        ye=HIWORD(lp);
        hdc=GetDC(hWnd);
        DrawLine(hdc,xs,ys,xe,ye,RGB(0,0,255));
        ReleaseDC(hWnd,hdc);
        break;
    }
    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        break;
    }
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd,msg,wc,lp);
    }
}

