#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<cmath>
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

void Draw4Points(HDC hdc,int xc,int yc,int a,int b,COLORREF c )
{
    SetPixel(hdc,xc-a,yc+b,c);
    SetPixel(hdc,xc-a,yc-b,c);
    SetPixel(hdc,xc+a,yc+b,c);
    SetPixel(hdc,xc+a,yc-b,c);
}
int Round(double x)
{
    return int(x+0.5);
}
int Max(int r1,int r2)
{
    if(r1>r2)
        return r1;
    return r2;
}
void DrawEllipse(HDC hdc,int xc,int yc,double r1,double r2,COLORREF c)
{
    double dtheta=1.0/Max(r1,r2);
    double ctheta=cos(dtheta);
    double stheta=sin(dtheta);
    double x=r1;
    double y=0;
    Draw4Points(hdc,xc,yc,x,y,c);
    while(x>0)
    {
        double x1=x*ctheta-(r1/r2)*y*stheta;
        y=y*ctheta+(r2/r1)*x*stheta;
        x=x1;
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
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
int xs,ys,xe,ye,x3,y3;
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
        xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        HDC hdc=GetDC(hwnd);
        x3=LOWORD(lParam);
        y3=HIWORD(lParam);
        double radius1=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
        double radius2=sqrt((x3-xs)*(x3-xs)+(y3-ys)*(y3-ys));
        DrawEllipse(hdc,xs,ys,radius1,radius2,RGB(255,0,0));
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
