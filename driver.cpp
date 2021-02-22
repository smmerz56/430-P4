//#include <pthread.h> 
#include <iostream>
#include <sys/time.h>
#include <unistd.h>
#include "Shop_org.h"
using namespace std;

void *barber(void *);
void *customer(void *);

// ThreadParam class
// This class is used as a way to pass more
// than one argument to a thread. 
class ThreadParam
{
public:
    ThreadParam(Shop_org* shop, int id, int service_time) :
        shop(shop), id(id), service_time(service_time) {};
    Shop_org* shop;         
    int id;             
    int service_time;    
};

int main(int argc, char *argv[]) 
{

   // Read arguments from command line
   // TODO: Validate values
   if (argc != 5)
   {
       cout << "Usage: num_barbers num_chairs num_customers service_time" << endl;
       return -1;
   }
   int num_barbers = atoi(argv[1]);// added***************
   int num_chairs = atoi(argv[2]);
   int num_customers = atoi(argv[3]);
   int service_time = atoi(argv[4]);

  cout << "here0\n" << endl;

   //Many barbers, one shop, many customers
   pthread_t barber_thread[num_barbers];// new*****************
   pthread_t customer_threads[num_customers];
   Shop_org shop(num_barbers, num_chairs);
   
   cout << "here1\n" << endl;
   
   //Create barbers
   for(int i = 0; i < num_barbers; i++){// new*****************
    // usleep(rand() % 1000);
     int barber_id = i;
     ThreadParam* barber_param = new ThreadParam(&shop, barber_id, service_time);
     pthread_create(&barber_thread[i], NULL, barber, barber_param);
   }
   
   cout << "here2\n" << endl;
   
   //create customers
   for (int i = 0; i < num_customers; i++) 
   {
      usleep(rand() % 1000);
      int id = i + 1;
      ThreadParam* customer_param = new ThreadParam(&shop, id, 0);
      pthread_create(&customer_threads[i], NULL, customer, customer_param);
   }
  // maybe join barbers instead of cancel below
  
  cout << "here3\n" << endl; 
  
   // Wait for customers to finish 
   for (int i = 0; i < num_customers; i++)
   {
       pthread_join(customer_threads[i], NULL);
   }
   
   cout << "here4\n" << endl;
   for(int i = 0; i < num_barbers; i++){// new*****************
     pthread_join(barber_thread[i], NULL);
   }
   //Cancel barbers
  /* for(int i = 0; i < num_barbers; i++){// new*****************
     pthread_cancel(barber_thread[i]);
   }*/
   

   cout << "# customers who didn't receive a service = " << shop.get_cust_drops() << endl;
   return 0;
}

void *barber(void *arg) 
{
   ThreadParam &barber_param = *(ThreadParam*) arg;
   Shop_org& shop = *(barber_param.shop);
   int id = barber_param.id;
   int service_time = barber_param.service_time;
   delete &barber_param;

   while(true) 
   {
      shop.helloCustomer(id);
      usleep(service_time);
      shop.byeCustomer(id);
   }
   return nullptr;
}

void *customer(void *arg) 
{
   ThreadParam &customer_param = *(ThreadParam*)arg;
   Shop_org &shop = *(customer_param.shop);
   int id = customer_param.id;
   delete &customer_param;

   int barber = -1;
   if ((barber = shop.visitShop(id)) != -1)// might need to change
   {
       shop.leaveShop(id, barber);//need to figure out how to pass barber_id
   }
   return nullptr;
}