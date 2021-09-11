#include "Des.h"
#include "Event.h"

#include <stdio.h>
#include <cstddef>
#include <iostream>
#include <list>

bool dotrace_event_des = false;
#define trace2(fmt...)         \
    do                         \
    {                          \
        if (dotrace_event_des) \
        {                      \
            printf(fmt);       \
            fflush(stdout);    \
        }                      \
    } while (0)

char const *EnumTransitionDesTypes[] =
    {
        "READY",
        "RUNNG",
        "BLOCK",
        "PREEMPT",
        "DONE",
};

Des::Des(bool dotrace)
{
    dotrace_event_des = dotrace;
}
Event *Des::get_event()
{
    Event *e = eventQueue.front();
    eventQueue.pop_front();
    return e;
}

void Des::put_event(Event *e)
{
    trace2("  AddEvent(%d:%d:%s):", e->timestamp, e->process->get_pid(), EnumTransitionDesTypes[e->transition]);
    print_current_eventlist();
    std::list<Event *>::iterator it;
    if (eventQueue.empty())
    {
        eventQueue.push_back(e);
    }
    else
    {
        it = eventQueue.begin();
        while (*it != nullptr)
        {
            if ((*it)->timestamp > e->timestamp)
            {
                eventQueue.insert(it, e);
                break;
            }
            else if ((*it)->timestamp == e->timestamp)
            {
                if (e->event_creation_time < (*it)->event_creation_time)
                {
                    eventQueue.insert(it, e);
                    break;
                }
                else
                {
                    ++it;
                    if (*it == nullptr)
                    {
                        eventQueue.push_back(e);
                        break;
                    }
                }
            }
            else
            {
                ++it;
                if (*it == nullptr)
                {
                    eventQueue.push_back(e);
                    break;
                }
            }
        }
    }

    trace2(" ==> ");
    print_current_eventlist();
    trace2("\n");
}

int Des::get_next_event_time()
{
    Event *e = eventQueue.front();
    if (e == NULL)
    {
        return -1;
    }
    else
    {
        return e->timestamp;
    }
}

void Des::rm_event(int _pid)
{
    // eventQueue.pop_back();
    trace2("RemoveEvent(%d):",_pid);
    print_current_eventlist();
    std::list<Event *>::iterator it;
    for (it = eventQueue.begin(); it != eventQueue.end(); it++)
    {
        if ((*it) != NULL)
        {
            if ((*it)->process->get_pid() == _pid)
            {
                eventQueue.erase(it);
                break;
            }
        }
    }
    trace2(" ==>");
    print_current_eventlist();
    trace2("\n");
    
}

void Des::print_current_eventlist()
{

    std::list<Event *>::iterator it;
    for (it = eventQueue.begin(); it != eventQueue.end(); it++)
    {
        trace2("  %d:%d:%s", (*it)->timestamp, (*it)->process->get_pid(), EnumTransitionDesTypes[(*it)->transition]);
    }
}
