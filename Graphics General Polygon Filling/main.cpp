#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<cmath>
#include <list>
using namespace std;//for MAXENTRIES
#define MAXENTRIES 60000
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
struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
        return x<r.x;
    }
};
typedef list<EdgeRec> EdgeList;
EdgeRec InitEdgeRec(POINT& v1,POINT& v2)
{
    if(v1.y>v2.y)swap(v1,v2);
    EdgeRec rec;
    rec.x=v1.x;
    rec.ymax=v2.y;
    rec.minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    return rec;
}
void InitEdgeTable(POINT *polygon,int n,EdgeList table[])
{
    POINT v1=polygon[n-1];
    for(int i=0; i<n; i++)
    {
        POINT v2=polygon[i];
        if(v1.y==v2.y)
        {
            v1=v2;
            continue;
        }
        EdgeRec rec=InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1=polygon[i];
    }
}
void GeneralPolygonFill(HDC hdc,POINT *polygon,int n,COLORREF c)
{
    for(int i=0; i<n-1; i++)
        DrawLine(hdc,polygon[i].x,polygon[i].y,polygon[i+1].x,polygon[i+1].y,RGB(255,0,0));
    DrawLine(hdc,polygon[n-1].x,polygon[n-1].y,polygon[0].x,polygon[0].y,RGB(255,0,0));
    EdgeList *table=new EdgeList [MAXENTRIES];
    InitEdgeTable(polygon,n,table);
    int y=0;
    while(y<MAXENTRIES && table[y].size()==0)y++;
    if(y==MAXENTRIES)return;
    EdgeList ActiveList=table[y];
    while (ActiveList.size()>0)
    {
        ActiveList.sort();
        for(EdgeList::iterator it=ActiveList.begin(); it!=ActiveList.end(); it++)
        {
            int x1=(int)ceil(it->x);
            it++;
            int x2=(int)floor(it->x);
            for(int x=x1; x<=x2; x++)SetPixel(hdc,x,y,c);
        }
        y++;
        EdgeList::iterator it=ActiveList.begin();
        while(it!=ActiveList.end())
            if(y==it->ymax) it=ActiveList.erase(it);
            else it++;
        for(EdgeList::iterator it=ActiveList.begin(); it!=ActiveList.end(); it++)
            it->x+=it->minv;
        ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
    }
    delete[] table;
}

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

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
POINT*p=new POINT[100];
static int Count=0;
int xs,ys,xe,ye;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDBLCLK:
    {
        xs=LOWORD(lParam);
        ys=HIWORD(lParam);
        p[Count].x=xs;
        p[Count].y=ys;
        Count++;
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        HDC hdc=GetDC(hwnd);
        GeneralPolygonFill(hdc,p,Count,RGB(0,200,255));
        Count=0;
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
