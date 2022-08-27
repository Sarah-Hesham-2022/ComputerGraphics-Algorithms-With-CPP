#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<cmath>
#include<stack>
using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void NRFloodFill(HDC hdc,int x,int y,COLORREF bc,COLORREF fc)
{
    struct MyPoint
    {
        int x,y;
        MyPoint(int x1=0,int y1=0)
        {
            x=x1;
            y=y1;
        }
        MyPoint(const MyPoint &obj)
        {
            x=obj.x;
            y=obj.y;
        }
    };
    stack<MyPoint> s;
    s.push(MyPoint(x,y));
    while(!s.empty())
    {
        MyPoint p=s.top();
        s.pop();
        COLORREF c=GetPixel(hdc,p.x,p.y);
        if(c==bc || c==fc)
            continue;
        SetPixel(hdc,p.x,p.y,fc);
        s.push(MyPoint((p.x)+1,p.y));
        s.push(MyPoint((p.x)-1,p.y));
        s.push(MyPoint(p.x,(p.y)-1));
        s.push(MyPoint(p.x,(p.y)+1));

    }
}
void Draw8Points(HDC hdc,int xc,int yc,int a,int b,COLORREF c)
{
    SetPixel(hdc,xc+a,yc+b,c);
    SetPixel(hdc,xc+a,yc-b,c);
    SetPixel(hdc,xc-a,yc+b,c);
    SetPixel(hdc,xc-a,yc-b,c);
    SetPixel(hdc,xc+b,yc+a,c);
    SetPixel(hdc,xc-b,yc+a,c);
    SetPixel(hdc,xc+b,yc-a,c);
    SetPixel(hdc,xc-b,yc-a,c);
}
void DrawCircle(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    while(x<y)
    {
        if(d1<0)
        {
            d1+=c1;
            c2+=2;
        }
        else
        {
            d1+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        Draw8Points(hdc,xc,yc,x,y,c);
    }
}
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Code::Blocks Template Windows App"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               544,                 /* The programs width */
               375,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
int xs,ys,xe,ye,x,y;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDOWN:
    {
        xs=LOWORD(lParam);
        ys=HIWORD(lParam);
        break;

    }
    case WM_LBUTTONUP:
    {
        HDC hdc=GetDC(hwnd);
        xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        int radius=(int)sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
        DrawCircle(hdc,xs,ys,radius,RGB(0,0,255));
        ReleaseDC(hwnd,hdc);
        break;

    }
    case WM_RBUTTONDBLCLK:
    {
        HDC hdc=GetDC(hwnd);
        x=LOWORD(lParam);
        y=HIWORD(lParam);
        NRFloodFill(hdc,x,y,RGB(0,0,255),RGB(255,0,0));
        ReleaseDC(hwnd,hdc);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
