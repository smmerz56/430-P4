
#include "Shop_org.h"

void Shop_org::init() 
{
   pthread_mutex_init(&mutex_, NULL);
   pthread_cond_init(&cond_customers_waiting_, NULL);
   pthread_cond_init(&cond_customer_served_, NULL);
   pthread_cond_init(&cond_barber_paid_, NULL);
   pthread_cond_init(&cond_barber_sleeping_, NULL);
}

string Shop_org::int2string(int i) 
{
   stringstream out;
   out << i;
   return out.str( );
}

void Shop_org::print(int person, string message)
{
   cout << ((person > 0) ? "customer[" + int2string(person) : "barber  [" + int2string(person * -1)) << "]: " << message << endl;
}

int Shop_org::get_cust_drops() const
{
    return cust_drops_;
}

int Shop_org::visitShop(int id) 
{
   pthread_mutex_lock(&mutex_);
   
   // If all chairs are full then leave shop
   if (waiting_chairs_.size() == max_waiting_cust_) 
   {
      print( id,"leaves the shop because of no available waiting chairs.");
      ++cust_drops_;
      pthread_mutex_unlock(&mutex_);
      return -1;
   }
   
   bool available_chair = false;
   int i;
   for(i = 0; i < num_barbers; i++){
     if(barber_chairs[i] == 0){
       barber_chairs[i] = id; //put cust id in the barber chair
       available_chair = true;
       break;
     }
   }
   
   // If someone is being served or transitioning waiting to service chair
   // then take a chair and wait for service
   if(!available_chair || !waiting_chairs_.empty()) //this needs to be changed******************* if(i == num_barbers || !waiting_chairs_.empty())
   {
      waiting_chairs_.push(id);
      print(id, "takes a waiting chair. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
      pthread_cond_wait(&cond_customers_waiting_, &mutex_);
      waiting_chairs_.pop();
   }
 /*  if (customer_in_chair_ != 0 || !waiting_chairs_.empty()) //this needs to be changed******************* if(i == num_barbers || !waiting_chairs_.empty())
   {
      waiting_chairs_.push(id);
      print(id, "takes a waiting chair. # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size()));
      pthread_cond_wait(&cond_customers_waiting_, &mutex_);
      waiting_chairs_.pop();
   }*/
  
   print(id, "moves to the service chair[" + int2string(i) + "], # waiting seats available = " + int2string(max_waiting_cust_ - waiting_chairs_.size())); 
   
   customer_in_chair_ = id;// I dont think I need it anymore because of the for loop above
   in_service_ = true;

   // wake up the barber just in case if he is sleeping
   pthread_cond_signal(&cond_barber_sleeping_);

   pthread_mutex_unlock(&mutex_); 
   return i;//return the number of the barber
}

void Shop_org::leaveShop(int id, int barber_id) 
{
   pthread_mutex_lock( &mutex_ );

   // Wait for service to be completed
   print(id, "wait for the hair-cut to be done");
   while (in_service_ == true)
   {
      pthread_cond_wait(&cond_customer_served_, &mutex_);
   }
   
   // Pay the barber and signal barber appropriately
   money_paid_ = true;
   pthread_cond_signal(&cond_barber_paid_);
   print( id, "says good-bye to barber[" + int2string(barber_id) + "]");// need to print barber_id vvvv
  
   pthread_mutex_unlock(&mutex_);
}

void Shop_org::helloCustomer(int id) 
{
   pthread_mutex_lock(&mutex_);
   
   // If no customers than barber can sleep
  /* if (waiting_chairs_.empty() && barber_chairs[id] == 0 ) //if (waiting_chairs_.empty() && barber_chairs[id] == 0)
   {
      print(barber, "sleeps because of no customers.");
      pthread_cond_wait(&cond_barber_sleeping_, &mutex_);
   }   */
   if (waiting_chairs_.empty() && customer_in_chair_ == 0 ) //if (waiting_chairs_.empty() && barber_chairs[id] == 0)
   {
      print(id, "sleeps because of no customers.");
      pthread_cond_wait(&cond_barber_sleeping_, &mutex_);
   }
   
  /* if (barber_chairs[id] == 0)               // check if the customer, sit down.
   {
       pthread_cond_wait(&cond_barber_sleeping_, &mutex_);
   }*/
   if (customer_in_chair_ == 0)               // check if the customer, sit down.
   {
       pthread_cond_wait(&cond_barber_sleeping_, &mutex_);
   }

   print(barber, "starts a hair-cut service for customer[" + int2string( customer_in_chair_ ) + "]" );
   pthread_mutex_unlock( &mutex_ );
}

void Shop_org::byeCustomer(int barber_id) 
{
  pthread_mutex_lock(&mutex_);

  // Hair Cut-Service is done so signal customer and wait for payment
  in_service_ = false;
  print(barber_id, "says he's done with a hair-cut service for customer[" + int2string(customer_in_chair_) + "]");
  //print(barber, "says he's done with a hair-cut service for " + int2string(customer_in_chair_));
  
  money_paid_ = false;
  pthread_cond_signal(&cond_customer_served_);
  while (money_paid_ == false)
  {
      pthread_cond_wait(&cond_barber_paid_, &mutex_);
  }

  //Signal to customer to get next one
  barber_chairs[barber_id] = 0;
  //customer_in_chair_ = 0;
  print(barber, "calls in another customer");
  pthread_cond_signal( &cond_customers_waiting_ );

  pthread_mutex_unlock( &mutex_ );  // unlock
}
