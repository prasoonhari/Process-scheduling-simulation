#ifndef EVENT
#define EVENT

using namespace std;
#include <list>
#include "Process.h"


class Event
{
public:


    typedef enum
    {
        CREATE,
        RUNNING,
        READY,
        BLOCKED,
        DONE,
    } _ProcessState;

    typedef enum
    {
        TRANS_TO_READY,
        TRANS_TO_RUN,
        TRANS_TO_BLOCK,
        TRANS_TO_PREEMPT,
        TRANS_TO_DONE,
    } _transition;

    Event(int _timestamp, Process *_process, _ProcessState _oldstate, _ProcessState _newstate,_transition __transition,int _event_creation_time);
    int timestamp;
    Process *process;
    _ProcessState oldstate;
    _ProcessState newstate;
    _transition transition;
    int event_creation_time;
   
};

#endif