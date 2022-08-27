#include<windows.h>

LRESULT WINAPI WindowProcedure(HWND,UINT,WPARAM,LPARAM);//basic function drawings

int round(double x)
{
    return (int)(x+0.5);
}

void DrawLine(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y=ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,x,round(y),color);
        }
    }
    else
    {
        int y=ys,yinc= dy>0?1:-1;
        double x=xs,xinc=(double)dx/dy*yinc;
        while(y!=ye)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,round(x),y,color);
        }
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

    /*  HWND hwndButton = CreateWindow(
                            "BUTTON",  // Predefined class; Unicode assumed
                            "OK BUTTON",      // Button text
                            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles
                            10,         // x position
                            10,         // y position
                            100,        // Button width
                            100,        // Button height
                            hWnd,     // Parent window
                            NULL,       // No menu.
                            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                            NULL);      // Pointer not needed.

      HWND hwndEdit= CreateWindow("EDIT",
                                  NULL,
                                  WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |
                                  ES_MULTILINE | ES_AUTOVSCROLL|WS_BORDER|WS_HSCROLL,
                                  100,100,100,100,
                                  hWnd,
                                  NULL,
                                  (HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                                  NULL);

      HWND hwndList= CreateWindow("LISTBox",
                                  NULL,
                                  WS_BORDER | WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT |WS_BORDER
                                  |WS_HSCROLL|LBS_COMBOBOX|LBS_DISABLENOSCROLL|LBS_EXTENDEDSEL|LBS_MULTIPLESEL,
                                  200,200,300,30,
                                  hWnd,
                                  NULL,
                                  (HINSTANCE) GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
                                  NULL);*/

    MSG msg= {0};
    while(GetMessage((&msg),NULL,NULL,NULL))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT WINAPI WindowProcedure(HWND hWnd,UINT msg,WPARAM wc,LPARAM lp)
{
    HDC hdc;
    static int x1,y1;
    switch(msg)
    {
    /* case WM_KEYDOWN:
     {
         hdc=GetDC(hWnd);
         Rectangle(hdc,900,1000,400,500);
         ReleaseDC(hWnd,hdc);
         break;
     }
     case WM_KEYUP:
     {
         hdc=GetDC(hWnd);
         Arc(hdc,100,200,200,400,300,600,400,800);
         ReleaseDC(hWnd,hdc);
         break;
     }*/
    case WM_LBUTTONDOWN:
    {
        /* MessageBox(hWnd,"Hello","Welcome",MB_OK);
        hdc=GetDC(hWnd);
        //IMPORTANT ADD LIBRARY gdi32 .Just normal circle: Ellipse(hdc,100,100,400,400);
        Ellipse(hdc,LOWORD(lp)-50,HIWORD(lp)-50,LOWORD(lp)+50,HIWORD(lp)+50);//where the mouse clicks
        ReleaseDC(hWnd,hdc);*/
        x1=LOWORD(lp);
        y1=HIWORD(lp);
        break;
    }
    case WM_LBUTTONUP:
    {
        hdc=GetDC(hWnd);
        DrawLine(hdc,x1,y1,LOWORD(lp),HIWORD(lp),RGB(0,0,255));
        ReleaseDC(hWnd,hdc);
        break;
    }
    /* case WM_RBUTTONDOWN:
     {
         hdc=GetDC(hWnd);
         LineTo(hdc,LOWORD(lp)+100,HIWORD(lp)+100);
         ReleaseDC(hWnd,hdc);
         break;
     }*/
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

