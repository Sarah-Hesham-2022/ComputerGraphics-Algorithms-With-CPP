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
bool DrawFlag=0;
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
struct Vector
{
    POINT v[2];
    Vector()
    {
        v[0].x=v[0].y=v[1].x=v[1].y=0;
    }
    Vector(POINT p1,POINT p2)
    {
        v[0]=p1;
        v[1]=p2;
    }
    Vector(const Vector&obj)
    {
        v[0]=obj.v[0];
        v[1]=obj.v[1];
    }
};
bool operator*(Vector &v1,Vector &v2)
{
    int X1 = (v1.v[1].x - v1.v[0].x);
    int Y1 = (v1.v[1].y -v1.v[0].y);
    int X2 = (v2.v[1].x - v2.v[0].x);
    int Y2 = (v2.v[1].y - v2.v[0].y);
    int crossProduct=X1 * Y2 - Y1 * X2;
    return (crossProduct>=0);
}
void DrawFigureIfConvexPolygon(HDC hdc,POINT p[],int n,COLORREF c,HWND hwnd)
{
    Vector *v;
    v=new Vector[n];

    for(int i=0; i<n-1; i++)
        v[i]=Vector(p[i],p[i+1]);
    v[n-1]=Vector(p[n-1],p[0]);

    bool *flag;
    flag=new bool[n];
    bool test=1;
    for(int i=0; i<n-1; i++)
    {
        flag[i]=v[i]*v[i+1];
    }
    flag[n-1]=v[n-1]*v[0];
    for(int i=0; i<n-1; i++)
    {
        if(flag[i]!=flag[i+1])
        {
            test=0;
            break;
        }
    }
    if(test)
    {
        for(int i=0; i<n-1; i++)
            DrawLine(hdc,p[i].x,p[i].y,p[i+1].x,p[i+1].y,c);
        DrawLine(hdc,p[n-1].x,p[n-1].y,p[0].x,p[0].y,c);
        MessageBox(hwnd,"Convex","Message",MB_OK);
        DrawFlag=1;
    }
    else
    {
        MessageBox(hwnd,"Non Convex Can't Fill it, sorry.","Message",MB_OK);
        DrawFlag=0;
        return;
    }
}
#define MAXENTRIES 60000
struct Entry
{
    int xmin,xmax;
};
void InitEntries(Entry table[])
{
    for(int i=0; i<MAXENTRIES; i++)
    {
        table[i].xmin=10000;
        table[i].xmax=-10000;
    }
}
void Swap(POINT &v1,POINT &v2)
{
    POINT z=POINT(v1);
    v1.x=v2.x;
    v1.y=v2.y;
    v2.x=z.x;
    v2.y=z.y;
}
void ScanEdge(POINT v1,POINT v2,Entry table[])
{
    if(v1.y==v2.y)return;
    if(v1.y>v2.y)Swap(v1,v2);
    double minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    double x=v1.x;
    int y=v1.y;
    while(y<v2.y)
    {
        if(x<table[y].xmin)table[y].xmin=(int)ceil(x);
        if(x>table[y].xmax)table[y].xmax=(int)floor(x);
        y++;
        x+=minv;
    }
}
void DrawSanLines(HDC hdc,Entry table[],COLORREF color)
{
    for(int y=0; y<MAXENTRIES; y++)
        if(table[y].xmin<table[y].xmax)
            for(int x=table[y].xmin; x<=table[y].xmax; x++)
                SetPixel(hdc,x,y,color);
}
void ConvexFill(HDC hdc,POINT p[],int n,COLORREF color)
{
    Entry *table=new Entry[MAXENTRIES];
    InitEntries(table);
    POINT v1=p[n-1];
    for(int i=0; i<n; i++)
    {
        POINT v2=p[i];
        ScanEdge(v1,v2,table);
        v1=p[i];
    }
    DrawSanLines(hdc,table,color);
    delete table;
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
int xs,ys,xe,ye;
static int SizeCount=0;
POINT p[100];
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDBLCLK:
    {
        xs=LOWORD(lParam);
        ys=HIWORD(lParam);
        POINT temp;
        temp.x=xs;
        temp.y=ys;
        p[SizeCount]=POINT(temp);
        SizeCount++;
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        HDC hdc=GetDC(hwnd);
        xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        DrawFigureIfConvexPolygon(hdc,p,SizeCount,RGB(255,0,0),hwnd);
        if(DrawFlag)
            ConvexFill(hdc,p,SizeCount,RGB(200,200,5));
        SizeCount=0;
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
