#include<iostream>
#include<random>
#include<math.h>
#include<string>

using namespace std;

//客レコードクラス
class customerRecord{
private:
  int _id;
  int _event_type;   //arrive(0) or laeve(1) or wait(2)
  double _occur_time;  //event_typeの時間
  double _enter_time;  //入店した時点の時間

public:
  int customerID(){ return _id;}
  void set_customerID(int in_id){ _id = in_id;}
  int event_type(){ return _event_type;}
  void set_event_type(int in_event_type){ _event_type = in_event_type;}
  double occur_time(){ return _occur_time;}
  void set_occur_time(double in_occur_time){ _occur_time = in_occur_time;}; //
  double enter_time(){ return _enter_time;}
  void set_enter_time(double in_real_time){ _enter_time = in_real_time;}

  customerRecord(){ set_customerID(-1); set_event_type(-1); set_occur_time(0.0); set_enter_time(0.0);} //デフォルトコンストラクタ
  customerRecord(int _id, int _event_type, double _occur_time, double _enter_time){ set_customerID(_id); set_event_type(_event_type); set_occur_time(_occur_time); set_enter_time(_enter_time);} //引数を持つコンストラクタ
};

//客コレクションクラス
class customerCollection{
private:
  struct customerNode{
    customerRecord customerData;
    customerNode* next;
  };
  typedef customerNode* customerList;
  customerList _listHead;

  void deleteList(customerList &listPtr); //
  customerList copiedList(const customerList original); //
  //customerList listHead(){ return _listHead;}

public:
  customerCollection(){ _listHead=NULL;}
  customerCollection(const customerCollection &original){ _listHead = copiedList(original._listHead);}
  ~customerCollection(){ deleteList(_listHead);}

  customerCollection& operator=(const customerCollection &cc); //
  //friend ostream& operator<<(ostream& os, const customerCollection &cc); //
  customerList listHead(){ return _listHead;}
  void que_addRecord(customerRecord newCustomer); //
  void event_addRecord(customerRecord newCustomer); //
  void removeRecord(); //
  int countlist(); //
};

void customerCollection::deleteList(customerList &listPtr){
  while(listPtr != NULL){
    customerNode *temp = listPtr;
    listPtr = listPtr->next;
    delete temp;
  }
}

customerCollection::customerList customerCollection::copiedList(const customerList original){
  if(original != NULL) return NULL;
  customerList newList = new customerNode;
  newList->customerData = original->customerData;
  customerNode *oldp = original->next;
  customerNode *newp = newList;
  while(oldp != NULL){
    newp->next = new customerNode;
    newp = newp->next;
    newp->customerData = oldp->customerData;
    oldp = oldp->next;
  }
  newp->next = NULL;
  return newList;
}

//代入＋関数
customerCollection& customerCollection::operator=(const customerCollection &sc){
  if(this != &sc){
    deleteList(_listHead);
    _listHead = copiedList(sc._listHead);
  }
  return *this;
}

//待ち行列表の末尾に追加
void customerCollection::que_addRecord(customerRecord newCustomer){
  customerNode* newNode = new customerNode;
  newNode->customerData = newCustomer;
  if(_listHead == NULL){
    newNode->next = NULL;
    _listHead = newNode;
  }
  else{
    customerNode *p = _listHead;
    while(p->next != NULL)
      p = p->next;
    newNode->next = NULL;
    p->next = newNode;
  }
}

void customerCollection::event_addRecord(customerRecord newCustomer){
  customerNode* newNode = new customerNode;
  newNode->customerData = newCustomer;
  if(_listHead == NULL){
    newNode->next = NULL;
    _listHead = newNode;
  }
  else{
    customerNode* p = _listHead;
    customerNode* prev = NULL;
    while(p != NULL){
      if(p->customerData.occur_time() > newNode->customerData.occur_time()){
        if(prev == NULL){
          newNode->next = _listHead;
          _listHead = newNode;
        }
        else{
          newNode->next = prev->next;
          prev->next = newNode;
        }
        return;
      }
      prev = p;
      p = p->next;
    }
    prev->next = newNode;
  }
}

//先頭を削除する
//void customerCollection::removeRecord(customerCollection &cc)
void customerCollection::removeRecord(){
  customerNode* p = _listHead;
  if(p != NULL)
    _listHead = p->next;
  else{
    cout << "headがNULLです" << endl;
  }
}

//int customerCollection::countlist(customerCollection &cc)
int customerCollection::countlist(){
  customerNode* p = _listHead;
  int countlist = 0;
  if(p == NULL){
    return 0;
  }
  while(p != NULL){
    p = p->next;
    countlist++;
  }
  return countlist;
}

//arrive_timeとservice_timeの導出は同様
double culculate_occur_time(double arrive_ave){  //double service_time
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

  customerCollection Event;
  customerCollection Queue;
  customerRecord cus1(1, 0, real_time+culculate_occur_time(arrive_ave), real_time);
  Event.event_addRecord(cus1);
  arrive_customer++;

  while(real_time < 100000){
    customerRecord head_cus = (Event.listHead())->customerData;
    Event.removeRecord();
    prev_time = real_time;
    real_time = head_cus.occur_time();

    count_existCustomer += (Queue.countlist() + in_busy)*(real_time - prev_time);

    double service_time = 0.0;
    double arrive_time = 0.0;

    //到着イベント
    if(head_cus.event_type() == 0){
      //cout << "l248 " << endl;
      arrive_customer++;
      arrive_time = culculate_occur_time(arrive_ave);
      customerRecord cus2(head_cus.customerID()+1, 0, real_time+arrive_time, real_time+arrive_time);
      Event.event_addRecord(cus2);


      if(in_busy == 1){
        //cout << "l255 " << endl;
        customerRecord cus3(head_cus.customerID(), 2, 0, real_time);
        Queue.que_addRecord(cus3);


      }
      else{
        //cout << "l260 " << endl;
        service_time = culculate_occur_time(service_ave);
        service_sum_time += service_time;
        count_stayTime += service_time;
        customerRecord cus4(head_cus.customerID(), 1, real_time+service_time, head_cus.enter_time());
        Event.event_addRecord(cus4);

        in_busy = 1;
      }
    }

    //退出イベント　
    else if(head_cus.event_type() == 1){
      //cout << "l272 " << endl;
      service_customer++;
      in_busy = 0;

      if(Queue.countlist() != 0){
        //cout << "l277 " << endl;
        customerRecord head_que = (Queue.listHead())->customerData;
        Queue.removeRecord();
        service_time = culculate_occur_time(service_ave);
        service_sum_time += service_time;
        customerRecord cus5(head_que.customerID(), 1, real_time+service_time, head_que.enter_time());
        Event.event_addRecord(cus5);

        in_busy = 1;
        count_stayTime += (real_time + service_time - head_que.enter_time());
      }
    }
  }

  double stayTime_ave = 1.00/(service_ave - arrive_ave); // 1/(μ-λ)
  double rho = arrive_ave/service_ave;  //ρ
  double existCustomer_ave = rho/(1-rho); // ρ/(1-ρ)
  cout << "平均滞在時間(理論値,実験値)" << stayTime_ave << " / " << count_stayTime/service_customer << endl;
  cout << "平均客数(理論値,実験値)" << existCustomer_ave << " / " << count_existCustomer/real_time << endl;

  return 0;
}
