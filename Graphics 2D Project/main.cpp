#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
//need this library for calculating the radii of circles as we need sqrt() function
#include<cmath>
//******************************************************************
//******************************************************************
//here we add interaction between window and console
#include<iostream>
using namespace std;
//******************************************************************
//******************************************************************
//Those for making a file and writing to it and saving the data of the screen
#include <fstream>
//******************************************************************
//******************************************************************
//Let us declare some variables to track all our data for the implementation of the load,save functions
static int SaveArrPoints[900000];
static int SaveArrNoOfPoints[900000];
static COLORREF SaveArrColorPoints[900000];
static int SP=0,SN=0,SC=0;
//******************************************************************
//******************************************************************
//I am implementing the Draw Line DDA algorithm
int Round(double x)
{
    return (int)(x+0.5);
}
void DrawLineDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
    int Count=0;
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    SaveArrPoints[SP++]=xs;
    SaveArrPoints[SP++]=ys;
    Count++;
    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y=ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,x,Round(y),color);
            Count++;
            SaveArrPoints[SP++]=x;
            SaveArrPoints[SP++]=Round(y);
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
            SetPixel(hdc,Round(x),y,color);
            Count++;
            SaveArrPoints[SP++]=Round(x);
            SaveArrPoints[SP++]=y;
        }
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the midpoint; Bresenham's algorithm to draw a line
//***************************************************************************************
//The algorithm can be extended to cover gradients between 0 and -1 by checking whether y needs to increase or decrease (i.e. dy < 0)
void PlotLineLow(HDC hdc,int x0,int y0,int x1, int y1,COLORREF c)
{
    int Count=0;
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
        SaveArrPoints[SP++]=x;
        SaveArrPoints[SP++]=y;
        Count++;
        if (D > 0)
        {
            y = y + yi;
            D = D + (2 * (dy - dx)) ;
        }
        else
            D = D + 2*dy;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
void PlotLineHigh(HDC hdc,int x0, int y0,int x1, int y1,COLORREF c)
{
    int Count=0;
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
        SaveArrPoints[SP++]=x;
        SaveArrPoints[SP++]=y;
        Count++;
        if(D > 0)
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
void DrawLineMidPoint(HDC hdc,int x0,int y0,int x1,int y1,COLORREF c)
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
//***************************************************************************************
//***************************************************************************************
//I am implementing the parametric line drawing algorithm
//***************************************************************************************
void DrawLineParametric(HDC hdc,int x1,int y1,int x2, int y2,COLORREF c)
{
    int Count=0;
    double dx=x2-x1;
    double dy=y2-y1;
    SetPixel(hdc,x1,y1,c);
    SaveArrPoints[SP++]=x1;
    SaveArrPoints[SP++]=y1;
    Count++;
    for(double t=0; t<1; t+=0.00001)
    {
        int x=x1+t*dx;
        int y=y1+t*dy;
        SetPixel(hdc,Round(x),Round(y),c);
        SaveArrPoints[SP++]=Round(x);
        SaveArrPoints[SP++]=Round(y);
        Count++;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the draw circle algorithm in the direct way
void Draw8Points(HDC hdc,int xc,int yc,int a,int b,COLORREF color)
{
    SetPixel(hdc,xc+a,yc+b,color);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc+a,yc-b,color);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc-b;
    SetPixel(hdc,xc-a,yc+b,color);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc-a,yc-b,color);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc-b;
    SetPixel(hdc,xc+b,yc+a,color);
    SaveArrPoints[SP++]=xc+b;
    SaveArrPoints[SP++]=yc+a;
    SetPixel(hdc,xc+b,yc-a,color);
    SaveArrPoints[SP++]=xc+b;
    SaveArrPoints[SP++]=yc-a;
    SetPixel(hdc,xc-b,yc+a,color);
    SaveArrPoints[SP++]=xc-b;
    SaveArrPoints[SP++]=yc+a;
    SetPixel(hdc,xc-b,yc-a,color);
    SaveArrPoints[SP++]=xc-b;
    SaveArrPoints[SP++]=yc-a;
}

void DrawCircleDirect(HDC hdc,int xc,int yc,double R,COLORREF color)
{
    int Count=0;
    double y=R;//y=0
    for(int x=0; x<y; x++)//x=R; x>0; x-- you choose your own octant
    {
        Draw8Points(hdc,xc,yc,x,y,color);
        Count+=8;
        y=Round(sqrt(R*R-x*x));
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the draw circle polar direct algorithm
void DrawCircleDirectPolar(HDC hdc,int xc,int yc,double R,COLORREF color)
{
    int Count=0;
    double y=0;
    double theta=0;
    double dtheta=1.0/R;
    for(int x=R; x>y ; theta+=dtheta)
    {
        Draw8Points(hdc,xc,yc,Round(x),Round(y),color);
        Count+=8;
        x=R*cos(theta);
        y=R*sin(theta);
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the draw circle polar iterative algorithm
void DrawCirclePolarIterative(HDC hdc,double xc,double yc,double radius,COLORREF c)
{
    int Count=0;
    double dtheta=1.0/radius;
    double cdtheta=cos(dtheta);
    double sdtheta=sin(dtheta);
    double x=0;
    double y=radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
    while(x<y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        Draw8Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the  midpoint;1st Order difference Bresenham's algorithm to draw a circle
void DrawCircleMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0,Count=0;
    int y=radius;
    int d1=1-radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
    while(x<y)
    {
        if(d1<0)
        {
            d1+=(2*x+3);

        }
        else
        {
            d1+=(2*(x-y)+5);
            y--;
        }
        x++;
        Draw8Points(hdc,xc,yc,x,y,c);
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the modified midpoint;Modified 2nd Order difference Bresenham's algorithm to draw a circle
void DrawCircleModifiedMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0,Count=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
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
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the quarter circle filling
void Draw8Lines(HDC hdc,int xc,int yc,int a,int b,int Quarter,COLORREF c)
{
    if(Quarter==1)
    {
        DrawLineMidPoint(hdc,xc,yc,xc+a,yc-b,c);
        DrawLineMidPoint(hdc,xc,yc,xc+b,yc-a,c);
    }
    else if (Quarter==2)
    {
        DrawLineMidPoint(hdc,xc,yc,xc-a,yc-b,c);
        DrawLineMidPoint(hdc,xc,yc,xc-b,yc-a,c);
    }
    else if(Quarter==3)
    {
        DrawLineMidPoint(hdc,xc,yc,xc-a,yc+b,c);
        DrawLineMidPoint(hdc,xc,yc,xc-b,yc+a,c);
    }
    else
    {
        DrawLineMidPoint(hdc,xc,yc,xc+a,yc+b,c);
        DrawLineMidPoint(hdc,xc,yc,xc+b,yc+a,c);
    }
}
void DrawCircleQuarterFillWithLines(HDC hdc,int xc,int yc,int radius,int Quarter,COLORREF c)
{
    int x=0;
    int y=radius;
    Draw8Lines(hdc,xc,yc,x,y,Quarter,c);
    int d=1-radius;
    int d1=3;
    int d2=5-2*radius;
    while(x<y)
    {
        if(d<0)
        {
            d+=d1;
            d1+=2;
            d2+=2;
            x++;
        }
        else
        {
            d+=d2;
            d1+=2;
            d2+=4;
            y--;
            x++;
        }
        Draw8Lines(hdc,xc,yc,x,y,Quarter,c);
    }
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the polar algorithm for ellipse drawing
void Draw4Points(HDC hdc,int xc,int yc,int a,int b,COLORREF c )
{
    SetPixel(hdc,xc+a,yc+b,c);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc+a,yc-b,c);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc-b;
    SetPixel(hdc,xc-a,yc+b,c);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc-a,yc-b,c);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc-b;
}
int Max(int r1,int r2)
{
    if(r1>r2)
        return r1;
    return r2;
}
void DrawEllipsePolar(HDC hdc,int xc,int yc,int r1,int r2,COLORREF c)
{
    int Count=0;
    double dtheta=1.0/Max(r1,r2);
    double x=r1;
    double y=0;
    double theta=0;
    for( ; x*r2*r2>y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=4;
        x=(r1*cos(theta));
        y=(r2*sin(theta));
    }
    x=0;
    y=r2;
    for( ; x*r2*r2<y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=4;
        x=(r1*cos(theta));
        y=(r2*sin(theta));
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;

}
//***************************************************************************************
//***************************************************************************************
//I am implementing the direct algorithm to draw an ellipse
void DrawEllipseDirect(HDC hdc,int xc,int yc,int r1,int r2,COLORREF c)
{
    int Count=4;
    double x=0;
    double y=r2;
    Draw4Points(hdc,xc,yc,x,y,c);
    while(x*r2*r2<y*r1*r1)
    {
        x++;
        y=sqrt(1-(x*x)/(r1*r1))*r2;
        Draw4Points(hdc,xc,yc,x,Round(y),c);
        Count+=4;
    }
    x=r1;
    y=0;
    while(y*r1*r1<x*r2*r2)
    {
        y++;
        x=sqrt(1-(y*y)/(r2*r2))*r1;
        Draw4Points(hdc,xc,yc,Round(x),y,c);
        Count+=4;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the clipping point algorithm
void PointClipping(HDC hdc,int x,int y,int xleft,int ytop,int xright,int ybottom,COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)
    {
        SetPixel(hdc,x,y,color);
        SaveArrPoints[SP++]=x;
        SaveArrPoints[SP++]=y;
        SaveArrColorPoints[SC++]=color;
        SaveArrNoOfPoints[SN++]=1;
    }
}
//***************************************************************************************
//***************************************************************************************
//I am implementing the line clipping Cohen Sutherland algorithm
union OutCode
{
    unsigned All:4;
    struct
    {
        unsigned left:1,top:1,right:1,bottom:1;
    };
};
OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;
    if(x<xleft)out.left=1;
    else if(x>xright)out.right=1;
    if(y<ytop)out.top=1;
    else if(y>ybottom)out.bottom=1;
    return out;
}
void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
{
    *xi=x;
    *yi=ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
{
    *yi=y;
    *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}
void CohenSuth(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom,COLORREF c)
{
    double x1=xs,y1=ys,x2=xe,y2=ye;
    OutCode out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    while( (out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi,yi;
        if(out1.All)
        {
            if(out1.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out1.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out1.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x1=xi;
            y1=yi;
            out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
        }
        else
        {
            if(out2.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out2.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out2.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x2=xi;
            y2=yi;
            out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
        }
    }
    if(!out1.All && !out2.All)
    {
        DrawLineMidPoint(hdc,Round(x1),Round(y1),Round(x2),Round(y2),c);
    }
}
//***************************************************************************************
//***************************************************************************************

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
HMENU hMenu;
void AddMenus(HWND hwnd);
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

    //here we change the cursor shape
    wincl.hCursor = LoadCursor (NULL, IDC_HAND);

    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */

    //here we change the background color to white
    wincl.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

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
static int xs,ys,xe,ye,r=255,g=0,b=0,xe2,ye2,x,y,x2,y2,xleft,xright,ytop,ybottom;
bool flag=0;
bool flag2=0;
bool SaveFlag=0;
ofstream outdata; // outdata is like cin
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)                  /* handle the messages */
    {
    case WM_COMMAND:
    {
        switch(wParam)
        {
        case 1://to save the screen data
        {
            SaveFlag=1;
            outdata.open("SaveFile.txt"); // opens the file
            if( !outdata )   // file couldn't be opened
            {
                cout<< "Error: file could not be opened" << endl;
                exit(1);
            }
            outdata<<"The points of the shapes drawn are: "<<endl;
            for (int i=0; i<(SP); ++i)
                outdata << SaveArrPoints[i] << endl;
            outdata<<"The numbers of the points drawn are: "<<endl;
            for(int i=0; i<SN; i++)
                outdata<<SaveArrNoOfPoints[i]<<endl;
            outdata<<"The colors of the shapes drawn are: "<<endl;
            for(int i=0; i<SC; i++)
                outdata<<SaveArrColorPoints[i]<<endl;
            outdata.close();
            break;
        }
        case 2://to load the screen data
        {
            if(SaveFlag)
            {
                HDC hdc=GetDC(hwnd);
                int j=0,i=0,z=0;
                for(j; j<(SP+2); j+=2,z++)
                {
                    SetPixel(hdc,SaveArrPoints[j],SaveArrPoints[j+1],SaveArrColorPoints[i]);
                    if(z==SaveArrNoOfPoints[i])
                    {
                        i++;
                        z=0;
                    }
                }
                SaveFlag=0;//I am doing this because I only want it to save the current data not accumulative or previous saved data, once the user
                SN=SP=SC=0;//saves new data, it overwrites the previous data
                outdata.open("SaveFile.txt");
                if( !outdata )   // file couldn't be opened
                {
                    cout<< "Error: file could not be opened" << endl;
                    exit(1);
                }
                outdata<<" "<<endl;
                outdata.close();
                ReleaseDC(hwnd,hdc);
            }
            break;
        }
        case 3:
        {
            if(!SaveFlag)
            {
                SN=SP=SC=0;
                outdata.open("SaveFile.txt");
                if( !outdata )   // file couldn't be opened
                {
                    cout<< "Error: file could not be opened" << endl;
                    exit(1);
                }
                outdata<<" "<<endl;
                outdata.close();
            }
            InvalidateRect(hwnd, NULL, TRUE);//to clear the screen from data
            break;
        }
        case 4:
        {
            HDC hdc=GetDC(hwnd);
            DrawLineDDA(hdc,xs,ys,xe,ye,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 5:
        {
            HDC hdc=GetDC(hwnd);
            DrawLineMidPoint(hdc,xs,ys,xe,ye,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 6:
        {
            HDC hdc=GetDC(hwnd);
            DrawLineParametric(hdc,xs,ys,xe,ye,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 7:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            DrawCircleDirect(hdc,xs,ys,radius,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 8:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            DrawCircleDirectPolar(hdc,xs,ys,radius,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 9:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            DrawCirclePolarIterative(hdc,xs,ys,radius,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 10:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            DrawCircleMidpoint(hdc,xs,ys,radius,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 11:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            DrawCircleModifiedMidpoint(hdc,xs,ys,radius,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 12:
        {
            HDC hdc=GetDC(hwnd);
            double radius=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            int Quarter;
            cout<<"********************************************************************************************"<<endl;
            cout<<"Enter the desired Quarter number.IN RANGE (1-4)"<<endl;
            cin>>Quarter;
            if(Quarter<1 || Quarter>4)
            {
                cout<<"********************************************************************************************"<<endl;
                cout<<"ERROR:::ONLY ENTER NUMBERS IN RANGE (1-4)"<<endl;
                cin>>Quarter;
            }
            DrawCircleQuarterFillWithLines(hdc,xs,ys,radius,Quarter,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 13:
        {
            HDC hdc=GetDC(hwnd);
            double radius1=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            double radius2=sqrt((xe2-xs)*(xe2-xs)+(ye2-ys)*(ye2-ys));
            DrawEllipseDirect(hdc,xs,ys,radius1,radius2,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 14:
        {
            HDC hdc=GetDC(hwnd);
            double radius1=sqrt((xe-xs)*(xe-xs)+(ye-ys)*(ye-ys));
            double radius2=sqrt((xe2-xs)*(xe2-xs)+(ye2-ys)*(ye2-ys));
            DrawEllipsePolar(hdc,xs,ys,radius1,radius2,RGB(r,g,b));
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 15:
        {
            HDC hdc=GetDC(hwnd);
            DrawLineMidPoint(hdc,xs,ys,xe,ys,RGB(r,g,b));
            DrawLineMidPoint(hdc,xs,ys,xs,ye2,RGB(r,g,b));
            DrawLineMidPoint(hdc,xe,ys,xe,ye2,RGB(r,g,b));
            DrawLineMidPoint(hdc,xs,ye2,xe,ye2,RGB(r,g,b));
            flag=1;
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 16:
        {
            HDC hdc=GetDC(hwnd);
            DrawLineMidPoint(hdc,xs,ys,xe,ys,RGB(r,g,b));
            DrawLineMidPoint(hdc,xs,ys,xs,ye2,RGB(r,g,b));
            DrawLineMidPoint(hdc,xe,ys,xe,ye2,RGB(r,g,b));
            DrawLineMidPoint(hdc,xs,ye2,xe,ye2,RGB(r,g,b));
            flag2=1;
            ReleaseDC(hwnd,hdc);
            break;
        }
        case 17:
        {
            r=255;
            g=0;
            b=0;
            break;
        }
        case 18:
        {
            r=0;
            g=255;
            b=0;
            break;
        }
        case 19:
        {
            r=0;
            g=0;
            b=255;
            break;
        }
        case 20:
        {
            r=255;
            g=255;
            b=255;
            break;
        }
        case 21:
        {
            r=0;
            g=0;
            b=0;
            break;
        }
        case 22:
        {
            r=160;
            g=160;
            b=160;
            break;
        }
        case 23:
        {
            r=0;
            g=204;
            b=204;
            break;
        }
        case 24:
        {
            r=102;
            g=51;
            b=0;
            break;
        }
        case 25:
        {
            r=255;
            g=52;
            b=255;
            break;
        }
        case 26:
        {
            r=127;
            g=0;
            b=255;
            break;
        }
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        xleft=xs=LOWORD(lParam);
        ytop=ys=HIWORD(lParam);
        break;
    }
    case WM_LBUTTONUP:
    {
        xright=xe=LOWORD(lParam);
        ye=HIWORD(lParam);
        break;
    }
    case WM_RBUTTONDBLCLK:
    {
        xe2=LOWORD(lParam);
        ybottom=ye2=HIWORD(lParam);
        break;
    }
    case WM_RBUTTONDOWN:
    {
        HDC hdc=GetDC(hwnd);
        x=LOWORD(lParam);
        y=HIWORD(lParam);
        if(flag)
            PointClipping(hdc,x,y,xleft,ytop,xright,ybottom,RGB(r,g,b));
        ReleaseDC(hwnd,hdc);
        break;

    }
    case WM_RBUTTONUP:
    {
        HDC hdc=GetDC(hwnd);
        x2=LOWORD(lParam);
        y2=HIWORD(lParam);
        if(flag2)
            CohenSuth(hdc,x,y,x2,y2,xleft,ytop,xright,ybottom,RGB(r,g,b));
        ReleaseDC(hwnd,hdc);
        break;

    }
    case WM_CREATE:
    {
        AddMenus(hwnd);
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
void AddMenus(HWND hwnd)
{
    hMenu=CreateMenu();

    HMENU hFileMenu=CreateMenu();
    AppendMenu(hFileMenu,MF_STRING,1,"Save");
    AppendMenu(hFileMenu,MF_STRING,2,"Load");
    AppendMenu(hFileMenu,MF_STRING,3,"Clear");
    AppendMenu(hFileMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)hFileMenu,"File");

    HMENU LineMenu=CreateMenu();
    AppendMenu(LineMenu,MF_STRING,4,"DDA");
    AppendMenu(LineMenu,MF_STRING,5,"MidPoint");
    AppendMenu(LineMenu,MF_STRING,6,"Parametric");
    AppendMenu(LineMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)LineMenu,"Line");

    HMENU CircleMenu=CreateMenu();
    AppendMenu(CircleMenu,MF_STRING,7,"Direct");
    AppendMenu(CircleMenu,MF_STRING,8,"Polar");
    AppendMenu(CircleMenu,MF_STRING,9,"Iterative Polar");
    AppendMenu(CircleMenu,MF_STRING,10,"Midpoint");
    AppendMenu(CircleMenu,MF_STRING,11,"Modified Midpoint");
    AppendMenu(CircleMenu,MF_STRING,12,"Fill a Quarter");
    AppendMenu(CircleMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)CircleMenu,"Circle");

    HMENU EllipseMenu=CreateMenu();
    AppendMenu(EllipseMenu,MF_STRING,13,"Direct");
    AppendMenu(EllipseMenu,MF_STRING,14,"Polar");
    AppendMenu(EllipseMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)EllipseMenu,"Ellipse");

    HMENU ClippingMenu=CreateMenu();
    AppendMenu(ClippingMenu,MF_STRING,15,"Point");
    AppendMenu(ClippingMenu,MF_STRING,16,"Line");
    AppendMenu(ClippingMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)ClippingMenu,"Clipping");

    HMENU ColorMenu=CreateMenu();
    AppendMenu(ColorMenu,MF_STRING,17,"Red");
    AppendMenu(ColorMenu,MF_STRING,18,"Green");
    AppendMenu(ColorMenu,MF_STRING,19,"Blue");
    AppendMenu(ColorMenu,MF_STRING,20,"White");
    AppendMenu(ColorMenu,MF_STRING,21,"Black");
    AppendMenu(ColorMenu,MF_STRING,22,"Grey");
    AppendMenu(ColorMenu,MF_STRING,23,"Light Blue");
    AppendMenu(ColorMenu,MF_STRING,24,"Brown");
    AppendMenu(ColorMenu,MF_STRING,25,"Pink");
    AppendMenu(ColorMenu,MF_STRING,26,"Violet");
    AppendMenu(ColorMenu,MF_SEPARATOR,NULL,NULL);

    AppendMenu(hMenu,MF_POPUP,(UINT_PTR)ColorMenu,"Color");

    SetMenu(hwnd,hMenu);
}
