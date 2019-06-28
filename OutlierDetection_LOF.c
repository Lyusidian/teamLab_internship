/*
	外れ値検知技術の一つであるLOFの実装
*/
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

//２点間のユークリッド距離を返す
double calc_two_distance( double A_x, double A_y, double B_x, double B_y){
	double dif_x = A_x - B_x;  //x座標の差
	double dif_y = A_y - B_y;  //y座標の差
	return sqrt(dif_x*dif_x + dif_y*dif_y);
}

//k_distance(A(or B))を計算する関数  x,yはAorBの座標
//Aからk番目に近い点とAとのユークリッド距離を返す関数　
double k_distance(double data[][2], double x, double y, int k){
  //各データの点と選んだ点との距離を入れていく
  double dist[200];
  for(int i=0; i<200; i++){
    dist[i] = calc_two_distance(data[i][0], data[i][1], x, y);
  }
  //ソートを行う
  for(int i=0; i<200; i++){
    double tmp;
    for(int j=i+1; j<200; j++){
      if(dist[i]>dist[j]){
        tmp = dist[i];
        dist[i] = dist[j];
        dist[j] = tmp;
      }
    }
  }
  return dist[k+1]; //dist[0]=0.0であるため
}

//ある配列に対して、ある値を持つ要素の番号を返す　
double search_ele_num(double dist[], double num){
  int ele_num = 0;
  for(int i=0; i<200; i++){
    if(dist[i]==num){
      ele_num = i;
    }
  }
  return ele_num;
}

int main(int argc, char const *argv[]){
  FILE *fp_in, *fp_out;
	int input;
	int count=0;
	double data[200][2];// the number of data is 200
	int label[200];
	int i=0;

  //Input//
	fp_in = fopen(argv[1],"r");
	if(fp_in==NULL){
		printf("fail: cannot open the input-file. Change the name of input-file. \n");
		return -1;
	}

	while( (input=fscanf(fp_in, "%lf,%lf", &data[count][0], &data[count][1])) != EOF){
		//printf("%lf %lf\n", data[count][0], data[count][1]);
		count++;
	}

  //LOF(外れ値検知)//

  int k = 5;
  double LOF_list[200];
  for(int data_num=0; data_num<200; data_num++){
    double A_x = data[data_num][0];
    double A_y = data[data_num][1];
    //printf("A_x: %lf, A_y: %lf\n", A_x, A_y);
    double lrd_k_AB[k+1]; //1番目はlrd_k(A),2〜k+1番目はlrd_k(B)|BはN_k(A)に含まれる

  //lrd_k(A)を計算
    //Step1. Aのk近傍を求める
    //Aのk近傍、k個の点の集合　
    double Nk_A[k][2];
    //B用のNk_A
    double save_Nk_A[k][2];
    //各データの点と選んだ点との距離を入れていく
    double dist[200];
    for(int i=0; i<200; i++){
      dist[i] = calc_two_distance(data[i][0], data[i][1], A_x, A_y);
    }
    //ソート用にコピーしていく
    double sort_dist[200];
    for(int i=0; i<200; i++){
      sort_dist[i] = dist[i];
    }
    //ソートを行う。
    for(int i=0; i<200; i++){
      double tmp;
      for(int j=i+1; j<200; j++){
        if(sort_dist[i]>sort_dist[j]){
          tmp = sort_dist[i];
          sort_dist[i] = sort_dist[j];
          sort_dist[j] = tmp;
        }
      }
    }

    for(int i=0; i<k; i++){
    //  printf("sort_dist[i+1]: %lf\n", sort_dist[i+1]);
      int ele_num = search_ele_num(dist, sort_dist[i+1]); //sort_dist[0]=0
      Nk_A[i][0] = data[ele_num][0];
      Nk_A[i][1] = data[ele_num][1];
      save_Nk_A[i][0] = Nk_A[i][0];
      save_Nk_A[i][1] = Nk_A[i][1];
    }
    //Step2. lrd_k(A)を求める　
    //reach-dist_k(A,B)を計算する
    //d(A,B),k-distance_k(A,B)の大きい方を返す
    double sum_reach = 0.0; //<=> sum_Nk_Aに含まれるBについて_reach-dist_k(A,B)
    for(int i=0; i<k; i++){
      double B_x = Nk_A[i][0];
      double B_y = Nk_A[i][1];
      double d_AB = calc_two_distance(A_x, A_y, B_x, B_y);
      double k_distance_B = k_distance(data, B_x, B_y, k);  //Bからk番目に近い点とBとのユークリッド距離
      double reach_dist_k_AB = 0.0;
      if(d_AB > k_distance_B)
        reach_dist_k_AB = d_AB;
      else
        reach_dist_k_AB = k_distance_B;
      sum_reach += reach_dist_k_AB;
    }
    lrd_k_AB[0] = k/sum_reach;


  //lrd_k(B)の合計を一つのループで計算する
    for(int cnt=0; cnt<k; cnt++){
      //Step3. lrd_k(B)を求める　
      //以下、AはBに置き換え　
      A_x = save_Nk_A[cnt][0];
      A_y = save_Nk_A[cnt][1];
      //Step1. Aのk近傍を求める
      //Aのk近傍、k個の点の集合　
      double Nk_A[k][2];
      //各データの点と選んだ点との距離を入れていく
      double dist[200];
      for(int i=0; i<200; i++){
        dist[i] = calc_two_distance(data[i][0], data[i][1], A_x, A_y);
      }
      //ソート用にコピーしていく
      double sort_dist[200];
      for(int i=0; i<200; i++){
        sort_dist[i] = dist[i];
      }

      //qsort(sort_dist, sizeof(sort_dist)/sizeof(int), sizeof(int), asc);
      for(int i=0; i<200; i++){
        double tmp;
        for(int j=i+1; j<200; j++){
          if(sort_dist[i]>sort_dist[j]){
            tmp = sort_dist[i];
            sort_dist[i] = sort_dist[j];
            sort_dist[j] = tmp;
          }
        }
      }

      for(int i=0; i<k; i++){
      //  printf("sort_dist[i+1]: %lf\n", sort_dist[i+1]);
        int ele_num = search_ele_num(dist, sort_dist[i+1]); //sort_dist[0]=0
        Nk_A[i][0] = data[ele_num][0];
        Nk_A[i][1] = data[ele_num][1];
      }
      //Step2. lrd_k(A)を求める　
      //reach-dist_k(A,B)を計算する
      //d(A,B),k-distance_k(A,B)の大きい方を返す
      double sum_reach = 0.0; //<=> sum_Nk_Aに含まれるBについて_reach-dist_k(A,B)
      for(int i=0; i<k; i++){
        double B_x = Nk_A[i][0];
        double B_y = Nk_A[i][1];
        double d_AB = calc_two_distance(A_x, A_y, B_x, B_y);
        double k_distance_B = k_distance(data, B_x, B_y, k);  //Bからk番目に近い点とBとのユークリッド距離
        double reach_dist_k_AB = 0.0;
        if(d_AB > k_distance_B)
          reach_dist_k_AB = d_AB;
        else
          reach_dist_k_AB = k_distance_B;
        sum_reach += reach_dist_k_AB;
      }
       lrd_k_AB[cnt+1] = k/sum_reach;

    }

    //Step4. LOFを求める
    double LOF_k_A = 0.0;
    double sum_lrd_k_B = 0.0;
    for(int i=0; i<k; i++){
      sum_lrd_k_B += lrd_k_AB[1+i];
    }
    LOF_k_A = sum_lrd_k_B/k;  //定義式の分子
    LOF_k_A = LOF_k_A/lrd_k_AB[0];
    LOF_list[data_num] = LOF_k_A;
    }

    //Output//
  	fp_out = fopen(argv[2],"w");
  	if(fp_out==NULL){
  		printf("fail: cannot open the output-file. Change the name of output-file.  \n");
  		return -1;
  	}

    fprintf(fp_out, "%d,%d,%d\n", 0,1,2);
  	for(i=0;i<200;i++){
  		fprintf(fp_out, "%lf,%lf,%lf\n", data[i][0], data[i][1], LOF_list[i]);
  	}

    return 0;
  }
