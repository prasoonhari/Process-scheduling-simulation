#include"Event.h"


Event::Event(int _timestamp,Process *_process,_ProcessState _oldstate,_ProcessState _newstate,_transition __transition,int _event_creation_time){
    timestamp = _timestamp;
    process =_process;
    oldstate = _oldstate;
    newstate = _newstate;
    transition = __transition;
    event_creation_time=_event_creation_time;
};