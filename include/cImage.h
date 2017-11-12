#ifndef CIMAGE_H
#define CIMAGE_H

#include <windows.h>
#include <iostream>
#include <thread>
#include <cmath>
#include <vector>

using namespace std;

class cImage
{
    public:
        cImage();
        virtual ~cImage();
        COLORREF *image, *temp;
        HBITMAP hBitmap;
        BITMAP bitmap;
        DWORD operation;
        int pos_x;
        int pos_y;
        int width;
        int height;

        int HistogramW[256];
        int HistogramR[256];
        int HistogramG[256];
        int HistogramB[256];

        double MaxH[4];
        double MaxC[4];
        double MinH[4];

        void setHBitmap(int x, int y);
        void setBitmap();
        void convertToCMYK();
        void changeValuesAdd(int r, int g, int b);
        void changeValuesSub(int r, int g, int b);
        void changeValuesMul(double r, double g, double b);
        void changeValuesDiv(double r, double g, double b);
        void changeBright(double bright);

        void filter(int s, int *wages);
        void filterMed(int s);
        void filterMedian();
        void filterGauss(int s, int u);
        void filterSobel(int s);
        void filterUpSharpen(int s);
        void filterMask();
        void colorDesturyzation(int s);

        void countHistograms();
        void resizeHistogram();
        void equalHistogram();

        void binaryzation(int p_min, int p_max);
        int iterationbin();
        int entropybin();
        int percentbin(double percent);
        int fuzzyminimumerrorbin();
        int minimumerrorbin();

        void morfology(int s, bool add);
        void morfologyHit(int s, bool thick, vector<tuple<int,int,int>> &strukt);
        void morfologyOpen(int s, bool open);
        void morfologyProc(COLORREF *temp, int ss, bool thick, vector <tuple<int, int, int>> strukt, int from, int to );

        void foo(int,int);

        void makeCopy();
        void returnCopy();
        bool ifSelected(int x, int y);
        void Move(int x, int y);

    protected:

    private:
};

#endif // CIMAGE_H
