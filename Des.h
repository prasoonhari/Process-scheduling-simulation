#ifndef DES__EVE
#define DES__EVE
#include <list>
#include"Process.h"
#include"Event.h"

#include <cstddef>
#include <iostream>



using namespace std;




class Des
{
public:
  Des(bool dotrace);
  Event* get_event(); 
  void put_event(Event*); 
  void rm_event(int pid); 
  int get_next_event_time();
  void print_current_eventlist();

private:
  list<Event*> eventQueue;
};

#endif
