#include<iostream>
#include<fstream>
#include<random>
#include<vector>
#include<algorithm>

using namespace std;

//DATA_SIZE個の一様乱数の生成による統計量の計算
double Caluculation(int DATA_SIZE){
  //１から10を一様乱数に従って返す
  random_device rnd;
  mt19937 mt(rnd());
  uniform_int_distribution<int> ten(1,10);
  //DATA_SIZE回一様乱数を生成して観測度数を求める
  int ObservedFRQ[10]={};  //ObservedFRQ=観測度数
  float ExpectedFRQ = DATA_SIZE/10.0;   //ExpectedFRQ=期待度数 は事象によらない
  for(int i=0; i<DATA_SIZE; i++){
    ObservedFRQ[ten(mt)-1]++;
  }
  //資料より統計量X^2を計算して求める。一様乱数であるので各事象の起こる確率は一律1/10とする。
  int sum=0;
  for(int i=0; i<10; i++){
    sum += ObservedFRQ[i]*ObservedFRQ[i];
  }
  //統計量が求まる
  double sum2 = sum/ExpectedFRQ - DATA_SIZE;
  return sum2;
}

int main(){
  //データサイズを入力
  int DATA_SIZE;
  cout << "DATA_SIZE => ";
  cin >> DATA_SIZE;
  //REPEAT_NUMを入力
  int REPEAT_NUM;
  cout << "REPEAT_NUM => ";
  cin >> REPEAT_NUM;
  //txtファイルの名前を入力
  string name;
  cout << "Filename => ";
  cin >> name;
  ofstream outputfile(name+".txt");
  //REPEAT_NUM回繰り返してヒストグラムを得る
  //統計量を保存する配列
  vector<double> Statistics;
  for(int i=0; i<REPEAT_NUM; i++){
    Statistics.push_back(Caluculation(DATA_SIZE));
  }
  //結果をtxtファイルに保存する
  for(int i=0; i<REPEAT_NUM; i++){
    outputfile << Statistics[i] << "\n";
  }
  outputfile.close();
  return 0;
}
