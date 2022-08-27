#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<cmath>
#include<iostream>
using namespace std;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

//I am using Bresenham's Drawing Circle Algorithm for the face and the eyes
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
//I am using cardinal spline algorithm for drawing the mouth curve
class Vector2
{
public:
    double x;
    double y;
    Vector2(double a=0,double b=0)
    {
        x=a;;
        y=b;
    }
    Vector2(const Vector2& a)
    {
        x=a.x;
        y=a.y;
    }
};
Vector2 operator-(Vector2& a,Vector2 &b)
{
    double m=a.x-b.x;
    double n=a.y-b.y;
    Vector2 c(m,n);
    return c;
}
Vector2 operator*(double z,Vector2 a)
{
    double m=a.x*z;
    double n=a.y*z;
    Vector2 c(m,n);
    return c;
}
class Vector4
{
public:
    double x1;
    double u1;
    double x2;
    double u2;
    double v[4];
    Vector4(double a=0,double b=0,double c=0,double d=0)
    {
        v[0]=x1=a;
        v[1]=u1=b;
        v[2]=x2=c;
        v[3]=u2=d;
    }
    Vector4(const Vector4& v2)
    {
        memcpy(v,v2.v,4*sizeof(double));
    }
    double&operator[](int index)
    {
        return v[index];
    }
};
class Matrix4
{
public:
    double x0,x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15;
    double m[4][4];
    Matrix4(double a0=0,double a1=0,double a2=0,double a3=0,double a4=0,double a5=0,double a6=0,double a7=0,double a8=0,double a9=0,double a10=0,double a11=0,double a12=0,double a13=0,double a14=0,double a15=0)
    {
        m[0][0]=x0=a0;
        m[0][1]=x1=a1;
        m[0][2]=x2=a2;
        m[0][3]=x3=a3;
        m[1][0]=x4=a4;
        m[1][1]=x5=a5;
        m[1][2]=x6=a6;
        m[1][3]=x7=a7;
        m[2][0]=x8=a8;
        m[2][1]=x9=a9;
        m[2][2]=x10=a10;
        m[2][3]=x11=a11;
        m[3][0]=x12=a12;
        m[3][1]=x13=a13;
        m[3][2]=x14=a14;
        m[3][3]=x15=a15;
    }
    Matrix4(const Matrix4& m2)
    {
        memcpy(m,m2.m,16*sizeof(double));
    }
    double&operator()(int index1,int index2)
    {
        return m[index1][index2];
    }
};
Vector4 operator*(Matrix4 m1,Vector4& v1)
{
    Vector4 res;
    for(int i=0; i<4; i++)
        for(int j=0; j<4; j++)
            res[i]+=(v1[j]*m1(i,j));
    return res;
}
double operator*(Vector4 &v1,Vector4& v2)
{
    return v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]+v1[3]*v2[3];
}
int Round(double x)
{
    return int(x+0.5);
}
void DrawHermiteCurve(HDC hdc,Vector2& P1,Vector2& T1,Vector2& P2,Vector2& T2,int n,COLORREF c)
{
    double dt=1.0/n;
    Vector4 Gx(P1.x,T1.x,P2.x,T2.x);
    Vector4 Gy(P1.y,T1.y,P2.y,T2.y);
    Matrix4 m(1,0,0,0,0,1,0,0,-3,-2,3,-1,2,1,-2,1);
    Vector4 Vx(m*Gx);
    Vector4 Vy(m*Gy);
    double x,y;
    for(double t=0; t<1; t+=dt)
    {
        Vector4 Vt(1,t,t*t,t*t*t);
        x=Vt*Vx;
        y=Vt*Vy;
        SetPixel(hdc,Round(x),Round(y),c);

    }
}
void CardinalSpline(HDC hdc,Vector2 P[],int n,int NoP,double c,COLORREF color)
{
    Vector2 *T;
    T=new Vector2[n];
    T[0]=(1-c)/2.0*(P[1]-P[0]);
    T[n-1]=(1-c)/2.0*(P[n-1]-P[n-2]);
    for(int i=1; i<n-1; i++)
        T[i]=(1-c)/2.0*(P[i+1]-P[i-1]);
    for(int i=0; i<n-1; i++)
        DrawHermiteCurve(hdc,P[i],T[i],P[i+1],T[i+1],NoP,color);

}
//I am using Bresenham's algorithm fro drawing a line for the hair of the face
//The algorithm can be extended to cover gradients between 0 and -1 by checking whether y needs to increase or decrease (i.e. dy < 0)
void PlotLineLow(HDC hdc,int x0,int y0,int x1, int y1,int xc,int yc,int r,COLORREF c)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    double d=0;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int  y = y0;

    for ( int x=x0; x<x1; x++)
    {
        //This is the part where the clipping happens; I check if the point is inside or outside the circle and then draw it in the colors required
        //This is done by calculating the distance between the center of the circle and the point and then comparing it to the radius
        d= r*r - ((xc-x)*(xc-x) + (yc-y)*(yc-y));
        if(d<=0)
            SetPixel(hdc,x,y,RGB(255,0,0));
        else
            SetPixel(hdc,x,y,RGB(0,0,255));
        if (D > 0)
        {
            y = y + yi;
            D = D + (2 * (dy - dx)) ;
        }
        else
            D = D + 2*dy;
    }
}
void PlotLineHigh(HDC hdc,int x0, int y0,int x1, int y1,int xc,int yc,int r,COLORREF c)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    int d;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y=y0; y<y1; y++)
    {
        //This is the part where the clipping happens; I check if the point is inside or outside the circle and then draw it in the colors required
        //This is done by calculating the distance between the center of the circle and the point and then comparing it to the radius
        d= r*r - ((xc-x)*(xc-x) + (yc-y)*(yc-y));
        if(d<=0)
            SetPixel(hdc,x,y,RGB(255,0,0));
        else
            SetPixel(hdc,x,y,RGB(0,0,255));
        if(D > 0)
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;
    }
}
void DrawLine(HDC hdc,int x0,int y0,int x1,int y1,int xc,int yc,int radius,COLORREF c)
{
    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0 > x1)
            PlotLineLow(hdc,x1, y1, x0, y0,xc,yc,radius,c);
        else
            PlotLineLow(hdc,x0, y0, x1, y1,xc,yc,radius,c);
    }
    else
    {
        if (y0 > y1)
            PlotLineHigh(hdc,x1, y1, x0, y0,xc,yc,radius,c);
        else
            PlotLineHigh(hdc,x0, y0, x1, y1,xc,yc,radius,c);
    }
}



/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    cout<<"*********************************************************************"<<endl;
    cout<<"BECAREFUL OF THE INSTRUCTION TO DRAW THE FIGURE, PLEASE FOLLOW THEM:"<<endl;
    cout<<"*********************************************************************"<<endl;
    cout<<"First: Draw the circle window boundary; the face by single down left click and sweep to the radius you want then single up left click"<<endl;
    cout<<"Second: Draw the eyes; small circles by single down left click and sweep to the radius you want then single up left click"<<endl;
    cout<<"Third: Draw the mouth; the curve by right double clicks for each point; four times as we want to draw a mouth"<<endl;
    cout<<"Fourth: Draw the lines; the hair by single down right click, then sweep the distance you want then single up right click"<<endl;
    cout<<"Thanks"<<endl;
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
int x1,y3,x2,y2,x11,y33,x22,y22,xs,ys,xe,ye,radius1;
int n=3;// BECAREFUL   This is our variable we can change this only and the rest is covered and if you want to draw n points then click n+1 times
Vector2 *P=new Vector2[n];
double c=0.36785963;
static int index=0;
bool flag=1;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_LBUTTONDOWN:
    {
        if(flag)
        {
            x1=LOWORD(lParam);
            y3=HIWORD(lParam);
        }
        else
        {
            x11=LOWORD(lParam);
            y33=HIWORD(lParam);
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        if(flag)
        {
            HDC hdc=GetDC(hwnd);
            x2=LOWORD(lParam);
            y2=HIWORD(lParam);
            radius1=sqrt((x2-x1)*(x2-x1)+(y2-y3)*(y2-y3));
            DrawCircle(hdc,x1,y3,radius1,RGB(0,0,0));
            ReleaseDC(hwnd,hdc);
            flag=0;
        }
        else
        {
            HDC hdc=GetDC(hwnd);
            x22=LOWORD(lParam);
            y22=HIWORD(lParam);
            int radius=sqrt((x22-x11)*(x22-x11)+(y22-y33)*(y22-y33));
            DrawCircle(hdc,x11,y33,radius,RGB(0,0,255));
            ReleaseDC(hwnd,hdc);
        }
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        if(index<n)
        {
            P[index].x=LOWORD(lParam);
            P[index].y=HIWORD(lParam);
            index++;
        }
        else
        {
            index=0;
            HDC hdc=GetDC(hwnd);
            CardinalSpline(hdc,P,n,112000,c,RGB(0,0,255));
            ReleaseDC(hwnd,hdc);
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        xs=LOWORD(lParam);
        ys=HIWORD(lParam);
        break;
    }
    case WM_RBUTTONUP:
    {
        HDC hdc=GetDC(hwnd);
        xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        hdc=GetDC(hwnd);
        DrawLine(hdc,xs,ys,xe,ye,x1,y3,radius1,RGB(0,0,255));
        flag=1;
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
