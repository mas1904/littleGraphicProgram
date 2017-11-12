#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <iostream>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#pragma comment( lib, "gdiplus.lib" )
#include <gdiplus.h>
#include <fstream>
#include <comdef.h>
#include <commctrl.h>
#include <sstream>
#include <string>
#include <vector>
#include "dialogs.h"
#include "include/cImage.h"

#define ID_OPEN 501
#define ID_SAVE_AS_PPM 502
#define ID_SAVE_AS_JPG 503

#define ID_CANVAS_UP 401
#define ID_CANVAS_DOWN 402

#define ID_ADD_VALUES 604
#define ID_COLOR_DESTURYZATION 605
#define ID_CHANGE_BRIGHT 606
#define ID_FILTER_MED 607
#define ID_FILTER_MEDIAN 608
#define ID_FILTER_SOBEL 609
#define ID_FILTER_GAUSS 610
#define ID_FILTER_UP_SHARP 611
#define ID_FILTER_MASK_WEAVE 612
#define ID_HISTOGRAM 613

#define ID_MORFOLOGY_DYL 614
#define ID_MORFOLOGY_ERO 615
#define ID_MORFOLOGY_OPEN 616
#define ID_MORFOLOGY_CLOSE 617
#define ID_MORFOLOGY_HIT 618

using namespace Gdiplus;
using namespace std;



INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgQualityProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgAddProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgBrightProc (HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK DlgFilterProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgFilterSobelProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgColorDesturyzationProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgFilterGaussProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgFilterUpSharpenProc (HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK DlgHistogramProc (HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK DlgMorfologyHitProc (HWND, UINT, WPARAM, LPARAM);


/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");
TCHAR szCanvasClassName[ ] = _T("Canvas");

int num_of_images;


vector<cImage*> images;
vector<int> que;
vector<tuple<int,int,int>> strukt;

HWND hCanvas, hwnd;

HDC canvasHdc;

HMENU hMenu = CreateMenu();
HMENU hFile = CreateMenu();
HMENU hSaveAs = CreateMenu();
HMENU hColors = CreateMenu();
HMENU hFilters = CreateMenu();
HMENU hMorfologyFilters = CreateMenu();


int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
    UINT  num = 0;          // number of image encoders
    UINT  size = 0;         // size of the image encoder array in bytes

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if(size == 0)
        return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL)
        return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j)
    {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
        {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

void SaveFile(char *filename)
{
    cout << filename << endl;

    BITMAP bitmap = images[0]->bitmap;
    if(bitmap.bmWidth<=0 || bitmap.bmHeight <= 0)
    {
        MessageBox (NULL, "Brak danych do zapisania.", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
        return;
    }
    fstream file;
    file.open(filename, ios::out | ios::binary);


    HDC tempIm = CreateCompatibleDC(canvasHdc);
    HBITMAP hOldIm = (HBITMAP) SelectObject(tempIm, images[0]->hBitmap);

    HDC temp = CreateCompatibleDC(canvasHdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(canvasHdc, bitmap.bmWidth, bitmap.bmHeight);
    HBITMAP hOld = (HBITMAP)SelectObject(temp, hBitmap);
    BitBlt(temp, 0, 0, bitmap.bmWidth, bitmap.bmHeight, tempIm, 0, 0, SRCCOPY);
    SelectObject(temp, hOld);
    SelectObject(tempIm, hOldIm);

    cout << bitmap.bmBitsPixel << endl;

    BITMAPINFOHEADER bmInfo = {0};
    bmInfo.biSize = sizeof(BITMAPINFOHEADER);
    bmInfo.biPlanes = 1;
    bmInfo.biBitCount = 32;
    bmInfo.biWidth = bitmap.bmWidth;
    bmInfo.biHeight = -bitmap.bmHeight;
    bmInfo.biCompression = BI_RGB;
    bmInfo.biSizeImage = 0;

    int buf_size = bitmap.bmWidth*bitmap.bmHeight*4;
    cout << buf_size << endl;
    int row_size = bitmap.bmWidth*4;
    cout << row_size << endl;
    int a_buf_size=row_size;
    if(row_size <= 20000)
        a_buf_size *= 20000/row_size;
    cout << a_buf_size << endl;
    int num_of_rows_in_one = a_buf_size/row_size;

    if(num_of_rows_in_one>bitmap.bmHeight)
        num_of_rows_in_one = bitmap.bmHeight;

    cout << num_of_rows_in_one << endl;

    if(a_buf_size>buf_size)
        a_buf_size=buf_size;

    cout << a_buf_size << endl;

    int index_rows = bitmap.bmHeight-num_of_rows_in_one;

    char bufor[a_buf_size];
    char rev_bufor[a_buf_size];
    file << "P6" << endl << "#Created By L 07" << endl << bmInfo.biWidth << " " << bitmap.bmHeight << endl << "255" << endl;;

    int j=0;

    cout << row_size << "  -  " << num_of_rows_in_one << " " << bitmap.bmHeight << " " << a_buf_size << " " << endl;
    while(buf_size>0)
    {
        GetDIBits(temp, hBitmap, index_rows, num_of_rows_in_one, bufor, (BITMAPINFO*)&bmInfo, DIB_RGB_COLORS);
        index_rows-=num_of_rows_in_one;
        for(int i=0; i<a_buf_size; i+=4)
        {
            rev_bufor[j]=bufor[i+2];
            rev_bufor[j+1]=bufor[i+1];
            rev_bufor[j+2]=bufor[i];
            j+=3;
        }
        file.write(rev_bufor, j);
        j=0;
        buf_size-=a_buf_size;
        if(index_rows<=0)
        {
            num_of_rows_in_one = index_rows+num_of_rows_in_one;
            a_buf_size = num_of_rows_in_one*4*bitmap.bmWidth;
            index_rows = 0;
        }
    }

    file.close();

    DeleteDC(temp);
    DeleteDC(tempIm);
    DeleteObject(hBitmap);

    MessageBox (NULL, "Zapisano.", "Komunikat", MB_ICONASTERISK);

}

bool pReadFile(char *filename)
{

    string t = filename;

    if(t.find(".ppm", t.size()-4)!=t.npos )
    {
        fstream file;
        file.open(t.c_str(), ios::in | ios::binary);

        int x=0, y=0, depth=0;
        int num_of_bytes=3;
        string date;
        char buff[255];
        while((x==0 || y ==0 || depth == 0 )&& !file.eof())
        {
            file >> date;
            if(date.find("#")!=date.npos)
            {
                file.getline(buff, 255);
            }
            //cout << buff << endl;

            if(is_number(date))
            {
                if(x==0) x = stoi(date);
                else if(y==0) y = stoi(date);
                else
                {
                    depth = stoi(date);
                    if(depth > 255)
                        num_of_bytes = 6;
                }
            }
        }
        if(file.eof())
        {
            //komunikat bledu
            MessageBox (NULL, "Brak danych do wczytania.", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
            return false;
        }
        if(x==0||y==0||depth==0)
        {
            MessageBox (NULL, "Brak wszystkich wymiarów.", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
            return false;
        }
        cout << x << " " << y << " " << depth <<  endl;
        int image_height = y;
        int image_width = x;


        double color_depth = (double)depth/255;
        char bufor[768];

        file.read(bufor, 1); //end of line

        int row=0, indeks=0;
        images.push_back(new cImage);
        que.push_back(images.size()-1);

        images[images.size()-1] = new cImage();

        images[images.size()-1]->image = (COLORREF*) calloc(x*y, sizeof(COLORREF));

        while(row*indeks!=x*y && !file.eof())
        {
            file.read(bufor, 768);
            if(file.gcount()%num_of_bytes!=0)
            {
                MessageBox (NULL, "Nieodpowiednia ilosc danych.", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
                return false;
            }
            if(num_of_bytes==3)
                for(int i=0; i<file.gcount(); i+=3)
                {
                    images[images.size()-1]->image[indeks + row*x] = RGB(bufor[i+2]/color_depth, bufor[i+1]/color_depth, bufor[i]/color_depth);
                    if(++indeks%x==0)
                    {
                        ++row;
                        if(row==y)
                            break; // wczytano wszystko
                        indeks=0;
                    }
                }
            else if(num_of_bytes==6)
                for(int i=0; i<file.gcount(); i+=6)
                {
                    images[images.size()-1]->image[indeks + row*x] = RGB((bufor[i+4]*256+bufor[i+5])/color_depth, (bufor[i+2]*256+bufor[i+3])/color_depth, (bufor[i]*256+bufor[i+1])/color_depth);
                    if(++indeks%x==0)
                    {
                        ++row;
                        if(row==y)
                            break; // wczytano wszystko
                        indeks=0;
                    }
                }
        }
        if(row*indeks!=x*y)
        {
            MessageBox (NULL, "Za malo wczytanych danych.", "Error", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
            return false;
        }

        file.close();

        RECT window, canvas;
        if(images.size()==1)
        {
            GetClientRect(hwnd, &window);
            SetRect(&canvas, (window.right-image_width)/2, (window.bottom-image_height)/2, image_width, image_height);
            MoveWindow(hCanvas, canvas.left, canvas.top, canvas.right, canvas.bottom, TRUE );
        }

        images[images.size()-1]->setHBitmap(image_width, image_height);

        images[images.size()-1]->pos_x = 0;
        images[images.size()-1]->pos_y = 0;




        InvalidateRect(hCanvas,NULL,TRUE);
        UpdateWindow(hCanvas);
        return true;
    }
}

HDC temp, hdcBufor;
HBITMAP hbOld, hbBuf, hbOldBuf;
RECT window;

ULONG_PTR           gdiplusToken;
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    setlocale( LC_ALL, "" );
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;

    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    for(int i=-2;i<=2;i++)
        for(int j=-2;j<=2;j++){
                if(i>-2&&i<2&&j>-2&&j<2)
                    strukt.push_back(make_tuple(i,j,1));
                else strukt.push_back(make_tuple(i,j,2));
        }

    HDC hdc;
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    wincl.hbrBackground = (HBRUSH) GetStockObject(COLOR_BACKGROUND);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;


    WNDCLASSEX canvasWcl;

    canvasWcl.hInstance = hThisInstance;
    canvasWcl.lpszClassName = szCanvasClassName;
    canvasWcl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    canvasWcl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    canvasWcl.hIcon = NULL;
    canvasWcl.hIconSm = NULL;
    canvasWcl.hCursor = LoadCursor (NULL, IDC_ARROW);
    canvasWcl.lpszMenuName = NULL;                 /* No menu */
    canvasWcl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    canvasWcl.cbWndExtra = 0;                      /* structure or the window instance */
    canvasWcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    canvasWcl.style = CS_OWNDC;


    if (!RegisterClassEx (&canvasWcl))
    {
        return 0;

    }

    AppendMenuW(hSaveAs, MF_STRING, ID_SAVE_AS_JPG, L"&JPEG");
    AppendMenuW(hSaveAs, MF_STRING, ID_SAVE_AS_PPM, L"&PPM");

    AppendMenuW(hFile, MF_STRING, ID_OPEN, L"&Otworz");
    AppendMenuW(hFile, MF_POPUP, (UINT_PTR)hSaveAs, L"&Zapisz jako...");

    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFile, L"&Plik");

    AppendMenuW(hColors, MF_STRING, ID_CHANGE_BRIGHT, L"&Zmien jasnosc");
    AppendMenuW(hColors, MF_STRING, ID_ADD_VALUES, L"&Dodaj wartosci");
    AppendMenuW(hColors, MF_STRING, ID_COLOR_DESTURYZATION, L"&Desturyzacja");
    AppendMenuW(hColors, MF_STRING, ID_HISTOGRAM, L"&Histogram");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hColors, L"&Kolory");

    AppendMenuW(hFilters, MF_STRING, ID_FILTER_GAUSS, L"&Rozmycie Gaussa");
    AppendMenuW(hFilters, MF_STRING, ID_FILTER_SOBEL, L"&Sobel");
    AppendMenuW(hFilters, MF_STRING, ID_FILTER_UP_SHARP, L"&Wyostrzajacy");
    AppendMenuW(hFilters, MF_STRING, ID_FILTER_MEDIAN, L"&Medianowy");
    AppendMenuW(hFilters, MF_STRING, ID_FILTER_MED, L"&Usredniajacy");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hFilters, L"&Filtry");

    AppendMenuW(hMorfologyFilters, MF_STRING, ID_MORFOLOGY_DYL, L"&Dylatacja");
    AppendMenuW(hMorfologyFilters, MF_STRING, ID_MORFOLOGY_ERO, L"&Erozja");
    AppendMenuW(hMorfologyFilters, MF_STRING, ID_MORFOLOGY_OPEN, L"&Otwarcie");
    AppendMenuW(hMorfologyFilters, MF_STRING, ID_MORFOLOGY_CLOSE, L"&Domkniecie");
    AppendMenuW(hMorfologyFilters, MF_STRING, ID_MORFOLOGY_HIT, L"&Hit or Miss");
    AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hMorfologyFilters, L"&Filtry morfologiczne");


    EnableMenuItem(hFile, (UINT_PTR)hSaveAs,  MF_GRAYED);


    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilites for variation */
               szClassName,         /* Classname */
               _T("Zad1"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               1040,                 /* The programs width */
               720,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               hMenu,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );

    hdc = GetWindowDC(hwnd);

    hCanvas = CreateWindowEx( 0, szCanvasClassName, NULL, WS_CHILD | WS_VISIBLE,
                              0, 0, 1024, 720, hwnd, NULL, hThisInstance, NULL  );

    HWND bCanvas_up = CreateWindowEx( 0, "BUTTON", "UP", WS_CHILD | WS_VISIBLE,
                                      1040, 100, 150, 30, hwnd, ( HMENU )ID_CANVAS_UP, hThisInstance, NULL );

    HWND bCanvas_down = CreateWindowEx( 0, "BUTTON", "DOWN", WS_CHILD | WS_VISIBLE,
                                        1040, 140, 150, 30, hwnd, ( HMENU )ID_CANVAS_DOWN, hThisInstance, NULL );

    canvasHdc = GetWindowDC(hCanvas);



    GetClientRect(hCanvas, &window);


    cout << " === " << endl;

    cout << GetPixel(hdcBufor, 50, 50) << endl;


    hdcBufor = CreateCompatibleDC(canvasHdc);
    hbBuf = CreateCompatibleBitmap( canvasHdc, window.right, window.bottom );
    hbOldBuf =( HBITMAP ) SelectObject( hdcBufor, hbBuf );


    temp = CreateCompatibleDC(canvasHdc);


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


cImage *selected;
int selected_id;
int mouse_x, mouse_y;
string filterName="Usredniajacy";

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
    case WM_DESTROY:
    {
        Gdiplus::GdiplusShutdown(gdiplusToken);
        SelectObject(hdcBufor, hbOldBuf);
        SelectObject(temp, hbOld);
        DeleteDC( temp );
        DeleteDC( hdcBufor );
        DeleteObject( hbOld );
        DeleteObject( hbOldBuf );
        for(int i=0; i<images.size(); i++)
            DeleteObject( images[i]->hBitmap );

        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
    }
        break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hwnd, &ps);

        SelectObject( hdcBufor, hbBuf );
        FillRect( hdcBufor, & window,( HBRUSH ) GetStockObject( WHITE_BRUSH ) );
        for(int i=0; i<que.size(); i++)
        {
            SelectObject(temp, images[que[i]]->hBitmap);
            BitBlt(hdcBufor, images[que[i]]->pos_x, images[que[i]]->pos_y, images[que[i]]->width, images[que[i]]->height, temp, 0, 0, images[que[i]]->operation  );
        }
        BitBlt(canvasHdc, 0, 0, window.right, window.bottom, hdcBufor, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
    }
        break;
    case WM_COMMAND:
        switch(wParam)
        {
            case ID_CANVAS_UP:
                if(selected_id<que.size()-1)
                {
                    int temp = que[selected_id+1];
                    que[selected_id+1]=que[selected_id];
                    que[selected_id] = temp;
                    selected_id++;
                    InvalidateRect(hCanvas,NULL,TRUE);
                }
                break;
            case ID_CANVAS_DOWN:
                if(selected_id>0)
                {
                    int temp = que[selected_id-1];
                    que[selected_id-1]=que[selected_id];
                    que[selected_id] = temp;
                    selected_id--;
                    InvalidateRect(hCanvas,NULL,TRUE);
                }
                break;
            case ID_OPEN:
            {
                OPENFILENAME ofn = {};
                char sFileName[ MAX_PATH ] = "";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.lpstrFile = sFileName;
                ofn.lpstrFilter = "Wszystkei obslugiwane(*.ppm, *.jpg, *.jpeg, *.jpe)\0*.ppm; *.jpg; *.jpeg; *.jpe\0Pliki PPM (*.ppm)\0*.ppm\0Pliki JPEG (*.jpg, *.jpeg, *.jpe)\0*.jpg; *.jpeg; *.jpe\0";
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrDefExt ="ppm";
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
                if(GetOpenFileName(&ofn))
                {
                    const size_t cSize = strlen(sFileName)+1;
                    wstring wc( cSize, L'#');
                    mbstowcs(&wc[0], sFileName, cSize);
                    string temp = sFileName;

                    if(temp.find(".ppm",temp.size()-4)!=temp.npos)
                    {
                        if(pReadFile(sFileName))
                            EnableMenuItem(hFile, (UINT_PTR)hSaveAs,  MF_ENABLED);
                    }
                    else
                    {
                        Gdiplus::Bitmap bmp(wc.c_str());
                        const Gdiplus::Color color = Gdiplus::Color(0,0,0);
                        images.push_back(new cImage);
                        que.push_back(images.size()-1);
                        images[images.size()-1]->operation = SRCCOPY;
                        bmp.GetHBITMAP(Gdiplus::Color(255,255,255), &(images[images.size()-1]->hBitmap));
                        images[images.size()-1]->setBitmap();

                        RECT window, canvas;
                        if(images.size()==1)
                        {
                            GetClientRect(hwnd, &window);
                            int cwidth = images[images.size()-1]->width;
                            int cheight = images[images.size()-1]->height;
                            int left = (window.right-images[images.size()-1]->width)/2;
                            int top = (window.bottom-images[images.size()-1]->height)/2;
                            if(cwidth>window.right) cwidth = window.right;
                            if(cheight>window.bottom) cheight = window.bottom;
                            if(left<0)left=0;
                            if(top<0)top=0;


                            SetRect(&canvas, left, top, cwidth, cheight);
                            MoveWindow(hCanvas, canvas.left, canvas.top, canvas.right, canvas.bottom, TRUE );
                        }

                        if(images[images.size()-1]->height!=0 && images[images.size()-1]->width != 0)
                        {
                            images[images.size()-1]->pos_x = 0;
                            images[images.size()-1]->pos_y = 0;
                            Gdiplus::BitmapData bitmapData;
                            Gdiplus::Rect rect(0,0, images[images.size()-1]->width,images[images.size()-1]->height);
                            if(Gdiplus::Ok == bmp.LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bitmapData ))
                            {
                                images[images.size()-1]->image = (COLORREF*) calloc(bitmapData.Height * bitmapData.Width, sizeof(COLORREF));
                                int len = bitmapData.Height * bitmapData.Width * sizeof(COLORREF);
                                memcpy( images[images.size()-1]->image, bitmapData.Scan0, len);
                                bmp.UnlockBits(&bitmapData);
                                images[images.size()-1]->setHBitmap(bitmapData.Width, bitmapData.Height);
                                images[images.size()-1]->setBitmap();
                                InvalidateRect(hCanvas,NULL,TRUE);
                                EnableMenuItem(hFile, (UINT_PTR)hSaveAs,  MF_ENABLED);
                            }
                        }
                    }

                }
            }
                break;
            case ID_SAVE_AS_PPM:
            {
                OPENFILENAME ofn;
                char sFileName[ MAX_PATH ] = "";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.lpstrFilter = "Plik PPM (*.ppm)\0*.ppm\0";
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFile = sFileName;
                ofn.lpstrDefExt = "ppm";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

                if(GetSaveFileName(&ofn))
                {
                    SaveFile(sFileName);
                }
            }
                break;
            case ID_SAVE_AS_JPG:
            {
                OPENFILENAMEW ofn;
                wchar_t sFileName[ MAX_PATH ] = L"";
                ZeroMemory(&ofn, sizeof(ofn));
                ofn.lStructSize = sizeof(ofn);
                ofn.lpstrFilter = L"Pliki JPEG (*.jpg, *.jpeg, *.jpe)\0*.jpg; *.jpeg; *.jpe\0";
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrFile = sFileName;
                ofn.lpstrDefExt = L"jpeg";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

                if(GetSaveFileNameW(&ofn))
                {
                    CLSID encoderClsid;
                    Gdiplus::EncoderParameters encoderParameters;
                    ULONG quality = 100;
                    GetEncoderClsid(L"image/jpeg", &encoderClsid);
                    encoderParameters.Count = 2;
                    encoderParameters.Parameter[0].Guid = EncoderQuality;
                    encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
                    encoderParameters.Parameter[0].NumberOfValues = 1;



                    quality = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_QUALITY), hwnd,  DlgQualityProc);
                    if(quality<0)
                        break;

                    encoderParameters.Parameter[0].Value = &quality;
                    Gdiplus::Bitmap bmp((images[0]->hBitmap),(HPALETTE)0);


                    if(bmp.Save(sFileName, &encoderClsid, &encoderParameters) == Ok)
                        MessageBoxW (NULL, L"Zapisano.", L"Komunikat", MB_ICONASTERISK);
                    else
                        MessageBoxW (NULL, L"Zapis nieudany.", L"Komunikat", MB_ICONERROR | MB_SYSTEMMODAL | MB_SETFOREGROUND);
                }
            }
                break;
            case ID_ADD_VALUES:
            {
                if(images.size()>0)
                {
                    images[que[selected_id]]->makeCopy();
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_ADD), hwnd,  DlgAddProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                        cout << response << endl;

                    }
                }

            }
                break;
            case ID_CHANGE_BRIGHT:
            {
                if(images.size()>0)
                {
                    images[que[selected_id]]->makeCopy();
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_BRIGHTNESS), hwnd,  DlgBrightProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }

            }
                break;
            case ID_FILTER_MED:
            {
                if(images.size()>0)
                {
                    filterName="Usredniajacy";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterMed(3);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_FILTER_MEDIAN:
            {
                if(images.size()>0)
                {
                    filterName="Medianowy";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterMedian();
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_FILTER_SOBEL:
            {
                if(images.size()>0)
                {
                    filterName="Sobel";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterSobel(0);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER_SOBEL), hwnd,  DlgFilterSobelProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_COLOR_DESTURYZATION:
            {
                if(images.size()>0)
                {
                    filterName="Sobel";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->colorDesturyzation(0);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_COLOR_DESTURYZATION), hwnd,  DlgColorDesturyzationProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_FILTER_UP_SHARP:
            {
                if(images.size()>0)
                {
                    filterName="Gornoprzepustowy wyostrzajacy";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterUpSharpen(0);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER_UP_SHARPEN), hwnd,  DlgFilterUpSharpenProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_FILTER_GAUSS:
            {
                if(images.size()>0)
                {
                    filterName="Rozmycie Gaussa";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterGauss(3,16);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER_GAUSS), hwnd,  DlgFilterGaussProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
            }
                break;
            case ID_FILTER_MASK_WEAVE:
            {
                if(images.size()>0)
                {

                }
            }
                break;
            case ID_HISTOGRAM:
            {
                if(images.size()>0)
                {
                    images[que[selected_id]]->makeCopy();
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_HISTOGRAM), hwnd,  DlgHistogramProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }

            }
                break;
            case ID_MORFOLOGY_DYL:
                if(images.size()>0)
                {
                    filterName="Dylatacja";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfology(3,true);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
                break;
            case ID_MORFOLOGY_ERO:
                if(images.size()>0)
                {
                    filterName="Erozja";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfology(3,false);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
                break;
            case ID_MORFOLOGY_OPEN:
                if(images.size()>0)
                {
                    filterName="Otwarcie";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfologyOpen(3,true);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
                break;
            case ID_MORFOLOGY_CLOSE:
                if(images.size()>0)
                {
                    filterName="Domkniecie";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfologyOpen(3,false);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_FILTER), hwnd,  DlgFilterProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
                break;
            case ID_MORFOLOGY_HIT:
                if(images.size()>0)
                {
                    filterName="Pocienianie";
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfologyHit(5,true,strukt);
                    InvalidateRect(hCanvas,NULL,TRUE);
                    int response = DialogBox( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG_MORFOLOGY_HIT), hwnd,  DlgMorfologyHitProc);
                    if(response==-1)
                    {
                        images[que[selected_id]]->returnCopy();
                        InvalidateRect(hCanvas,NULL,FALSE);
                    }
                }
                break;
        }
        break;
    case WM_LBUTTONDOWN:
        if(hwnd == hCanvas)
        {
            selected = NULL;
            for(int i=que.size()-1; i>=0; i--)
            {
                if(images[que[i]]->ifSelected(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)))
                {
                    selected = images[que[i]];
                    selected_id = i;
                    SetCapture(hwnd);
                    mouse_x=GET_X_LPARAM(lParam)-selected->pos_x;
                    mouse_y=GET_Y_LPARAM(lParam)-selected->pos_y;
                    break;
                }
            }
            if(selected==NULL&&images.size()>0)
                selected = images[que[selected_id]];
        }
        break;

    case WM_LBUTTONUP:
        ReleaseCapture();
        break;

    case WM_MOUSEMOVE:
        if(GetCapture() == hCanvas)
        {

            selected->Move(GET_X_LPARAM(lParam)-mouse_x,GET_Y_LPARAM(lParam)-mouse_y);
            InvalidateRect (hCanvas, NULL, FALSE);

        }


        break;
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
DWORD dwPos;
HWND hwndBuddy;

LRESULT CALLBACK DlgQualityProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        hwndBuddy = CreateWindowEx(0, "EDIT", "0", SS_LEFT | WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                   0, 0, 50, 20, hwnd, NULL, GetModuleHandle(NULL), NULL);

        SendMessage(hwndBuddy, EM_SETLIMITTEXT, 3, 0);
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)0, (UINT)100));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)hwndBuddy);
    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            cout << "dd" << endl;
            EndDialog(hwnd, 1);
        }
        break;

        }
        case EN_CHANGE:
        {
            char buff[4];
            GetWindowText(hwndBuddy,buff,4);
            int num = atoi(buff);
            if(num > 100)
            {
                num=100;
                SetWindowText(hwndBuddy,(to_string(num)).c_str() );
            }
            else if(num < 0)
            {
                num =0;
                SetWindowText(hwndBuddy,(to_string(num)).c_str() );
            }
            SendMessage(GetDlgItem( hwnd, ID_TRACKBAR), TBM_SETPOS, (WPARAM)TRUE, atoi(buff));
            SendMessage(GetDlgItem( hwnd, ID_UPDOWN), UDM_SETPOS, (WPARAM)TRUE, atoi(buff));
        }
        break;
        case WM_HSCROLL:
            dwPos = SendMessage(GetDlgItem( hwnd, ID_TRACKBAR), TBM_GETPOS, 0, 0);
            std::ostringstream stream;
            stream << dwPos;
            string str = stream.str();
            cout << str << endl;
            if(SetWindowText(hwndBuddy,str.c_str())==0)
            {
                cout << GetLastError() << endl;
            }

            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

int rr=0, gg=0, bb=0;
int tr=0, tg=0, tb=0;

LRESULT CALLBACK DlgAddProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        rr=0, gg=0, bb=0;
        tr=0, tg=0, tb=0;

        SetWindowText(GetDlgItem( hwnd, ID_ED_R),"0");
        SetWindowText(GetDlgItem( hwnd, ID_ED_G),"0");
        SetWindowText(GetDlgItem( hwnd, ID_ED_B),"0");

        SendMessage(GetDlgItem( hwnd, ID_ED_R), EM_SETLIMITTEXT, 3, 0);
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_R), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)0, (UINT)255));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_R), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_R));

        SendMessage(GetDlgItem( hwnd, ID_ED_G), EM_SETLIMITTEXT, 3, 0);
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_G), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)0, (UINT)255));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_G), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_G));

        SendMessage(GetDlgItem( hwnd, ID_ED_B), EM_SETLIMITTEXT, 3, 0);
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_B), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)0, (UINT)255));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_B), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_B));

        SendMessage(GetDlgItem( hwnd, ID_RB_A), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);

    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            char buff[4];

            GetWindowText(GetDlgItem( hwnd, id),buff,4);
            int num = atoi(buff);
            if(num > 255)
            {
                num=255;
            }
            else if(num < 0)
            {
                num =0;
            }
            cout << "num: " << num << endl;
            switch(id)
            {
            case ID_ED_R:
                SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_R), TBM_SETPOS, (WPARAM)TRUE, atoi(buff));
                rr=num-tr;
                tr=num;
                break;
            case ID_ED_G:
                SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_G), TBM_SETPOS, (WPARAM)TRUE, atoi(buff));
                gg=num-tg;
                tg=num;
                break;
            case ID_ED_B:
                SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_B), TBM_SETPOS, (WPARAM)TRUE, atoi(buff));
                bb=num-tb;
                tb=num;
                break;
            case ID_RB_A:
                SetWindowText(GetDlgItem( hwnd, ID_ED_R),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_G),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_B),"0");
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                break;
            case ID_RB_S:
                SetWindowText(GetDlgItem( hwnd, ID_ED_R),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_G),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_B),"0");
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                break;
            case ID_RB_M:
                SetWindowText(GetDlgItem( hwnd, ID_ED_R),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_G),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_B),"0");
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                break;
            case ID_RB_D:
                SetWindowText(GetDlgItem( hwnd, ID_ED_R),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_G),"0");
                SetWindowText(GetDlgItem( hwnd, ID_ED_B),"0");
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                break;

            }
            if(IsDlgButtonChecked(hwnd, ID_RB_A))
                images[que[selected_id]]->changeValuesAdd(rr,gg,bb);
            else if(IsDlgButtonChecked(hwnd, ID_RB_S))
                images[que[selected_id]]->changeValuesSub(rr,gg,bb);
            else if(IsDlgButtonChecked(hwnd, ID_RB_M))
                images[que[selected_id]]->changeValuesMul(double(rr)/255,double(gg)/255,double(bb)/255);
            else if(IsDlgButtonChecked(hwnd, ID_RB_D))
                images[que[selected_id]]->changeValuesDiv(double(rr)/255,double(gg)/255,double(bb)/255);
            rr=0;
            gg=0;
            bb=0;
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
        case WM_HSCROLL:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            dwPos = SendMessage(GetDlgItem( hwnd, id), TBM_GETPOS, 0, 0);
            string str = to_string(dwPos);
            switch(id)
            {
            case ID_TRACKBAR_R:
                SetWindowText(GetDlgItem( hwnd, ID_ED_R),str.c_str());
                break;
            case ID_TRACKBAR_G:
                SetWindowText(GetDlgItem( hwnd, ID_ED_G),str.c_str());
                break;
            case ID_TRACKBAR_B:
                SetWindowText(GetDlgItem( hwnd, ID_ED_B),str.c_str());
                break;
            }
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

int bright, tbright;

LRESULT CALLBACK DlgBrightProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        bright = 0;
        tbright = 0;

        SetWindowText(GetDlgItem( hwnd, ID_ED_BRIGHT),"0");


        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_BRIGHT), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)0, (UINT)200));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_BRIGHT), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_BRIGHT));

        SendMessage(GetDlgItem( hwnd, ID_ED_BRIGHT), EM_SETLIMITTEXT, 3, 0);

    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            char buff[5];

            GetWindowText(GetDlgItem( hwnd, id),buff,5);
            int num = atoi(buff);
            if(num > 100)
            {
                num=100;
            }
            else if(num < -100)
            {
                num=0;
            }
            switch(id)
            {
                case ID_ED_BRIGHT:
                    SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_BRIGHT), TBM_SETPOS, (WPARAM)TRUE, num+100);
                    bright=num;
                    break;
            }
            images[que[selected_id]]->changeBright(double(bright)/100);
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
        case WM_HSCROLL:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            dwPos = SendMessage(GetDlgItem( hwnd, id), TBM_GETPOS, 0, 0);
            string str = to_string((int)dwPos-100);
            switch(id)
            {
            case ID_TRACKBAR_BRIGHT:
                SetWindowText(GetDlgItem( hwnd, ID_ED_BRIGHT),str.c_str());
                break;
            }
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK DlgFilterProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
        case WM_INITDIALOG:
        {
            bright = 0;
            tbright = 0;
            SetWindowText(GetDlgItem( hwnd, ID_L_FILTER_NAME),filterName.c_str());
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
        case WM_COMMAND:
        {
            switch( LOWORD(wParam) )
            {
            case IDCANCEL:
            {
                EndDialog(hwnd, -1);
            }
            break;
            case IDOK:
            {
                EndDialog(hwnd, 1);
            }
            break;
            }
        }
        break;
        default:
            return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK DlgFilterSobelProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        SendMessage(GetDlgItem( hwnd, ID_RB_POZ), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            switch(id)
            {
                case ID_RB_POZ:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterSobel(0);
                    break;
                case ID_RB_PION:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterSobel(1);
                    break;
            }
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK DlgColorDesturyzationProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        SendMessage(GetDlgItem( hwnd, ID_RB_NORM), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            switch(id)
            {
                case ID_RB_NORM:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->colorDesturyzation(0);
                    break;
                case ID_RB_YUV:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->colorDesturyzation(1);
                    break;
            }
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}


LRESULT CALLBACK DlgFilterUpSharpenProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        SendMessage(GetDlgItem( hwnd, ID_RB_LVL0), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            switch(id)
            {
                case ID_RB_LVL0:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterUpSharpen(0);
                    break;
                case ID_RB_LVL1:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterUpSharpen(1);
                    break;
                case ID_RB_LVL2:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterUpSharpen(2);
                    break;
                case ID_RB_LVL3:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->filterUpSharpen(3);
                    break;
            }
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

int siz, expected;
int tsiz, texpected;
bool start = false;
LRESULT CALLBACK DlgFilterGaussProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        start = true;
        siz = 1;
        expected = 16;
        tsiz = 1;
        texpected = 16;

        SetWindowText(GetDlgItem( hwnd, ID_ED_S),"1");

        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_SIZE), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)1, (UINT)10));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_SIZE), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_S));

        SendMessage(GetDlgItem( hwnd, ID_ED_S), EM_SETLIMITTEXT, 2, 0);

        SetWindowText(GetDlgItem( hwnd, ID_ED_U),"8");

        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_U), TBM_SETRANGE, (WPARAM) TRUE, (LPARAM) MAKELONG((UINT)1, (UINT)64));
        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_U), TBM_SETBUDDY, (WPARAM)FALSE, (LPARAM)GetDlgItem( hwnd, ID_ED_U));

        SendMessage(GetDlgItem( hwnd, ID_ED_U), EM_SETLIMITTEXT, 3, 0);

        start= false;

    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            if(!start){
                int id = GetDlgCtrlID((HWND)lParam);
                char buff[5];

                GetWindowText(GetDlgItem( hwnd, id),buff,5);

                int num = atoi(buff);

                switch(id)
                {
                    case ID_ED_S:
                        if(num > 10) num=10;
                        else if(num < 1) num=1;
                        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_SIZE), TBM_SETPOS, (WPARAM)TRUE, num);
                        siz=num;
                        break;
                    case ID_ED_U:
                        if(num > 64) num=64;
                        else if(num < 1) num=1;
                        SendMessage(GetDlgItem( hwnd, ID_TRACKBAR_U), TBM_SETPOS, (WPARAM)TRUE, num);
                        expected=num;
                        break;
                }

                if(texpected!=expected||tsiz!=siz){
                    texpected=expected;
                    tsiz=siz;
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    cout << siz << " omg " << expected << endl;
                    images[que[selected_id]]->filterGauss(siz*2+1, expected);
                    InvalidateRect(hCanvas,NULL,FALSE);
                }
            }
        }
        break;
        case WM_HSCROLL:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            dwPos = SendMessage(GetDlgItem( hwnd, id), TBM_GETPOS, 0, 0);
            string str = to_string((int)dwPos);
            switch(id)
            {
                case ID_TRACKBAR_SIZE:
                    SetWindowText(GetDlgItem( hwnd, ID_ED_S),str.c_str());
                    break;
                case ID_TRACKBAR_U:
                    SetWindowText(GetDlgItem( hwnd, ID_ED_U),str.c_str());
                    break;
            }
        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

string histograms[] = {
    "Wartosc", "Czerwony", "Zielony", "Niebieski"
};
string binaryzation[] = {
    "Recznie", "% Selekcja czarnego", "Iteratywna srednia", "Selekcja entropii", "Blad minimalny", "Rozmyty blad minimalny"
};


HDC hdcBuforD;
HBITMAP hbBufD, hbOldBufD;
int histogramid=0;
const double proportionx = 512.0/256;

int prog_min_x=0;

int prog_max_x=511;

int binaryzation_method=0;

HPEN redp, bluep;
LRESULT CALLBACK DlgHistogramProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {
        hdcBuforD = CreateCompatibleDC(GetWindowDC(hwnd));
        hbBufD = CreateCompatibleBitmap( canvasHdc, 514, 224 );
        hbOldBufD =( HBITMAP ) SelectObject( hdcBuforD, hbBufD );

        redp = CreatePen( PS_SOLID, 1, 0x0000FF );
        bluep = CreatePen( PS_SOLID, 1, 0xFF0000 );

        binaryzation_method=0;


        SendMessage(GetDlgItem( hwnd, ID_RB_BEZ), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);

        HWND hwndList = GetDlgItem( hwnd, ID_HIST_LIST);
        for (int i = 0; i < 4; i++)
        {
            SendMessage(GetDlgItem( hwnd, ID_HIST_LIST), CB_ADDSTRING, 0, (LPARAM) histograms[i].c_str());
        }

        SendMessage(GetDlgItem( hwnd, ID_HIST_LIST), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

        HWND hwndBinList = GetDlgItem( hwnd, ID_BIN_LIST);
        for (int i = 0; i < 6; i++)
        {
            SendMessage(GetDlgItem( hwnd, ID_BIN_LIST), CB_ADDSTRING, 0, (LPARAM) binaryzation[i].c_str());
        }

        SendMessage(GetDlgItem( hwnd, ID_BIN_LIST), CB_SETCURSEL, (WPARAM)0, (LPARAM)0);



        images[que[selected_id]]->countHistograms();

        int histogramid=0;
        SetWindowText(GetDlgItem( hwnd, ID_LB_MAX), to_string((int) images[que[selected_id]]->MaxH[0]).c_str()) ;
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        SelectObject( hdcBuforD, hbBufD );
        Rectangle(hdcBuforD, 0, 0, 514, 156);


        if(histogramid==0)
            for(int i=0;i<256;i++){
                Rectangle(hdcBuforD,i*proportionx+1, 154, (i+1)*proportionx+1,154-(154 * (double)(images[que[selected_id]]->HistogramW[i])/images[que[selected_id]]->MaxH[0]) );
            }
        else if(histogramid==1)
            for(int i=0;i<256;i++){
                Rectangle(hdcBuforD,i*proportionx+1, 154, (i+1)*proportionx+1,154-(154 * (double)(images[que[selected_id]]->HistogramR[i])/images[que[selected_id]]->MaxH[1]) );
            }
        else if(histogramid==2)
            for(int i=0;i<256;i++){
                Rectangle(hdcBuforD,i*proportionx+1, 154, (i+1)*proportionx+1,154-(154 * (double)(images[que[selected_id]]->HistogramG[i])/images[que[selected_id]]->MaxH[2]) );
            }
        else if(histogramid==3)
            for(int i=0;i<256;i++){
                Rectangle(hdcBuforD,i*proportionx+1, 154, (i+1)*proportionx+1,154-(154 * (double)(images[que[selected_id]]->HistogramB[i])/images[que[selected_id]]->MaxH[3]) );
            }



        HPEN white = (HPEN) SelectObject(hdcBuforD, redp);
        Rectangle(hdcBuforD, prog_min_x, 154, prog_min_x+2, 0);
        SelectObject(hdcBuforD, bluep);
        Rectangle(hdcBuforD, prog_max_x, 154, prog_max_x+2, 0);

        SelectObject(hdcBuforD, white);

        BitBlt(hdc, 10, 40, 514, 155, hdcBuforD, 0, 0, SRCCOPY);

        EndPaint(hwnd, &ps);
    }
        break;

    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }

        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);

                switch(id)
                {
                    case ID_HIST_LIST:
                        switch (HIWORD(wParam))
                        {
                            case LBN_SELCHANGE:
                                histogramid = (int)SendMessage(GetDlgItem( hwnd, ID_HIST_LIST), CB_GETCURSEL, 0, 0);
                                SetWindowText(GetDlgItem( hwnd, ID_LB_MAX), to_string((int) images[que[selected_id]]->MaxH[histogramid]).c_str()) ;
                                InvalidateRect(hwnd,NULL,TRUE);

                            break;
                        }
                        break;
                    case ID_BIN_LIST:
                    {
                        switch (HIWORD(wParam))
                        {
                            case LBN_SELCHANGE:
                                binaryzation_method = (int)SendMessage(GetDlgItem( hwnd, ID_BIN_LIST), CB_GETCURSEL, 0, 0);
                                if(binaryzation_method==2){
                                    images[que[selected_id]]->returnCopy();
                                    images[que[selected_id]]->makeCopy();
                                    prog_max_x=prog_min_x=2*images[que[selected_id]]->iterationbin();
                                    InvalidateRect(hCanvas,NULL,FALSE);
                                } else if(binaryzation_method==3) {
                                    images[que[selected_id]]->returnCopy();
                                    images[que[selected_id]]->makeCopy();
                                    prog_max_x=prog_min_x=2*images[que[selected_id]]->entropybin();
                                    InvalidateRect(hCanvas,NULL,FALSE);
                                } else if(binaryzation_method==4) {
                                    images[que[selected_id]]->returnCopy();
                                    images[que[selected_id]]->makeCopy();
                                    prog_max_x=prog_min_x=2*images[que[selected_id]]->minimumerrorbin();
                                    InvalidateRect(hCanvas,NULL,FALSE);
                                } else if(binaryzation_method==5) {
                                    images[que[selected_id]]->returnCopy();
                                    images[que[selected_id]]->makeCopy();
                                    prog_max_x=prog_min_x=2*images[que[selected_id]]->fuzzyminimumerrorbin();
                                    InvalidateRect(hCanvas,NULL,FALSE);
                                }

                                InvalidateRect(hwnd,NULL,TRUE);
                            break;
                        }
                        InvalidateRect(hwnd,NULL,FALSE);
                    }
                        break;
                    case ID_RB_BEZ:
                        images[que[selected_id]]->returnCopy();
                        images[que[selected_id]]->makeCopy();
                        images[que[selected_id]]->countHistograms();
                        InvalidateRect(hCanvas,NULL,FALSE);
                        break;
                    case ID_RB_ROZC:
                        images[que[selected_id]]->returnCopy();
                        images[que[selected_id]]->makeCopy();
                        images[que[selected_id]]->countHistograms();
                        images[que[selected_id]]->resizeHistogram();
                        images[que[selected_id]]->countHistograms();
                        InvalidateRect(hCanvas,NULL,FALSE);
                        break;
                    case ID_RB_WYR:
                        images[que[selected_id]]->returnCopy();
                        images[que[selected_id]]->makeCopy();
                        images[que[selected_id]]->countHistograms();
                        images[que[selected_id]]->equalHistogram();
                        images[que[selected_id]]->countHistograms();
                        InvalidateRect(hCanvas,NULL,FALSE);
                        break;
                }
                InvalidateRect(hwnd,NULL,FALSE);


            }
        break;




        case WM_RBUTTONDOWN:
        {
            if(binaryzation_method==0){
                int y = GET_Y_LPARAM(lParam);
                int x = GET_X_LPARAM(lParam);
                if(x<10) x = 10;
                else if(x>521) x = 521;
                if(y > 40 && y<155)
                    prog_max_x = x-10;
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();

                cout << prog_max_x << endl;

                images[que[selected_id]]->binaryzation(prog_min_x/2,prog_max_x/2);

                InvalidateRect(hwnd, 0, FALSE);
                InvalidateRect(hCanvas, 0, FALSE);
            }

        }
            break;

        case WM_LBUTTONDOWN:
        {
            if(binaryzation_method<2){
                int y = GET_Y_LPARAM(lParam);
                int x = GET_X_LPARAM(lParam);
                if(x<10) x = 10;
                else if(x>520) x = 520;
                if(y > 40 && y<155){
                    prog_min_x = x-10;

                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();

                    if(binaryzation_method==1){
                        prog_max_x=2*images[que[selected_id]]->percentbin(double(prog_min_x)/510);
                    }
                    else{
                        images[que[selected_id]]->binaryzation(prog_min_x/2,prog_max_x/2);
                    }

                    InvalidateRect(hwnd, 0, FALSE);
                    InvalidateRect(hCanvas, 0, FALSE);
                }
            }

        }
            break;


    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}

HDC morfologyHDC, hdcBuforM;
HBITMAP hbBufM, hbOldBufM;
HBRUSH hbWhite, hbBlack, hbGray;

bool thick = true;

LRESULT CALLBACK DlgMorfologyHitProc( HWND hwnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_INITDIALOG:
    {

        hbBlack = CreateSolidBrush(0x000444444);
        hbWhite = CreateSolidBrush(0x00FFFFFF );
        hbGray = CreateSolidBrush(0x00999999 );

        morfologyHDC = GetWindowDC(hwnd);

        hdcBuforM = CreateCompatibleDC(morfologyHDC);
        hbBufM = CreateCompatibleBitmap( morfologyHDC, 250, 250 );
        hbOldBufM =( HBITMAP ) SelectObject( hdcBuforM, hbBufM );

        SendMessage(GetDlgItem( hwnd, ID_RB_THICK), BM_SETCHECK, (WPARAM) BST_CHECKED, 0);
        thick=true;
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        SelectObject( hdcBuforM, hbBufM );
        Rectangle(hdcBuforM, 0, 0, 250, 250);
        for(int i=0;i<strukt.size();i++){
            if(get<2>(strukt[i])==0)  SelectObject(hdcBuforM, hbBlack);
            else if(get<2>(strukt[i])==1)  SelectObject(hdcBuforM, hbWhite);
            else SelectObject(hdcBuforM, hbGray);
            Rectangle(hdcBuforM, 100+(get<1>(strukt[i])*50), 100+(get<0>(strukt[i])*50), 100+(get<1>(strukt[i])*50)+50, 100+(get<0>(strukt[i])*50)+50);
        }
        BitBlt(hdc, 21, 30, 250, 250, hdcBuforM, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps);
    }
    break;
    case WM_COMMAND:
    {
        switch( LOWORD(wParam) )
        {
        case IDCANCEL:
        {
            EndDialog(hwnd, -1);
        }
        break;
        case IDOK:
        {
            EndDialog(hwnd, 1);
        }
        break;
        }
        case EN_CHANGE:
        {
            int id = GetDlgCtrlID((HWND)lParam);
            switch(id)
            {
                case ID_RB_THICK:
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfologyHit(5,true,strukt);
                    thick=true;
                    break;
                case ID_RB_THIN:
                    cout << "lol" << endl;
                    images[que[selected_id]]->returnCopy();
                    images[que[selected_id]]->makeCopy();
                    images[que[selected_id]]->morfologyHit(5,false,strukt);
                    thick=false;
                    break;
            }
            InvalidateRect(hCanvas,NULL,FALSE);
        }
        break;
        case WM_LBUTTONDOWN:
        {
            int mouse_x = GET_X_LPARAM(lParam);
            int mouse_y = GET_Y_LPARAM(lParam);
            int id=0;
            if(mouse_x>21&&mouse_x<271&&mouse_y>30&&mouse_y<280){
                int w = (mouse_x-21)/50;
                int h = (mouse_y-30)/50;
                id=h*5+w;
                get<2>(strukt[id]) = (get<2>(strukt[id])+1)%3;
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                images[que[selected_id]]->morfologyHit(5,thick,strukt);
                InvalidateRect(hCanvas,NULL,FALSE);
                InvalidateRect(hwnd,NULL,FALSE);
            }

        }
        break;
        case WM_RBUTTONDOWN:
        {
            int mouse_x = GET_X_LPARAM(lParam);
            int mouse_y = GET_Y_LPARAM(lParam);
            int id=0;
            if(mouse_x>21&&mouse_x<271&&mouse_y>30&&mouse_y<280){
                int w = (mouse_x-21)/50;
                int h = (mouse_y-30)/50;
                id=h*5+w;
                get<2>(strukt[id]) = get<2>(strukt[id])-1;
                if(get<2>(strukt[id])==-1) get<2>(strukt[id]) =2;
                images[que[selected_id]]->returnCopy();
                images[que[selected_id]]->makeCopy();
                images[que[selected_id]]->morfologyHit(5,thick,strukt);
                InvalidateRect(hCanvas,NULL,FALSE);
                InvalidateRect(hwnd,NULL,FALSE);
            }

        }
        break;
    }
    break;
    default:
        return FALSE;
    }
    return TRUE;
}
