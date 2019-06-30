
#include <stdio.h>
#include <stdlib.h>

/*
 * マクロ定義
 */
#define min(A, B) ((A)<(B) ? (A) : (B))
#define max(A, B) ((A)>(B) ? (A) : (B))
//二乗計算
#define sq(x) ((x)*(x))
//三乗計算
#define cu(x) ((x)*(x)*(x))

/*
 * 画像構造体の定義
 */
typedef struct
{
    int width;              /* 画像の横方向の画素数 */
    int height;             /* 画像の縦方向の画素数 */
    int maxValue;           /* 画像の値（明るさ）の最大値 */
    unsigned char *data;    /* $B2hA|$N2hAGCM%G!<%?$r3JG<$9$kNN0h$r;X$9(B */
                            /* $B%]%$%s%?(B */
} image_t;

void
parseArg(int argc, char **argv, FILE **infp, FILE **outfp)
{
    FILE *fp;

    if (argc!=3)
    {
        goto usage;
    }

    *infp = fopen(argv[1], "rb");

    if (*infp==NULL)
    {
        fputs("Opening the input file was failend\n", stderr);
        goto usage;
    }

    *outfp = fopen(argv[2], "wb");


    if (*outfp==NULL)
    {
        fputs("Opening the output file was failend\n", stderr);
        goto usage;
    }

    return;

usage:
    fprintf(stderr, "usage : %s <input pgm file> <output pgm file>\n", argv[0]);
    exit(1);
}

void
initImage(image_t *ptImage, int width, int height, int maxValue)
{
    ptImage->width = width;
    ptImage->height = height;
    ptImage->maxValue = maxValue;

    ptImage->data = (unsigned char *)malloc((size_t)(width * height));

    if (ptImage->data==NULL)
    {
        fputs("out of memory\n", stderr);
        exit(1);
    }
}

char *
readOneLine(char *buf, int n, FILE *fp)
{
    char *fgetsResult;

    do
    {
        fgetsResult = fgets(buf, n, fp);
    } while(fgetsResult!=NULL && buf[0]=='#');

    return fgetsResult;
}

void
readPgmRawHeader(FILE *fp, image_t *ptImage)
{
    int width, height, maxValue;
    char buf[128];


    if(readOneLine(buf, 128, fp)==NULL)
    {
        goto error;
    }
    if (buf[0]!='P' || buf[1]!='5')
    {
        goto error;
    }

    if (readOneLine(buf, 128, fp)==NULL)
    {
        goto error;
    }
    if (sscanf(buf, "%d %d", &width, &height) != 2)
    {
        goto error;
    }
    if ( width<=0 || height<=0)
    {
        goto error;
    }

    if (readOneLine(buf, 128, fp)==NULL)
    {
        goto error;
    }
    if (sscanf(buf, "%d", &maxValue) != 1)
    {
        goto error;
    }
    if ( maxValue<=0 || maxValue>=256 )
    {
        goto error;
    }

    initImage(ptImage, width, height, maxValue);

    return;

error:
    fputs("Reading PGM-RAW header was failed\n", stderr);
    exit(1);
}

void
readPgmRawBitmapData(FILE *fp, image_t *ptImage)
{
    if( fread(ptImage->data, sizeof(unsigned char),
            ptImage->width * ptImage->height, fp)
            != ptImage->width * ptImage->height )
    {
        fputs("Reading PGM-RAW bitmap data was failed\n", stderr);
        exit(1);
    }
}


/*======================================================================
 * フィルタリング（ネガポジ反転）
 *======================================================================
 *   画像構造体image_t *originalImage の画像をフィルタリング（ネガポジ反転）して
 *   image_t *originalImage に格納する。
 */
void
filteringImage(image_t *resultImage, image_t *originalImage)
{
    int     x, y;
    int     width, height;

    /* originalImage と resultImage のサイズが違う場合は、共通部分のみを処理する */
    width = min(originalImage->width, resultImage->width);
    height = min(originalImage->height, resultImage->height);

    for(y=0; y<height; y++)
    {
        for(x=0; x<width; x++)
        {
            resultImage->data[x+resultImage->width*y]
                    = ( originalImage->maxValue
                    -originalImage->data[x+originalImage->width*y] )
                    *resultImage->maxValue/originalImage->maxValue;
        }
    }
}

//prewitフィルタによるエッジ強調
void prewit_edge_enhance(image_t *resultImage, image_t *originalImage){
  int x,y;
  int width, height;

  /* originalImage と resultImage のサイズが違う場合は、共通部分のみを処理する */
  width = min(originalImage->width, resultImage->width);
  height = min(originalImage->height, resultImage->height);

  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      //----例外処理について----
      //一次元配列なので、配列の範囲外を参照してしまう計算を行うのは
      //data[0],data[width-1 + width*(height-1)]の時のみである。
      int df_di, df_dj;
      if(x-1 + y*originalImage->width == 0){
        //右隣と同じにする
        df_di = originalImage->data[x+2 + y*originalImage->width] - originalImage->data[x + y*originalImage->width];
      }
      else{
        df_di = originalImage->data[x+1 + y*originalImage->width] - originalImage->data[x-1 + y*originalImage->width];
      }
      if(x + (y+1)*originalImage->width == (originalImage->width)*(originalImage->height)-1){
        //左隣と同じにする
        df_dj = originalImage->data[x + (y)*originalImage->width] - originalImage->data[x + (y-2)*originalImage->width];
      }
      else{
        df_dj = originalImage->data[x + (y+1)*originalImage->width] - originalImage->data[x + (y-1)*originalImage->width];
      }

      if(df_di<0) df_di = -df_di;
      if(df_dj<0) df_dj = -df_dj;
      /* df_di+df_dj が255を超える場合も考慮しなければならない */
      if(df_di+df_dj > 255){
        resultImage->data[x + resultImage->width*y] = 255;
      }
      else{
        resultImage->data[x + resultImage->width*y] = (df_di + df_dj);
      }
    }
  }
}

//大津の方法による動的閾値決定と二値化
void Otsu_binarization(image_t *resultImage, image_t *originalImage){
  int     x, y;
  int     width, height;

  /* originalImage と resultImage のサイズが違う場合は、共通部分のみを処理する */
  width = min(originalImage->width, resultImage->width);
  height = min(originalImage->height, resultImage->height);

  //階調数
  int L = 256;
  //全画素数 N
  int N = width*height;
  //階調数がiである画素の数 n_i
  int n_i[256];
  //初期化
  for(int i=0; i<256; i++){
    n_i[i] = 0;
  }
  //n_iの計算
  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      n_i[ originalImage->data[x + originalImage->width*y] ] += 1;
    }
  }
  //暫定のT
  int T=0;
  //暫定のsigma_Tの値
  float sigma_T=0.0;
  for(int k=0; k<255; k++){ //k-1 = 255まで

    //i=0からi=kまでのn_iの総和 a_1
    //i=k+1からi=L-1までのn_iの総和 a_2
    int a_1=0, a_2=0;
    //i=0からi=kまでのi*n_iの総和 b_1
    //i=k+1からi=L-1までのi*n_iの総和 b_2
    int b_1=0, b_2=0;
    //a_1,b_1の計算
    for(int i=0; i<=k; i++){
      a_1 += n_i[i];
      b_1 += i*n_i[i];
    }

    //a_2,b_2の計算
    for(int i=k+1; i<= L-1; i++){
      a_2 += n_i[i];
      b_2 += i*n_i[i];
    }

    float sigma_B=0;
    sigma_B = ( sq( b_1-a_1*(b_1+b_2)/(float)N )/(N*a_1)+sq( b_2-a_2*(b_1+b_2)/(float)N )/(N*a_2) )/(float)N;

    if(sigma_B > sigma_T){
      sigma_T = sigma_B;
      T = k;
    }

  }

  for(y=0; y<height; y++){
    for(x=0; x<width; x++){
      if(originalImage->data[x + originalImage->width*y] <= T)
        resultImage->data[x + originalImage->width*y] = 0;
      else
        resultImage->data[x + originalImage->width*y] = 255;
    }
  }
}


void
writePgmRawHeader(FILE *fp, image_t *ptImage)
{
    if(fputs("P5\n", fp)==EOF)
    {
        goto error;
    }

    if (fprintf(fp, "%d %d\n", ptImage->width, ptImage->height)==EOF)
    {
        goto error;
    }

    if (fprintf(fp, "%d\n", ptImage->maxValue)==EOF)
    {
        goto error;
    }

    return;

error:
    fputs("Writing PGM-RAW header was failed\n", stderr);
    exit(1);
}

void
writePgmRawBitmapData(FILE *fp, image_t *ptImage)
{
    if( fwrite(ptImage->data, sizeof(unsigned char),
            ptImage->width * ptImage->height, fp)
            != ptImage->width * ptImage->height )
    {
        fputs("Writing PGM-RAW bitmap data was failed\n", stderr);
        exit(1);
    }
}


/*
 * メイン
 */
int
main(int argc, char **argv)
{
    image_t originalImage, resultImage;
    FILE *infp, *outfp;

    /* 引数の解析 */
    parseArg(argc, argv, &infp, &outfp);

    /* 元画像の画像ファイルのヘッダ部分を読み込み、画像構造体を初期化する */
    readPgmRawHeader(infp, &originalImage);

    /* 元画像の画像ファイルのビットマップデータを読みこむ */
    readPgmRawBitmapData(infp, &originalImage);

    /* 結果画像の画像構造体を初期化する。画素数・階調数は元画像と同じ */
    initImage(&resultImage, originalImage.width, originalImage.height,
            originalImage.maxValue);

    /* prewitフィルタによりエッジ強調を行ったあと、二知華を行う */
    prewit_edge_enhance(&resultImage, &originalImage);

    /* 画像ファイルのヘッダ部分の書き込み */
    writePgmRawHeader(outfp, &resultImage);

    /* 画像ファイルのビットマップデータの書き込み */
    writePgmRawBitmapData(outfp, &resultImage);

    return 0;
}
