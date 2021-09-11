#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <deque>
#include <cstddef>
#include <set>

#include "Process.h"
#include "Event.h"
#include "Des.h"

typedef long long LL;

using namespace std;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------Professor Stuff----------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------START------------------------------------------------------------------------------//

char const *EnumStateTypes[] =
    {
        "CREATED",
        "RUNNG",
        "READY",
        "BLOCK",
        "DONE",
};
char const *EnumTransitionTypes[] =
    {
        "READY",
        "RUNNNG",
        "BLOCK",
        "PREEMPT",
        "DONE",
};

char const *Algorithm[] =
    {
        "garbage",
        "FCFS",
        "LCFS",
        "SRTF",
        "RR",
        "PRIO",
        "PREPRIO",
};

#define MAX_OBJS (10) // always put brackets around expressions

int global_pid = 0;
bool verbose = false;
bool dotrace_scheduler = false;
bool dotrace_event = false;

// really convenient way to implement a global conditional trace function
// note the do { } while .. this is important if you do if (cond) trace("yoo") else { something }

#define trace(fmt...)       \
    do                      \
    {                       \
        if (verbose)        \
        {                   \
            printf(fmt);    \
            fflush(stdout); \
        }                   \
    } while (0)
//#define trace(fmt...)      do { if (0) { printf(fmt); fflush(stdout); } } while(0)
//#define trace(fmt...)      do { } while(0)

int fct(int a)
{
    if (a == 0)
        trace("haha");
    else
        printf("no");
}

// this is the base class of objects
class GObj
{
    static int counter; // static class counter telling me how many objects were created
    static int deleted; // static class counter telling me how many objects were deleted
protected:
    int id; // unique ID for this object
public:
    GObj() { id = counter++; }
    ~GObj() { deleted++; }

    //   virtual
    //   void display() { trace("obj<%d>\n", ((GObj*)NULL)->id); }
    void display() { trace("[%-40s]: <%d>\n", __PRETTY_FUNCTION__, ((GObj *)this)->id); }
    virtual void fct() { trace("[%-40s]:\n", __PRETTY_FUNCTION__); }
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------Professor Stuff----------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------END------------------------------------------------------------------------------//

int max_rand_size;
LL *randvals;
int ofs = 0;
int myrandom(int burst)
{
    if (ofs > max_rand_size)
    {
        ofs = ofs % max_rand_size;
    }
    int val = randvals[ofs];
    ofs++;
    return 1 + (val % burst);
}

bool compare_process_id(Process *first, Process *second)
{

    return (first->get_pid() < second->get_pid());
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------Scheduler Algorithm------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------START------------------------------------------------------------------------------//

class Scheduler
{
public:
    Scheduler(){};

    virtual Process *get_from_queue() = 0;
    virtual void add_to_queue(Process *p) = 0;
    virtual void print_readyQueue() = 0;
    virtual int get_quantum() = 0;
    virtual bool test_preempt() = 0;
};

class FCFS : public Scheduler
{
public:
    FCFS(int quant = 0)
    {
        quantum = quant;
    }
    void add_to_queue(Process *p)
    {
        ready_queue.push_back(p);
    };
    Process *get_from_queue()
    {
        if (ready_queue.size() != 0)
        {
            Process *f = ready_queue.front();

            ready_queue.pop_front();

            return f;
        }
        else
        {
            return nullptr;
        }
    };
    void print_readyQueue()
    {
        trace("SCHED (%d): ", ready_queue.size());

        std::list<Process *>::iterator it;

        for (it = ready_queue.begin(); it != ready_queue.end(); it++)
        {
            trace(" %d:%d", (*it)->get_pid(), (*it)->state_ts);
        }
        trace("\n");
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return false;
    }

private:
    list<Process *> ready_queue;
    int quantum;
};

class LCFS : public Scheduler
{
public:
    LCFS(int quant = 0)
    {
        quantum = quant;
    }
    void add_to_queue(Process *p)
    {
        ready_queue.push_front(p);
    };
    Process *get_from_queue()
    {
        if (ready_queue.size() != 0)
        {
            Process *f = ready_queue.front();

            ready_queue.pop_front();

            return f;
        }
        else
        {
            return nullptr;
        }
    };
    void print_readyQueue()
    {
        trace("SCHED (%d): ", ready_queue.size());

        std::list<Process *>::iterator it;

        for (it = ready_queue.begin(); it != ready_queue.end(); it++)
        {
            trace(" %d:%d", (*it)->get_pid(), (*it)->state_ts);
        }
        trace("\n");
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return false;
    }

private:
    list<Process *> ready_queue;
    int quantum;
};

class SRTF : public Scheduler
{
public:
    SRTF(int quant = 0)
    {
        quantum = quant;
    }
    void add_to_queue(Process *p)
    {
        std::list<Process *>::iterator it;
        it = ready_queue.begin();
        while (true)
        {
            // trace(" Schedule ready queue %d \n", (*it)->get_cb());
            if (it == ready_queue.end())
            {
                ready_queue.push_back(p);
                break;
            }
            // trace(" %d - %d \n", (*it)->remain_time , p->remain_time);
            if ((*it)->remain_time > p->remain_time)
            {
                ready_queue.insert(it, p);
                break;
            }
            it++;
        }
    };
    Process *get_from_queue()
    {
        if (ready_queue.size() != 0)
        {
            Process *f = ready_queue.front();

            ready_queue.pop_front();

            return f;
        }
        else
        {
            return nullptr;
        }
    };
    void print_readyQueue()
    {
        if (dotrace_scheduler)
        {
            trace("SCHED (%d): ", ready_queue.size());

            std::list<Process *>::iterator it;

            for (it = ready_queue.begin(); it != ready_queue.end(); it++)
            {
                trace(" %d:%d", (*it)->get_pid(), (*it)->state_ts);
            }
            trace("\n");
        }
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return false;
    }

private:
    list<Process *> ready_queue;
    int quantum;
};

class RR : public Scheduler
{
public:
    RR(int quant = 0)
    {
        quantum = quant;
    }
    void add_to_queue(Process *p)
    {
        p->set_dynamic_prio(p->get_prio() - 1);
        ready_queue.push_back(p);
    };
    Process *get_from_queue()
    {
        if (ready_queue.size() != 0)
        {
            Process *f = ready_queue.front();

            ready_queue.pop_front();

            return f;
        }
        else
        {
            return nullptr;
        }
    };
    void print_readyQueue()
    {
        if (dotrace_scheduler)
        {
            trace("SCHED (%d): ", ready_queue.size());

            std::list<Process *>::iterator it;

            for (it = ready_queue.begin(); it != ready_queue.end(); it++)
            {
                trace(" %d:%d", (*it)->get_pid(), (*it)->state_ts);
            }
            trace("\n");
        }
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return false;
    }

private:
    list<Process *> ready_queue;
    int quantum;
};

class PRIO : public Scheduler
{
public:
    PRIO(int quant = 0, int _maxprios = 4)
    {
        quantum = quant;
        maxprios = _maxprios;
        // activeQ =
        // (deque<Process *> *)calloc(_maxprios, sizeof(deque<Process *>));
        // expiredQ =
        // (deque<Process *> *)calloc(_maxprios, sizeof(deque<Process *>));

        activeQ = new vector<deque<Process *> >(_maxprios);
        expiredQ = new vector<deque<Process *> >(_maxprios);
    }
    void add_to_queue(Process *p)
    {
        if (p->get_dynamic_prio() < 0)
        {

            p->set_dynamic_prio(p->get_prio() - 1);
            expiredQ->at(p->get_dynamic_prio()).push_back(p);
        }
        else
        {
            activeQ->at(p->get_dynamic_prio()).push_back(p);
        }
        // ready_queue.push_back(p);
    };
    Process *get_from_queue()
    {
        bool is_empty = true;
        for (int i = maxprios - 1; i >= 0; i--)
        {

            if (!(*activeQ)[i].empty())
            {

                is_empty = false;
                swapActive = false;
                Process *f = (*activeQ)[i].front();
                (*activeQ)[i].pop_front();
                return f;
            }
        }
        if (is_empty)
        {
            if (swapActive)
            {
                return nullptr;
            }
            else
            {
                swapActive = true;
                vector<deque<Process *> > *temp = activeQ;
                activeQ = expiredQ;
                expiredQ = temp;
                trace("switched queues\n");
                get_from_queue();
            }
        }
    };
    void print_readyQueue()
    {

        if (dotrace_scheduler)
        {

            trace("{ ");
            for (int i = maxprios - 1; i >= 0; i--)
            {
                trace("[");

                if (!(*activeQ)[i].empty())
                {

                    for (auto it = (*activeQ)[i].cbegin(); it != (*activeQ)[i].cend(); ++it)
                    {
                        trace("%d,", (*it)->get_pid());
                    }
                }
                trace("]");
            }
            trace("} : ");
            trace("{ ");
            for (int i = maxprios - 1; i >= 0; i--)
            {
                trace("[");
                if (!(*expiredQ)[i].empty())
                {

                    for (auto it = (*expiredQ)[i].cbegin(); it != (*expiredQ)[i].cend(); ++it)
                    {
                        trace("%d,", (*it)->get_pid());
                    }
                }
                trace("]");
            }
            trace("} : \n");
        }
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return false;
    }

private:
    vector<deque<Process *> > *activeQ;
    vector<deque<Process *> > *expiredQ;
    int quantum;
    int maxprios;
    bool swapActive = false;
};

class PREPRIO : public Scheduler
{
public:
    PREPRIO(int quant = 0, int _maxprios = 4)
    {
        quantum = quant;
        maxprios = _maxprios;
        // activeQ =
        // (deque<Process *> *)calloc(_maxprios, sizeof(deque<Process *>));
        // expiredQ =
        // (deque<Process *> *)calloc(_maxprios, sizeof(deque<Process *>));

        activeQ = new vector<deque<Process *> >(_maxprios);
        expiredQ = new vector<deque<Process *> >(_maxprios);
    }
    void add_to_queue(Process *p)
    {
        if (p->get_dynamic_prio() < 0)
        {

            p->set_dynamic_prio(p->get_prio() - 1);
            expiredQ->at(p->get_dynamic_prio()).push_back(p);
        }
        else
        {
            activeQ->at(p->get_dynamic_prio()).push_back(p);
        }
        // ready_queue.push_back(p);
    };
    Process *get_from_queue()
    {
        bool is_empty = true;
        for (int i = maxprios - 1; i >= 0; i--)
        {

            if (!(*activeQ)[i].empty())
            {

                is_empty = false;
                swapActive = false;
                Process *f = (*activeQ)[i].front();
                (*activeQ)[i].pop_front();
                return f;
            }
        }
        if (is_empty)
        {
            if (swapActive)
            {
                return nullptr;
            }
            else
            {
                swapActive = true;
                vector<deque<Process *> > *temp = activeQ;
                activeQ = expiredQ;
                expiredQ = temp;
                trace("switched queues\n");
                get_from_queue();
            }
        }
    };
    void print_readyQueue()
    {

        if (dotrace_scheduler)
        {

            trace("{ ");
            for (int i = maxprios - 1; i >= 0; i--)
            {
                trace("[");

                if (!(*activeQ)[i].empty())
                {

                    for (auto it = (*activeQ)[i].cbegin(); it != (*activeQ)[i].cend(); ++it)
                    {
                        trace("%d,", (*it)->get_pid());
                    }
                }
                trace("]");
            }
            trace("} : ");
            trace("{ ");
            for (int i = maxprios - 1; i >= 0; i--)
            {
                trace("[");
                if (!(*expiredQ)[i].empty())
                {

                    for (auto it = (*expiredQ)[i].cbegin(); it != (*expiredQ)[i].cend(); ++it)
                    {
                        trace("%d,", (*it)->get_pid());
                    }
                }
                trace("]");
            }
            trace("} : \n");
        }
    }

    int get_quantum()
    {
        return quantum;
    }
    bool test_preempt()
    {
        return true;
    }

private:
    vector<deque<Process *> > *activeQ;
    vector<deque<Process *> > *expiredQ;
    int quantum;
    int maxprios;
    bool swapActive = false;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------Scheduler Algorithm------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------END--------------------------------------------------------------------------------//

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------SIMULATOR----------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------START------------------------------------------------------------------------------//
int io_free;
list<Process *> Simulation(Des *des, Scheduler *sched)
{
    list<Process *> finalOutput;
    Event *evt;
    int CURRENT_TIME;
    bool CALL_SCHEDULER = false;
    int timeInPrevState;
    Process *CURRENT_RUNNING_PROCESS = nullptr;
    int current_io_burst;
    int current_cb_burst;
    set<int> proc_in_IO;
    int time_in_io_begin = 0;
    int time_in_io = 0;
    int io_flag = -1;

    while ((evt = des->get_event()))
    {
        Process *proc = evt->process; // this is the process the event works on
        CURRENT_TIME = evt->timestamp;
        timeInPrevState = CURRENT_TIME - proc->state_ts;
        Process *latest_proc;
        switch (evt->transition)
        { // which state to transition to?
        case Event::TRANS_TO_READY:
            // must come from BLOCKED or from PREEMPTION
            // must add to run queue

            proc_in_IO.erase(proc->get_pid());
            if (proc_in_IO.empty() && (evt->oldstate == Event::BLOCKED))
            {
                io_flag = -1;
                time_in_io = time_in_io + (CURRENT_TIME - time_in_io_begin);
            }
            if (evt->oldstate == Event::BLOCKED)
            {
                proc->set_dynamic_prio(proc->get_prio() - 1);
            }
            trace("%d %d %d: %s -> %s\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate]);
            sched->add_to_queue(proc);
            proc->state_ts = CURRENT_TIME;
            if (sched->test_preempt())
            {
                if (CURRENT_RUNNING_PROCESS != nullptr && proc->get_dynamic_prio() > CURRENT_RUNNING_PROCESS->get_dynamic_prio() && (((CURRENT_RUNNING_PROCESS->state_ts+ CURRENT_RUNNING_PROCESS->cpu_running_time)-CURRENT_TIME) > 0))
                {
                    trace("---> PRIO preemption %d by %d ? %d TS=%d now=%d) --> YES\n", CURRENT_RUNNING_PROCESS->get_pid(),proc->get_pid(),(CURRENT_TIME - CURRENT_RUNNING_PROCESS->state_ts), (CURRENT_RUNNING_PROCESS->state_ts+ CURRENT_RUNNING_PROCESS->cpu_running_time),CURRENT_TIME );
                    int current_running_time = CURRENT_TIME - CURRENT_RUNNING_PROCESS->state_ts;
                    des->rm_event(CURRENT_RUNNING_PROCESS->get_pid());
                    // CURRENT_RUNNING_PROCESS->state_ts = CURRENT_TIME;
                    // trace("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], current_cb_burst, CURRENT_RUNNING_PROCESS->remain_time, CURRENT_RUNNING_PROCESS->get_dynamic_prio());
                    
                    // printf("kskks %d , %d , %d  %d \n",current_running_time,CURRENT_RUNNING_PROCESS->remain_time,CURRENT_RUNNING_PROCESS->current_cpu_burst_remaining,CURRENT_RUNNING_PROCESS->cpu_running_time);

                    
                    CURRENT_RUNNING_PROCESS->remain_time = CURRENT_RUNNING_PROCESS->remain_time - current_running_time+CURRENT_RUNNING_PROCESS->cpu_running_time;
                    CURRENT_RUNNING_PROCESS->current_cpu_burst_remaining = CURRENT_RUNNING_PROCESS->current_cpu_burst_remaining - current_running_time+CURRENT_RUNNING_PROCESS->cpu_running_time;

                    des->put_event(new Event(CURRENT_TIME, CURRENT_RUNNING_PROCESS, Event::RUNNING, Event::READY, Event::TRANS_TO_PREEMPT, CURRENT_TIME));
                    CURRENT_RUNNING_PROCESS = nullptr;
                }
            }
            CALL_SCHEDULER = true; // conditional on whether something is run
            break;
        case Event::TRANS_TO_RUN:
            // create event for either preemption or blocking
            if (proc->current_cpu_burst_remaining == 0)
            {
                current_cb_burst = myrandom(proc->get_cb());
            }
            else
            {
                current_cb_burst = proc->current_cpu_burst_remaining;
            }
            if (evt->oldstate == Event::READY)
            {
                proc->set_cw(proc->get_cw() + timeInPrevState);
            }
            if (current_cb_burst >= proc->remain_time)
            {
                current_cb_burst = proc->remain_time;
            }
            if (current_cb_burst <= sched->get_quantum())
            {
                if (current_cb_burst >= proc->remain_time)
                {
                    current_cb_burst = proc->remain_time;
                    proc->state_ts = CURRENT_TIME;
                    trace("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], current_cb_burst, proc->remain_time, proc->get_dynamic_prio());
                    proc->remain_time = 0;
                    proc->current_cpu_burst_remaining = 0;
                    proc->cpu_running_time = current_cb_burst;
                    des->put_event(new Event(CURRENT_TIME + current_cb_burst, proc, Event::RUNNING, Event::BLOCKED, Event::TRANS_TO_BLOCK, CURRENT_TIME));
                }
                else
                {
                    proc->state_ts = CURRENT_TIME;
                    trace("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], current_cb_burst, proc->remain_time, proc->get_dynamic_prio());
                    proc->remain_time = proc->remain_time - current_cb_burst;
                    proc->current_cpu_burst_remaining = 0;
                    proc->cpu_running_time = current_cb_burst;
                    des->put_event(new Event(CURRENT_TIME + current_cb_burst, proc, Event::RUNNING, Event::BLOCKED, Event::TRANS_TO_BLOCK, CURRENT_TIME));
                }
            }
            else
            {
                proc->state_ts = CURRENT_TIME;
                trace("%d %d %d: %s -> %s cb=%d rem=%d prio=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], current_cb_burst, proc->remain_time, proc->get_dynamic_prio());
                proc->remain_time = proc->remain_time - sched->get_quantum();
                proc->current_cpu_burst_remaining = current_cb_burst - sched->get_quantum();
                proc->cpu_running_time = sched->get_quantum();
                des->put_event(new Event(CURRENT_TIME + sched->get_quantum(), proc, Event::RUNNING, Event::READY, Event::TRANS_TO_PREEMPT, CURRENT_TIME));
            }
            break;
        case Event::TRANS_TO_BLOCK:
            //create an event for when process becomes READY again

            if (proc->remain_time == 0)
            {

                trace("%d %d %d: Done\n", evt->timestamp, evt->process->get_pid(), timeInPrevState);
                proc->set_ft(CURRENT_TIME);
                proc->set_tt(proc->get_ft() - proc->get_at());
                finalOutput.push_back(proc);
                CALL_SCHEDULER = true;
                CURRENT_RUNNING_PROCESS = nullptr;
                break;
            }
            else
            {
                proc_in_IO.insert(proc->get_pid());
                if (io_flag == -1)
                {
                    io_flag = 1;
                    time_in_io_begin = CURRENT_TIME;
                }
                current_io_burst = myrandom(proc->get_io());
                trace("%d %d %d: %s -> %s  ib=%d rem=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], current_io_burst, proc->remain_time);

                proc->set_it(proc->get_it() + current_io_burst);
                proc->state_ts = CURRENT_TIME;

                des->put_event(new Event(CURRENT_TIME + current_io_burst, proc, Event::BLOCKED, Event::READY, Event::TRANS_TO_READY, CURRENT_TIME));

                CALL_SCHEDULER = true;
                CURRENT_RUNNING_PROCESS = nullptr;
                break;
            }
        case Event::TRANS_TO_PREEMPT:
            // add to runqueue (no event is generated)
            trace("%d %d %d: %s -> %s  cb=%d rem=%d prio=%d\n", evt->timestamp, evt->process->get_pid(), timeInPrevState, EnumStateTypes[evt->oldstate], EnumStateTypes[evt->newstate], proc->current_cpu_burst_remaining, proc->remain_time, proc->get_dynamic_prio());
            proc->set_dynamic_prio(proc->get_dynamic_prio() - 1);
            sched->add_to_queue(proc);
            proc->state_ts = CURRENT_TIME;
            CURRENT_RUNNING_PROCESS = nullptr;
            CALL_SCHEDULER = true;
            break;

        case Event::TRANS_TO_DONE:
            trace("done");
            latest_proc = sched->get_from_queue();
            proc->set_ft(CURRENT_TIME);
            proc->set_tt(proc->get_ft() - proc->get_at());
            finalOutput.push_back(proc);
            CALL_SCHEDULER = true;
            break;
        }
        // remove current event object from Memory
        delete evt;
        evt = nullptr;
        if (CALL_SCHEDULER)
        {
            // trace("[%-40s]: next event time  <%d> current time <%d>\n", __PRETTY_FUNCTION__,des->get_next_event_time(),CURRENT_TIME);
            if (des->get_next_event_time() == CURRENT_TIME)
                continue;           //process next event from Event queue
            CALL_SCHEDULER = false; // reset global flag
            if ((CURRENT_RUNNING_PROCESS) == nullptr)
            {
                sched->print_readyQueue();

                CURRENT_RUNNING_PROCESS = sched->get_from_queue();

                if (CURRENT_RUNNING_PROCESS == nullptr)
                {
                    continue;
                }
                else
                {
                    des->put_event(new Event(CURRENT_TIME, CURRENT_RUNNING_PROCESS, Event::READY, Event::RUNNING, Event::TRANS_TO_RUN, CURRENT_TIME));
                }
            }
        }
    }
    io_free = time_in_io;

    return finalOutput;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ---------------------------------------------------------------------------------SIMULATOR----------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------END--------------------------------------------------------------------------------//

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------Main------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------Start------------------------------------------------------------------------------//

int main(int argc, char **argv)
{
    int c;
    int count = 1;
    int segv = 0;
    int schedtype = 1;
    char *buffer;
    char *buffer2;
    size_t linecap = 0;
    char const *sep = " \n\t";
    char *word;
    int global_pid = 0;
    int given_quantum = -1;
    int max_prio = 4;

    // proper way to parse arguments
    while ((c = getopt(argc, argv, ":vVtes:")) != -1)
    {
        switch (c)
        {
        case 'v':
            verbose = true;
            break;
        case 's':
            segv = 1;
            if ((optarg[0]) == 'F')
            {
                schedtype = 1;
            }
            else if ((optarg[0]) == 'L')
            {
                schedtype = 2;
            }
            else if ((optarg[0]) == 'S')
            {
                schedtype = 3;
            }
            else if ((optarg[0]) == 'R')
            {
                word = strtok((optarg + 1), " \n\t");
                given_quantum = atoi(word);
                schedtype = 4;
            }
            else if ((optarg[0]) == 'P')
            {
                word = strtok((optarg + 1), " :\n\t");
                given_quantum = atoi(word);
                word = strtok(NULL, " \n\t");
                if (word != NULL)
                {
                    max_prio = atoi(word);
                }
                schedtype = 5;
            }
            else if ((optarg[0]) == 'E')
            {
                word = strtok((optarg + 1), " :\n\t");
                given_quantum = atoi(word);
                word = strtok(NULL, " \n\t");
                if (word != NULL)
                {
                    max_prio = atoi(word);
                }
                schedtype = 6;
            }
            else
            {
                printf("Unknown Scheduler spec: -v {FLSRPE}\n");
                exit(1);
            }
            break;
        case 'V':
            verbose = true;
            break;
        case 't':
            dotrace_scheduler = true;
            break;
        case 'e':
            dotrace_event = true;
            break;
        }
    }
    Scheduler *b1;
    switch (schedtype)
    {
    case 1:
    {
        b1 = new FCFS(10000);
    }
    break;
    case 2:
    {
        b1 = new LCFS(10000);
    }
    break;
    case 3:
    {
        b1 = new SRTF(10000);
    }
    break;
    case 4:
    {
        b1 = new RR(given_quantum);
    }
    break;
    case 5:
    {
        b1 = new PRIO(given_quantum, max_prio);
    }
    break;
    case 6:
    {
        b1 = new PREPRIO(given_quantum, max_prio);
    }
    break;
    default:
        printf("At least specify a valid scheduler\n");
        exit(1);
    }
    if ((argc - optind) < 2)
    {
        printf("Now, I told you I need two fixed arguments, what the heck are you asking me to do?\n");
        exit(1);
    }
    // note these additional arguments do nothing
    // printf("%d <%s> <%s>\n", argc, argv[optind], argv[optind + 1]);

    //----------------------------------------------------------------Create Random value array------------------------------------------------------------------------//

    FILE *randfile = fopen(argv[optind + 1], "r");
    if (randfile == NULL)
    {
        cout << "Not a valid inputfile <" << argv[optind + 1] << ">\n";
        exit(1);
    }
    else
    {
        getline(&buffer, &linecap, randfile);
        max_rand_size = atoi(buffer);
        LL randomValues[max_rand_size];
        LL i = 0;
        while (getline(&buffer, &linecap, randfile) != -1)
        {
            randomValues[i] = atoi(buffer);
            // printf(" %d -  %d \n",i ,randomValues[i]);
            i++;
        }
        randvals = randomValues;
    }

    // cout<<randvals[0]<<"\n";

    //----------------------------------------------------------------Create Des object------------------------------------------------------------------------//

    Des *scheduler_des = new Des(dotrace_event && verbose);

    //----------------------------------------------------------------Read Input file------------------------------------------------------------------------//

    FILE *myfile = fopen(argv[optind], "r");
    if (myfile == NULL)
    {
        cout << "Not a valid inputfile <" << argv[optind] << ">\n";
        exit(1);
    }
    else
    {
        while (getline(&buffer, &linecap, myfile) != -1)
        {
            word = strtok(buffer, sep);
            int arrival = atoi(word);
            word = strtok(NULL, sep);
            int totalCPU = atoi(word);
            word = strtok(NULL, sep);
            int CPUBurst = atoi(word);
            word = strtok(NULL, sep);
            int IOBurst = atoi(word);

            Process *proc = new Process(arrival, totalCPU, CPUBurst, IOBurst, global_pid, arrival, totalCPU, myrandom(max_prio));
            global_pid++;

            Event *eve = new Event(arrival, proc, Event::CREATE, Event::READY, Event::TRANS_TO_READY, 0);
            scheduler_des->put_event(eve);
        }

        // printf("ye jo des hai mera %d\n", scheduler_des->get_event()->process->get_io());
        // printf("ye jo des hai mera %d\n", scheduler_des->get_event()->process->get_io());
        // printf("ye jo des hai mera %d\n", scheduler_des->get_event()->process->get_io());
    }
    //----------------------------------------------------------------Read Input file------------------------------------------------------------------------//
    //----------------------------------------------------------------Run Simulator ------------------------------------------------------------------------//

    list<Process *> l1 = Simulation(scheduler_des, b1);
    // cout << l1.front()->get_pid() << "\n";

    //----------------------------------------------------------------Run Simulator -------------------------------------------------------------------------//

    //----------------------------------------------------------------Print Output-------------------------------------------------------------------------//

    l1.sort(compare_process_id);
    std::list<Process *>::iterator it;
    int total_ft = 0;
    int total_tt = 0;
    int total_it = 0;
    int total_cw = 0;
    int total_cputime = 0;

    int num_processes = 0;
    printf("%s", Algorithm[schedtype]);
    if (given_quantum != -1)
    {
        printf(" %d\n", given_quantum);
    }
    else
    {
        printf("\n");
    }
    for (it = l1.begin(); it != l1.end(); it++)
    {
        printf("%04d: %4d %4d %4d %4d %d |  %4d  %4d  %4d  %4d\n", (*it)->get_pid(), (*it)->get_at(), (*it)->get_tc(), (*it)->get_cb(), (*it)->get_io(), (*it)->get_prio(), (*it)->get_ft(), (*it)->get_tt(), (*it)->get_it(), (*it)->get_cw());
        if (total_ft < (*it)->get_ft())
        {
            total_ft = (*it)->get_ft();
        }
        total_tt += (*it)->get_tt();
        total_it += (*it)->get_it();
        total_cw += (*it)->get_cw();
        total_cputime += (*it)->get_tc();
        num_processes++;
    }
    double cpu_util = 100.0 * (total_cputime / (double)total_ft);

    double io_util = 100.0 * (io_free / (double)total_ft);
    double throughput = 100.0 * (num_processes / (double)total_ft);
    double avg_tt = (total_tt / (double)num_processes);
    double avg_wt = (total_cw / (double)num_processes);

    printf("SUM: %d %.2f %.2f %.2f %.2f %.3f\n", total_ft, cpu_util, io_util, avg_tt, avg_wt, throughput);

    //----------------------------------------------------------------Print Output-------------------------------------------------------------------------//

    return 0;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------Main------------------------------------------------------------------------------//
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------//
// -----------------------------------------------------------------------------------END------------------------------------------------------------------------------//