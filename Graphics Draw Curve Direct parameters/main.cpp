#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
class Vector4
{
public:
    double x1;
    double u1;
    double x2;
    double v[3];
    Vector4(double a=0,double b=0,double c=0)
    {
        v[0]=x1=a;
        v[1]=u1=b;
        v[2]=x2=c;
    }
    Vector4(const Vector4& v2)
    {
        memcpy(v,v2.v,3*sizeof(double));
    }
    double&operator[](int index)
    {
        return v[index];
    }
};
double operator*(Vector4 &v1,Vector4& v2)
{
    return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2];
}
int Round(double x)
{
    return int(x+0.5);
}
void DrawHermiteCurve(HDC hdc,double x1,double x2,double x3,double y1,double y2,double y3,int n,COLORREF c)
{
    double dt=1.0/n;
    Vector4 x(x1,x2,x3);
    Vector4 y(y1,y2,y3);
    double a=x1;
    double b=y1;
    double d1x=dt*x2*dt+x1*dt;
    double d2x=dt*dt*2*x2;
    double d1y=dt*y2*dt+y1*dt;
    double d2y=dt*dt*y2*2;
    for(double t=dt; t<1; t+=dt)
    {
        SetPixel(hdc,Round(a),Round(b),c);
        a+=(d1x);
        b+=(d1y);
        d1x+=d2x;
        d1y+=d2y;

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
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
    {
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;
    }
    case WM_LBUTTONDBLCLK:
    {
        HDC hdc=GetDC(hwnd);
        DrawHermiteCurve(hdc,10,205,100,50,100,100,120000,RGB(0,0,255));
        ReleaseDC(hwnd,hdc);
        break;
    }
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
