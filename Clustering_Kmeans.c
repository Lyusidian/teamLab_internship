/*
	クラスタリングの一種であるKmeansの実装
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//２点間の距離を返す　
double calc_distance( double data[][2], double center[][2], int raw, int k){
	double dif_x = data[raw][0]-center[k][0];
	double dif_y = data[raw][1]-center[k][1];
	return sqrt(dif_x*dif_x + dif_y*dif_y);
}

//配列の最小値を返す関数 kはクラスタの数　
int min_label( double *dis_list, int k){
	int min_label = 0;	//最小値のクラスタラベル
	double min = dis_list[min_label];
	for(int i=0; i<k; i++){
		if(dis_list[i] < min){
			min = dis_list[i];
			min_label = i;
		}
	}
	return min_label;
}

double calc_each_cluster_center( double data[][2], int *label, int k, int zahyo){
	double sum = 0.0;	//あるクラスターの座標x(y)の総和
	int cnt = 0;	//あるクラスターの要素数　
	for(int i=0; i<200; i++){
		if(label[i] == k){
			sum += data[i][zahyo];
			cnt++;
		}
	}
	return sum/cnt;
}

int main(int argc, char *argv[])
{
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
		count++;
	}

	//k-means//
	//
	//Step1. k個の点をランダムに選択し、各クラスタの重心(仮)とする。
	int k=3;
	//printf("Input: ");
	//scanf("%d", &k);

	double center[k][2];
	for(int i=0; i<k; i++){
		center[i][0] = data[i][0];
		center[i][1] = data[i][1];
	}
	while(1){
		//Step2. 各点を一番近い重心のクラスタに変更する
		for(int i=0; i<200; i++){	//iはデータのraw行番号
			double dis_list[k];
			//k個あるクラスタの仮重心との距離を配列に入れていく
			for(int j=0; j<k; j++){
				dis_list[j] = calc_distance(data, center, i, j);
			}
			label[i] = min_label(dis_list, k);
		}

		//Step3. クラスタの重心を計算する。
		double new_center[k][2];
		for(int i=0; i<k; i++){
			new_center[i][0] = calc_each_cluster_center(data, label, i, 0);
			new_center[i][1] = calc_each_cluster_center(data, label, i, 1);
		}

		//Step4. Step3に変化がなければ終了する。変化がある場合はStep2に戻る。
		int chk=0;
		for(int i=0; i<k; i++){
			//printf("old_x: %lf ,new_x: %lf\n", center[i][0], new_center[i][0]);
			if(center[i][0]!=new_center[i][0] || center[i][1]!=new_center[i][1])
				chk++;
		}
		//printf("---------\n");
		if(chk==0) break;
		for(int i=0; i<k; i++){
			center[i][0] = new_center[i][0];
			center[i][1] = new_center[i][1];
		}
	}

	///////////

	//Output//
	fp_out = fopen(argv[2],"w");
	if(fp_out==NULL){
		printf("fail: cannot open the output-file. Change the name of output-file.  \n");
		return -1;
	}
	//ヘッダ
	fprintf(fp_out, "%d,%d,%d\n", 0,1,2);
	for(i=0;i<200;i++){
		fprintf(fp_out, "%lf,%lf,%d\n", data[i][0], data[i][1], label[i]);
	}
	return 0;
}
