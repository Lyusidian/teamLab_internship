//時間駆動からイベント駆動(スキップ)に変更
#include<iostream>
#include<random>
#include<math.h>

using namespace std;

//客のレコード
struct listNode{
  int ID;   //到着順につける
  int event_type;   //arrive(0) or laeve(1) or wait(2)
  double occur_time;  //event_typeの時間
  double enter_time;  //入店した時点の時間
  listNode *next;
};

typedef listNode* customer;

//arrive_timeとservice_timeの導出は同様
double occurtime(double arrive_ave){  //double service_time
  double lambda_a = arrive_ave; //到着時間率 //同様に、double lambda_s = service_ave;  //サービス時間率
  //1から10を返す一様乱数を生成
  random_device rnd;
  mt19937 mt(rnd());
  uniform_real_distribution<double> gen_rand_uni_real(0.0, 1.0);
  //次の到着時間・退去までの時間を求める
  double Ft = gen_rand_uni_real(mt);
  double arrive_time;
  if(Ft == 1.0)
    arrive_time = 0;
  //1-Ft/10 は一様性より Ft/10 と同じ分布である
  else{
    arrive_time = -log(Ft)/lambda_a; //到着するのにかかる時間 //同様にint service_time = -exp(Ft/10)/lambda_s;  //退出までにかかる時間
  }
  return arrive_time; //同様に、return service_time;
}

void que_addRecord(listNode* &head, int id, int type, double o_time, double e_time){
  listNode* newNode = new listNode;
  newNode->ID = id;
  newNode->event_type = type; //arrive(0) or laeve(1) or wait(2)
  newNode->occur_time = o_time; //到着時間(前の人の到着時間＋arrive_time)・退却時間(前の人の退却時間＋service_time)
  newNode->enter_time = e_time;
  if(head == NULL){
    newNode->next = NULL;
    head = newNode;   //もともと末尾リストへの
  }
  else{
    listNode* current_head = head;
    while(current_head->next != NULL){
      current_head = current_head->next;
    } //NULLで抜ける
    newNode->next = NULL;
    current_head->next = newNode;
  }
}

void event_addRecord(listNode* &head, int id, int type, double o_time, double e_time){
  listNode* copy_head = head;
  listNode* newNode = new listNode;
  newNode->ID = id;
  newNode->event_type = type; //arrive(0) or laeve(1) or wait(2)
  newNode->occur_time = o_time; //到着時間(前の人の到着時間＋arrive_time)・退却時間(前の人の退却時間＋service_time)
  newNode->enter_time = e_time;
  newNode->next = NULL;
  if(head == NULL){
    head = newNode;   //もともと末尾リストへの
  }
  else{
    listNode* prev = NULL;
    while(copy_head != NULL){
        if(copy_head->occur_time>o_time){
          if(prev == NULL){
            newNode->next = head;
            head = newNode;
          }
          else{
            newNode->next = prev->next;
            prev->next = newNode;
          }
          return;
        }
      prev = copy_head;
      copy_head = copy_head->next;
    }
    prev->next = newNode;
  }
}

//先頭を削除する。
void removeRecord3(listNode* &head){
  listNode* rhead=head;
  if(head != NULL)
    head=rhead->next;
  else{
    cout << "headがNULLです" << endl;
  }
}

//ポインタを受け取り、連結リストのリストの数を数えて返す
int countlist(listNode* &head){
  listNode* current_head = head;
  int countlist = 0;
  if(current_head == NULL){
    return 0;
  }
  while(current_head != NULL){
    current_head = current_head->next;
    countlist++;
  }
  return countlist;
}

int main(){
  //二つの入力値からそれぞれのλを求める
  double arrive_ave=0.9; //平均到着率λ
  double service_ave=1.9; //平均サービス率μ

  cout << "単位時間に到着する平均客数(λ)は";
  cin >> arrive_ave;
  cout << "単位時間にサービスされる客数(μ)は";
  cin >> service_ave;

  double real_time = 0.0;  //現在時刻 real_time
  double prev_time = 0.0;
  double count_existCustomer = 0.0; //銀行内に存在する平均客数の元
  double count_stayTime = 0.0; //客の滞在時間の合計
  double service_sum_time = 0.0;

  int arrive_customer = 0; //到着した客の数
  int service_customer = 0;  //サービス受けた客の数　

  int in_busy = 0; //1:窓口サービス中 / 0:窓口空いてる

  customer Event = nullptr;
  //一人目をイベント表に追加
  event_addRecord(Event, 1, 0, real_time+occurtime(arrive_ave), 0);
  arrive_customer++;
  //待ち行列表Queue
  customer Queue = nullptr;

  //イベント駆動型のシミュレーションを行う
  while(real_time < 100000){  //100人まで

    //イベント表から先頭の客を取り出し、表から削除する。
    listNode* head_cus = Event;
    removeRecord3(Event);
    prev_time = real_time;
    real_time = head_cus->occur_time; //時間をスキップして更新

    count_existCustomer += (countlist(Queue)+in_busy)*(real_time - prev_time);

    double service_time = 0.0;
    double arrive_time = 0.0;

    //到着イベント(event_type=0)
    if(head_cus->event_type == 0){
      //次の客とその到着時刻をイベント表に追加する
      arrive_customer++;

      arrive_time = occurtime(arrive_ave);
      event_addRecord(Event, head_cus->ID+1, 0, real_time+arrive_time, real_time+arrive_time);

      //サービスを受けている人がいるとき<=>その人の退出がイベント表にある -> 行列表に加える
      if(in_busy == 1){
        que_addRecord(Queue, head_cus->ID, 2, 0, real_time);
      }
      //窓口が開いているとき -> サービスを受ける
      else{
        service_time = occurtime(service_ave);
        service_sum_time += service_time;
        count_stayTime += service_time;
        event_addRecord(Event, head_cus->ID, 1, real_time+service_time, head_cus->enter_time);
        in_busy = 1;
      }
    }

    //退出イベント(event_type=1)
    else if(head_cus->event_type == 1){
      service_customer++;
      in_busy = 0;
      //待ち行列に客がいる場合
      if(Queue != NULL){
        customer head_que = Queue;
        removeRecord3(Queue);
        service_time = occurtime(service_ave);
        service_sum_time += service_time;
        event_addRecord(Event, head_que->ID, 1, real_time+service_time, head_que->enter_time);
        in_busy = 1;
        count_stayTime += (real_time + service_time - head_que->enter_time);
      }
    }
  }

  //理論値と出力を比較する
  double stayTime_ave = 1.00/(service_ave - arrive_ave); // 1/(μ-λ)
  double rho = arrive_ave/service_ave;  //ρ
  double existCustomer_ave = rho/(1-rho); // ρ/(1-ρ)
  //cout << "平均到着率λ(理論値,実験値)" << arrive_ave << " / " << arrive_customer/real_time << endl;
  //cout << "arrive_customer: "<< arrive_customer << " / real_time: " << real_time << endl;
  //cout << "平均サービス率μ(理論値,実験値)" << service_ave << " / " << service_customer/service_sum_time << endl;
  //cout << "service_customer: " << service_customer << " / service_sum_time: " << service_sum_time << endl;
  cout << "平均滞在時間(理論値,実験値)" << stayTime_ave << " / " << count_stayTime/service_customer << endl;
  //cout << "count_stayTime: " << count_stayTime << " / service_customer: " << service_customer << endl;
  cout << "平均客数(理論値,実験値)" << existCustomer_ave << " / " << count_existCustomer/real_time << endl;
  //cout << "count_existCustomer: " << count_existCustomer << " / real_time: " << real_time << endl;

  return 0;
}
