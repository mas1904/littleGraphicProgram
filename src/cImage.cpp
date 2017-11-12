#include "../include/cImage.h"

cImage::cImage()
{
    operation = SRCCOPY;

}

cImage::~cImage()
{
    //dtor
}

void cImage::setBitmap(){
    GetObject( hBitmap, sizeof( bitmap), &bitmap);
    height = bitmap.bmHeight;
    width = bitmap.bmWidth;
}

void cImage::setHBitmap(int x, int y){
    hBitmap = CreateBitmap(x, y, 1, 32, ((void*)(image)) );
    GetObject( hBitmap, sizeof( bitmap), &bitmap);
    height = bitmap.bmHeight;
    width = bitmap.bmWidth;
}

void cImage::convertToCMYK(){
    for(int i=0; i<4; i+=4){
    }
}

bool cImage::ifSelected(int x, int y){
    cout << x << " : " << y << " | " << pos_x << " : " << pos_y << endl;
    if(x>pos_x && x<width+pos_x && y>pos_y && y < height+pos_y)
        return true;
    return false;
}
void cImage::Move(int x, int y){

    pos_x = x;
    pos_y = y;

}

void cImage::makeCopy(){
    cout << width << " " << height << " <<< " << endl;
    temp = (COLORREF*) calloc(width*height, sizeof(COLORREF));
    memcpy(temp, image, width*height*sizeof(COLORREF));
}

void cImage::returnCopy(){
    cout << temp[1] << " " << image[1] << endl;
    memcpy(image, temp, width*height*sizeof(COLORREF));
    free(temp);
    temp=NULL;
    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
}

void cImage::changeValuesAdd(int r, int g, int b){
    int bb, gg, rr;
    if(r!=0||g!=0||b!=0){
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])+b);
                gg = (GetGValue(image[i*width+j])+g);
                rr = (GetBValue(image[i*width+j])+r);
                if(bb>255) bb=255;
                if(gg>255) gg=255;
                if(rr>255) rr=255;
                image[i*width+j]=RGB( bb, gg, rr);

            }
        DeleteObject(hBitmap);
        hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    }
}
void cImage::changeValuesSub(int r, int g, int b){
    int bb, gg, rr;
    if(r!=0||g!=0||b!=0){
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])-b);
                gg = (GetGValue(image[i*width+j])-g);
                rr = (GetBValue(image[i*width+j])-r);
                if(bb<0) bb=0;
                if(gg<0) gg=0;
                if(rr<0) rr=0;
                image[i*width+j]=RGB( bb, gg, rr);

            }
        DeleteObject(hBitmap);
        hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    }
}
void cImage::changeValuesMul(double r, double g, double b){
    int bb, gg, rr;
    if(r!=0||g!=0||b!=0){
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])*(1.0+b));
                gg = (GetGValue(image[i*width+j])*(1.0+g));
                rr = (GetBValue(image[i*width+j])*(1.0+r));
                if(bb>255) bb=255;
                if(gg>255) gg=255;
                if(rr>255) rr=255;
                image[i*width+j]=RGB( bb, gg, rr);

            }
        DeleteObject(hBitmap);
        hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    }
}
void cImage::changeValuesDiv(double r, double g, double b){
    int bb, gg, rr;
    if(r!=0||g!=0||b!=0){
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])/(1.0+b));
                gg = (GetGValue(image[i*width+j])/(1.0+g));
                rr = (GetBValue(image[i*width+j])/(1.0+r));
                if(bb>255) bb=255;
                if(gg>255) gg=255;
                if(rr>255) rr=255;
                image[i*width+j]=RGB( bb, gg, rr);

            }
        DeleteObject(hBitmap);
        hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    }
}

void cImage::changeBright(double bright){
    if(bright!=0){
        int bb, gg, rr;
        returnCopy();
        makeCopy();
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])*(1.0+bright));
                gg = (GetGValue(image[i*width+j])*(1.0+bright));
                rr = (GetBValue(image[i*width+j])*(1.0+bright));
                if(bb>255) bb=255;
                if(gg>255) gg=255;
                if(rr>255) rr=255;
                image[i*width+j]=RGB( bb, gg, rr);

            }
        DeleteObject(hBitmap);
        hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    }
}

void cImage::filterMed(int s){
    int wages[s*s];
    for(int i=0;i<s*s;i++)
        wages[i]=1;
    filter(s, wages);
}

void cImage::filterSobel(int s){
    if(s==0){
        int wages[9]={1,2,1,0,0,0,-1,-2,-1};
        filter(3, wages);
    } else {
        int wages[9]={1,0,-1,2,0,-2,1,0,-1};
        filter(3, wages);
    }
}

void cImage::colorDesturyzation(int s){
    double wages[3]={1,1,1};
    int bb, gg, rr;
    if(s==0){
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j]));
                gg = (GetGValue(image[i*width+j]));
                rr = (GetBValue(image[i*width+j]));
                image[i*width+j]=RGB( (bb+gg+rr)/3, (bb+gg+rr)/3, (bb+gg+rr)/3);
            }
    }

    if(s==1){
        wages[0] = 0.299;
        wages[1] = 0.587;
        wages[2] = 0.114;

        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j])*wages[2] );
                gg = (GetGValue(image[i*width+j])*wages[1]) ;
                rr = (GetBValue(image[i*width+j])*wages[0] );
                image[i*width+j]=RGB( bb+gg+rr, bb+gg+rr, bb+gg+rr);
            }
    }


    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
}

void cImage::filterUpSharpen(int s){
    switch(s){
        case 0:{
            int wages[9]={-1,-1,-1,-1,9,-1,-1,-1,-1};
            filter(3, wages);
            break;
        }
        case 1:{
            int wages[9]={0,-1,0,-1,5,-1,0,-1,0};
            filter(3, wages);
            break;
        }
        case 2:{
            int wages[9]={1,-2,1,-2,5,-2,1,-2,1};
            filter(3, wages);
            break;
        }
        case 3:{
            int wages[9]={0,-1,0,-1,20,-1,0,-1,0};
            filter(3, wages);
            break;
        }
    }
}

void cImage::filterGauss(int s, int u){
    double wages[s*s];
    int s2 = s/2;

    for(int i=0;i<s;i++){
        for(int j=0;j<s;j++){
            wages[i*s+j]=1.0/(2*3.14*s*s) * exp( -1.0/2*( (pow(i-s2,2)/(s*s)) + (pow(j-s2,2)/(s*s)) ) );
        }
    }
    int wag[s*s];
    for(int i=0;i<s;i++){
        for(int j=0;j<s;j++){
            wag[i*s+j]=wages[i*s+j]/wages[s2*s+s2]*u;
            //cout << wag[i*s+j] << "   " << wages[i*s+j] << " ' ";
        }
       // cout << endl;
    }

    filter(s, wag);

}

void cImage::filterMask(){

}

void cImage::filter(int s, int *wages){
    int rr, gg, bb;
    int ss = s/2;
    int s2 = 0;
    for(int i=0;i<s*s;i++)
        s2+=wages[i];
    if(s2==0)s2=1;

    COLORREF *temp = (COLORREF*) calloc((2*ss+width)*(2*ss+height),sizeof(COLORREF));

    memcpy(ss*width+temp+ss, image, width*height*sizeof(COLORREF));

    for(int i=0;i<ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*width+j]=image[i*width+(j-1)];
        }
    for(int i=height;i<height+ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*width+j]=image[(height-1)*width+(j-1)];
        }
    for(int i=0;i<height+ss;i++)
        for(int j=0;j<ss;j++){
                temp[i*(2*ss+width)+j]=temp[i*(2*ss+width)+j+1];
        }
    for(int i=0;i<height+ss;i++)
        for(int j=ss+width;j<2*ss+width;j++){
                temp[i*(2*ss+width)+j]=temp[i*(2*ss+width)+j-1];
        }
    for(int i=ss;i<height+ss;i++)
        for(int j=ss;j<width+ss;j++){
            bb = 0; gg = 0; rr = 0;
            for(int k=-ss;k<=ss;k++)
                for(int m=-ss;m<=ss;m++){
                    bb += (GetRValue(temp[(i+k)*width+j+m]))*wages[(k+ss)*s+m+ss];
                    gg += (GetGValue(temp[(i+k)*width+j+m]))*wages[(k+ss)*s+m+ss];
                    rr += (GetBValue(temp[(i+k)*width+j+m]))*wages[(k+ss)*s+m+ss];
                    //cout << "TRW " << k << " l " << (int)GetRValue(temp[(i+k)*width+j+m]) << " " << (int)GetGValue(temp[(i+k)*width+j+m]) << " " << (int)GetBValue(temp[(i+k)*width+j+m]) << endl;
                }
            //cout << "S " << s2 << " s " << bb/s2 << " " << gg/s2 << " " << rr/s2 << "ss[" << (i-ss)*width+(j-ss) << endl;
            bb/=s2;
            gg/=s2;
            rr/=s2;
            if(bb<0)bb=0;
            if(gg<0)gg=0;
            if(rr<0)rr=0;
            if(bb>255)bb=255;
            if(gg>255)gg=255;
            if(rr>255)rr=255;
            image[(i-ss)*width+(j-ss)]=RGB( bb, gg, rr);
        }

    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    //memcpy(image, ss*width+temp+ss, width*height*sizeof(COLORREF));

    free(temp);
    temp=NULL;
}

int partitionB(int c[10], int a, int b){
    int e,tmp;
    e=c[a];        //elemennt dzielacy
    while (a<b)
    {
        while ((a<b) && ((c[b])>=e)) b--;
        while ((a<b) && ((c[a])<e)) a++;
        if (a<b)
        {
            tmp=(c[a]);
            (c[a])=(c[b]);
            (c[b])=tmp;
        }
    }
    return a;
}

void getColor(int tb[10]){
    int q=0;
    int r=8;
    int w=4;
    int n=w;
    int bb;
    while (q!=r)
    {
        bb=partitionB(tb,q,r);
        bb=bb-q+1;
        if (bb>=w) r=q+bb-1;
        else if (bb<w)
        {
            w-=bb;
            q+=bb;
        }
    }
    tb[9]=tb[bb];
}

void cImage::filterMedian(){
    int rr, gg, bb;
    int s = 3;
    int ss = s/2;
    int s2 = s*s;

    COLORREF *temp = (COLORREF*) calloc((2*ss+width)*(2*ss+height),sizeof(COLORREF));

    memcpy(ss*width+temp+ss, image, width*height*sizeof(COLORREF));

    for(int i=0;i<ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*width+j]=image[i*width+(j-1)];
        }
    for(int i=height;i<height+ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*width+j]=image[(i-1)*width+(j-1)];
        }
    for(int i=0;i<height+ss;i++)
        for(int j=0;j<ss;j++){
                temp[i*(2*ss+width)+j]=temp[i*(2*ss+width)+j+1];
        }
    for(int i=0;i<height+ss;i++)
        for(int j=ss+width;j<2*ss+width;j++){
                temp[i*(2*ss+width)+j]=temp[i*(2*ss+width)+j-1];
        }
    int tr[10],tg[10],tb[10],id=0;


    for(int i=ss;i<height+ss;i++)
        for(int j=ss;j<width+ss;j++){
            for(int k=-ss;k<=ss;k++)
                for(int m=-ss;m<=ss;m++){
                    tb[id] = (GetRValue(temp[(i+k)*width+j+m]));
                    tg[id] = (GetGValue(temp[(i+k)*width+j+m]));
                    tr[id] = (GetBValue(temp[(i+k)*width+j+m]));
                    ++id;
                }
            id=0;
            getColor( tb );
            getColor( tg );
            getColor( tr );
            image[(i-ss)*width+(j-ss)]=RGB( tb[9], tg[9], tr[9]);
        }

    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
    //memcpy(image, ss*width+temp+ss, width*height*sizeof(COLORREF));

    free(temp);
    temp=NULL;
}

void cImage::binaryzation(int p_min, int p_max){

    int bb, gg, rr;
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            bb = (GetRValue(image[i*width+j]));
            gg = (GetGValue(image[i*width+j]));
            rr = (GetBValue(image[i*width+j]));

            int sr = double(bb+gg+rr)/3;

            if(p_min <= p_max){
                if(sr<=p_min)
                    bb=gg=rr=0;
                else if(sr>=p_max)
                    bb=gg=rr=255;
            } else {
                if(sr<=p_max)
                    bb=gg=rr=255;
                else if(sr>=p_min)
                    bb=gg=rr=0;
            }


            image[i*width+j]=RGB( bb, gg, rr);
        }

    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );

}

void cImage::countHistograms(){
    int rr, gg, bb;
    for(int i=0;i<256;i++){
        HistogramW[i]=0;
        HistogramR[i]=0;
        HistogramG[i]=0;
        HistogramB[i]=0;
    }
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            bb = (GetRValue(image[i*width+j]));
            gg = (GetGValue(image[i*width+j]));
            rr = (GetBValue(image[i*width+j]));
            HistogramW[int(((double)(rr+gg+bb))/3)]++;
            HistogramR[rr]++;
            HistogramG[gg]++;
            HistogramB[bb]++;
        }

    MinH[0]=HistogramW[0];
    MaxH[0]=HistogramW[0];

    MinH[1]=HistogramR[0];
    MaxH[1]=HistogramR[0];

    MinH[2]=HistogramG[0];
    MaxH[2]=HistogramG[0];

    MinH[3]=HistogramB[0];
    MaxH[3]=HistogramB[0];

    for(int i=0;i<4;i++){
        MaxC[i]=255;
        MinH[i]=0;
    }

    bool not0[4] = {true, true, true, true};

    if(HistogramW[0]==0 && not0[0] ) MinH[0]++;
    else not0[0] = false;

    if(HistogramR[0]==0 && not0[0] ) MinH[1]++;
    else not0[1] = false;

    if(HistogramG[0]==0 && not0[0] ) MinH[2]++;
    else not0[2] = false;

    if(HistogramB[0]==0 && not0[0] ) MinH[3]++;
    else not0[3] = false;

    for(int i=1;i<256;i++){
        if(HistogramW[i]>MaxH[0]) MaxH[0] = HistogramW[i];
        if(HistogramR[i]>MaxH[1]) MaxH[1] = HistogramR[i];
        if(HistogramG[i]>MaxH[2]) MaxH[2] = HistogramG[i];
        if(HistogramB[i]>MaxH[3]) MaxH[3] = HistogramB[i];

        if(HistogramW[i]==0 && not0[0] ) MinH[0]++;
        else not0[0] = false;

        if(HistogramR[i]==0 && not0[0] ) MinH[1]++;
        else not0[1] = false;

        if(HistogramG[i]==0 && not0[0] ) MinH[2]++;
        else not0[2] = false;

        if(HistogramB[i]==0 && not0[0] ) MinH[3]++;
        else not0[3] = false;
    }

    not0[0] = true; not0[1] = true; not0[2] = true; not0[3] = true;

    for(int i=255;i>=0;i--){
        if(HistogramW[i]==0 && not0[0] ) MaxC[0]--;
        else not0[0] = false;

        if(HistogramR[i]==0 && not0[0] ) MaxC[1]--;
        else not0[1] = false;

        if(HistogramG[i]==0 && not0[0] ) MaxC[2]--;
        else not0[2] = false;

        if(HistogramB[i]==0 && not0[0] ) MaxC[3]--;
        else not0[3] = false;
    }

    cout << MaxC[1] << " sds" << MaxC[2] << "xx" << MaxC[3] << endl;

    cout << MinH[1] << " sds" << MinH[2] << "xx" << MinH[3] << endl;
}

void cImage::resizeHistogram(){
    int rr, gg, bb, lut;
    int LUTr[256];
    int LUTg[256];
    int LUTb[256];
    for(int i=0;i<256;i++){
        LUTr[i] = (255.0/(MaxC[1] - MinH[1])) * (i - MinH[1]);
        LUTg[i] = (255.0/(MaxC[2] - MinH[2])) * (i - MinH[2]);
        LUTb[i] = (255.0/(MaxC[3] - MinH[3])) * (i - MinH[3]);
    }
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            bb = LUTb[GetRValue(image[i*width+j])];
            gg = LUTg[GetGValue(image[i*width+j])];
            rr = LUTr[GetBValue(image[i*width+j])];

            image[i*width+j]=RGB( bb, gg, rr);
        }

    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
}

void cImage::equalHistogram(){
    int rr, gg, bb, lut;

    double sumR=0, sumG=0, sumB=0;
    int pixels = width*height;

    double Dr[256], Dg[256], Db[256];


    double first_not_0[3];

    bool not0[3] = {false, false, false};

    for(int i=0;i<256;i++){
        sumR += (double(HistogramR[i])/pixels);
        sumG += (double(HistogramG[i])/pixels);
        sumB += (double(HistogramB[i])/pixels);
        Dr[i] = sumR;
        Dg[i] = sumG;
        Db[i] = sumB;
        if(Dr[i]!=0 && !not0[0] ){
            first_not_0[0] = Dr[i];
            not0[0] = true;
        }
        if(Dg[i]!=0 && !not0[1] ){
            first_not_0[1] = Dg[i];
            not0[1] = true;
        }
        if(Db[i]!=0 && !not0[2] ){
            first_not_0[2] = Db[i];
            not0[2] = true;
        }
    }

    int LUTr[256];
    int LUTg[256];
    int LUTb[256];
    for(int i=0;i<256;i++){
        LUTr[i] = (Dr[i]- first_not_0[0])/(1.0 - first_not_0[0]) * (255);
        LUTg[i] = (Dg[i]- first_not_0[1])/(1.0 - first_not_0[1]) * (255);
        LUTb[i] = (Db[i]- first_not_0[2])/(1.0 - first_not_0[2]) * (255);
    }
    for(int i=0;i<height;i++)
        for(int j=0;j<width;j++){
            bb = LUTb[GetRValue(image[i*width+j])];
            gg = LUTg[GetGValue(image[i*width+j])];
            rr = LUTr[GetBValue(image[i*width+j])];

            image[i*width+j]=RGB( bb, gg, rr);
        }

    DeleteObject(hBitmap);
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );
}

int cImage::entropybin() {
    double po, pb, Ho=1, Hb=0, Hm=0;
    double p, pco, pcb;

    int tt=0;
    for(int t=0;t<256;t++){

        for(int i=0;i<t;i++)
            pco+=double(HistogramW[i])/(width*height);
        pcb=1-pco;

        for(int i=0;i<t;i++){
            p = double(HistogramW[i])/(width*height);
            po += (p/pco)*log(p/pco);
        }
        for(int i=t;i<256;i++){
            p = double(HistogramW[i])/(width*height);
            pb += (p/pcb)*log(p/pcb);
        }
        Ho=-po;
        Hb=-pb;
        po=pco=0;
        pb=pcb=0;
        if(Hm<Ho+Hb){
            Hm=Ho+Hb;
            tt=t;
        }
    }
    cout << tt << " tt" << endl;

    binaryzation(tt,tt);
    return tt;
}

int cImage::minimumerrorbin(){

}

int cImage::fuzzyminimumerrorbin(){

}

int cImage::percentbin(double percent){
    int i=0;
    double N = width*height*percent;
    int Had=0;
    while(Had < N && i<255)
        Had+=HistogramW[i++];

    cout << i << "kek " << percent << " %" << endl;
    binaryzation(i-1,i-1);
    return i-1;
}

int cImage::iterationbin(){
    int bb, gg, rr;

    int ob_pixels=width*height-4;
    int wl_pixels=4;

    int lut_wl = (GetRValue(image[0])+GetBValue(image[0])+GetGValue(image[0]) )/3;
    lut_wl += (GetRValue(image[width-1])+GetBValue(image[width-1])+GetGValue(image[width-1]) )/3;
    lut_wl += (GetRValue(image[(height-1)*width])+GetBValue(image[(height-1)*width])+GetGValue(image[(height-1)*width]) )/3;
    lut_wl += (GetRValue(image[(height-2)*width+1])+GetBValue(image[(height-2)*width+1])+GetGValue(image[(height-2)*width+1]) )/3;

    lut_wl/=4;

    int lut_ob=0;

    for(int i=i;i<height;i++)
        for(int j=0;j<width;j++)
            lut_ob+=(GetRValue(image[0])+GetBValue(image[0])+GetGValue(image[0]) )/3;

    lut_ob/=width*height;

    int T0 = (lut_ob+lut_wl)/2;
    int T1 = 0;

    cout << "kek" << T0 << endl;

    while(T0!=T1){
        T1 = T0;
        ob_pixels=0;
        wl_pixels=0;
        lut_wl=0;
        lut_ob=0;
        for(int i=0;i<height;i++)
            for(int j=0;j<width;j++){
                bb = (GetRValue(image[i*width+j]));
                gg = (GetGValue(image[i*width+j]));
                rr = (GetBValue(image[i*width+j]));

                int sr = double(bb+gg+rr)/3;
                if(sr<=T1){
                    lut_wl+=sr;
                    wl_pixels++;
                }
                else {
                    lut_ob+=sr;
                    ob_pixels++;
                }
            }
        lut_wl/=wl_pixels;
        lut_ob/=ob_pixels;
        T0 = (lut_wl+lut_ob)/2;
    }
    cout << T0 << " te zero " << endl;
    binaryzation(T0,T0);
    return T0;
}

void cImage::morfologyOpen(int s, bool open){
    if(open){
        morfology(s, false);
        morfology(s, true);
    } else {
        morfology(s, true);
        morfology(s, false);
    }

}

void cImage::morfologyProc(COLORREF *temp, int ss, bool thick, vector <tuple<int, int, int>> strukt, int from, int to ){

    bool hit=true;
    int cb,cg,cr,bb,gg,rr;

    int temp_width = 2*ss+width;

    for(int i=ss+from;i<to+ss;i++)
        for(int j=ss;j<width+ss;j++){
            cb = (GetRValue(temp[(i)*(temp_width)+j]));
            cg = (GetGValue(temp[(i)*(temp_width)+j]));
            cr = (GetBValue(temp[(i)*(temp_width)+j]));

            hit=true;

            for(int v=0;v<strukt.size();v++){
                if(get<2>(strukt[v])!=2){
                    int k = get<0>(strukt[v]);
                    int m = get<1>(strukt[v]);
                    bb = (GetRValue(temp[(i+k)*(temp_width)+j+m]));
                    gg = (GetGValue(temp[(i+k)*(temp_width)+j+m]));
                    rr = (GetBValue(temp[(i+k)*(temp_width)+j+m]));
                    if(get<2>(strukt[v])==0&&bb+gg+rr>0){      ///  third=0 czyli czarne
                        hit=false;
                        break;
                    }
                    else if (get<2>(strukt[v])==1&&bb+gg+rr==0){  /// third=1 czyli biale
                        hit=false;
                        break;
                    }
                }
            }

            if(!thick&&hit)
                cb=cg=cr=255;
            else if(thick&&hit)
                cb=cg=cr=0;

            image[(i-ss)*width+(j-ss)]=RGB( cb, cg, cr);
        }


}

void cImage::morfologyHit(int s, bool thick, vector <tuple<int, int, int>> &strukt){
    int rr, gg, bb;
    int ss = s/2;
    int s2 = s*s;

    COLORREF *temp = (COLORREF*) calloc((2*ss+width)*(2*ss+height),sizeof(COLORREF));

    for(int i=0;i<height;i++){
            memcpy( (2*ss+width) * (i+ss) + temp+ss, width*i+image, width*sizeof(COLORREF));
    }


    for(int i=0;i<ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=height+ss;i<2*ss+height;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=0;i<height+ss;i++)
        for(int j=0;j<ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=0;i<height+ss;i++)
        for(int j=ss+width;j<2*ss+width;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }

    int t_height = height/8;

    thread first(morfologyProc,this,temp,ss,thick,strukt,0,t_height);
    thread second(morfologyProc,this,temp,ss,thick,strukt,t_height,2*t_height);
    thread third(morfologyProc,this,temp,ss,thick,strukt,2*t_height,3*t_height);
    thread fourth(morfologyProc,this,temp,ss,thick,strukt,3*t_height,4*t_height);

    thread first2(morfologyProc,this,temp,ss,thick,strukt,4*t_height,5*t_height);
    thread second2(morfologyProc,this,temp,ss,thick,strukt,5*t_height,6*t_height);
    thread third2(morfologyProc,this,temp,ss,thick,strukt,6*t_height,7*t_height);
    thread fourth2(morfologyProc,this,temp,ss,thick,strukt,7*t_height,height);

    first.join();
    second.join();
    third.join();
    fourth.join();

    first2.join();
    second2.join();
    third2.join();
    fourth2.join();


    DeleteObject(hBitmap);
    //memcpy(image, ss*width+temp+ss, width*height*sizeof(COLORREF));
/*
    for(int i=0;i<height;i++){
            memcpy( width*i+image, (2*ss+width) * (i+ss) + temp+ss, width*sizeof(COLORREF));
    }
*/
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );


    free(temp);
    temp=NULL;
}

void cImage::morfology(int s, bool add){
    int rr, gg, bb;
    int ss = s/2;
    int s2 = s*s;

    COLORREF *temp = (COLORREF*) calloc((2*ss+width)*(2*ss+height),sizeof(COLORREF));

    for(int i=0;i<height;i++){
            memcpy( (2*ss+width) * (i+ss) + temp+ss, width*i+image, width*sizeof(COLORREF));
    }


    for(int i=0;i<ss;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=height+ss;i<2*ss+height;i++)
        for(int j=ss;j<width+ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=0;i<height+ss;i++)
        for(int j=0;j<ss;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }
    for(int i=0;i<height+ss;i++)
        for(int j=ss+width;j<2*ss+width;j++){
                temp[i*(2*ss+width)+j]=RGB(255,255,255);
        }

    int black=0;
    int cb,cg,cr;

    for(int i=ss;i<height+ss;i++)
        for(int j=ss;j<width+ss;j++){
            black=0;
            cb = (GetRValue(temp[(i)*(2*ss+width)+j]));
            cg = (GetGValue(temp[(i)*(2*ss+width)+j]));
            cr = (GetBValue(temp[(i)*(2*ss+width)+j]));
            for(int k=-ss;k<=ss;k++)
                for(int m=-ss;m<=ss;m++){
                    bb = (GetRValue(temp[(i+k)*(2*ss+width)+j+m]));
                    gg = (GetGValue(temp[(i+k)*(2*ss+width)+j+m]));
                    rr = (GetBValue(temp[(i+k)*(2*ss+width)+j+m]));
                    if(bb+gg+rr==0)
                        black++;
                }

            if(add&&black>0)
                    cb=cg=cr=0;
            else if(!add&&black<9)
                    cb=cg=cr=255;

            image[(i-ss)*width+(j-ss)]=RGB( cb, cg, cr);
        }

    DeleteObject(hBitmap);
    //memcpy(image, ss*width+temp+ss, width*height*sizeof(COLORREF));
/*
    for(int i=0;i<height;i++){
            memcpy( width*i+image, (2*ss+width) * (i+ss) + temp+ss, width*sizeof(COLORREF));
    }
*/
    hBitmap = CreateBitmap(width, height, 1, 32, ((void*)(image)) );


    free(temp);
    temp=NULL;
}
