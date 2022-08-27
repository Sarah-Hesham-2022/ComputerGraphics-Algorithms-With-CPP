#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <cmath>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

int Round(double x)
{
    return int(x+0.5);
}
void Swap(int &x1,int &x2)
{
    int z;
    z=x1;
    x1=x2;
    x2=z;
}
void DrawLine(HDC hdc,int x1,int y1,int x2,int y2,COLORREF c)
{
    int dx=x2-x1;
    int dy=y2-y1;
    if(abs(dx)>=abs(dy))
    {
        if(x1>x2)
        {
            Swap(x1,x2);
            Swap(y1,y2);
        }
        double y=y1;
        double slope=double(dy)/dx;
        for(int x=x1; x<=x2; x++)
        {
            SetPixel(hdc,x,Round(y),c);
            y+=slope;
        }

    }
    else
    {
        if(y1>y2)
        {
            Swap(x1,x2);
            Swap(y1,y2);
        }
        double x=x1;
        double slope=double(dx)/dy;
        for(int y=y1; y<=y2; y++)
        {
            SetPixel(hdc,Round(x),y,c);
            x+=slope;
        }
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
int xs,xe,ys,ye;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc=GetDC(hwnd);
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
        int length=sqrt(pow((xe-xs),2)+pow((ye-ys),2));
        int y3,y4;
        y3=ys-length;
        y4=ye-length;
        DrawLine(hdc,xs,ys,xe,ye,RGB(255,0,0));
        DrawLine(hdc,xs,ys,xs,y3,RGB(255,0,0));
        DrawLine(hdc,xe,ye,xe,y4,RGB(255,0,0));
        DrawLine(hdc,xs,y3,xe,y4,RGB(255,0,0));
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
