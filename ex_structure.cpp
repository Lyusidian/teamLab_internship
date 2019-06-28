#include<iostream>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

struct student{
  int grade;
  int studentID;
  string name;
};
//構造体の配列
const int NUM_STUDENTS = 10;
student studentArray[NUM_STUDENTS] = {
  {87, 10001, "Fred"},    //7
  {28, 10002, "Tom"},     //10
  {100, 10003, "Alice"},  //2
  {78, 10004, "John"},    //8
  {84, 10005, "Dave"},    //5
  {98, 10006, "Bob"},     //3
  {75, 10007, "Charlie"}, //4
  {70, 10008, "Eve"},     //6
  {81, 10009, "Adam"},    //1
  {68, 10010, "Steve"}    //9
};

int compareFunc(const void*voidA,const void*voidB){
  int*intA = (int*)(voidA);
  int*intB = (int*)(voidB);
  return *intA-*intB;
}

int main(){
//点数のデータのみ抜き出して新しく配列に入れる
  int point[10];
  for(int i=0;i<10;i++){
    point[i]=studentArray[i].grade;
  }
//点数順にクイックソート
  qsort(point,10,sizeof(int),compareFunc);
//中央値を取得する
  float med;
  med = (point[4]+point[5])/2.00;
  cout << "中央値 " << med << endl;
  return 0;
}
