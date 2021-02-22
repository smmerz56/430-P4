#ifndef SHOP_ORG_H_
#define SHOP_ORG_H_
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <queue>
using namespace std;

#define kDefaultNumChairs 3
#define kDefaultBarbers 1

class Shop_org 
{
public:
 /* Shop_org(int num_barbers, int num_chairs){
    init();
    int[num_barbers] barber_chairs;
  }
  
  Shop_org(){
    
  }*/
  
   Shop_org(int b, int num_chairs) : max_waiting_cust_((num_chairs > 0 ) ? num_chairs : kDefaultNumChairs), customer_in_chair_(0),
      in_service_(false), money_paid_(false), cust_drops_(0), num_barbers(b) 
   { 
      init();
      barber_chairs = new int[b]; 
      fill_n(barber_chairs, b, 0);
      //num_barbers(b);
      
      //go through barber_chairs and set to 0
   };
   
   Shop_org() : max_waiting_cust_(kDefaultNumChairs), customer_in_chair_(0), in_service_(false),
      money_paid_(false), cust_drops_(0), num_barbers(kDefaultBarbers) 
   { 
      init(); 
   };

   int visitShop(int id);   // return true only when a customer got a service
   void leaveShop(int id, int barber_id);
   void helloCustomer(int id);
   void byeCustomer(int id);
   int get_cust_drops() const;

 private:
   const int num_barbers;// new
   
   const int max_waiting_cust_;              // the max number of threads that can wait
   int customer_in_chair_;// this might need to be changed becuase of multiple barber chairs now
   
   //above to be changed to int[] barber_chairs with the cust # at the chair or cell
   int *barber_chairs;//pointer to array 
   
   bool in_service_;            
   bool money_paid_;
   queue<int> waiting_chairs_;  // includes the ids of all waiting threads
   int cust_drops_;

   // Mutexes and condition variables to coordinate threads
   // mutex_ is used in conjuction with all conditional variables
   pthread_mutex_t mutex_;
   pthread_cond_t  cond_customers_waiting_;
   pthread_cond_t  cond_customer_served_;
   pthread_cond_t  cond_barber_paid_;
   pthread_cond_t  cond_barber_sleeping_;

   static const int barber = 0; // the id of the barber thread***needs to be replaced********************
  
   void init();
   string int2string(int i);
   void print(int person, string message);
};
#endif
