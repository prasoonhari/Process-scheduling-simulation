#include"Process.h"


    Process::Process(int _at, int _tc, int _cb, int _io,int _pid,int _state_ts,int _remain_time, int _prio)
    {
        at = _at;
        tc = _tc;
        cb = _cb;
        io = _io;
        pid = _pid;
        it =0;
        cw =0;
        state_ts = _state_ts;
        remain_time =_remain_time;
        prio = _prio;
        dynamic_prio = _prio-1;
        current_cpu_burst_remaining = 0;
        cpu_running_time = 0;

    };
    int Process::get_pid() { return pid; };
    int Process::get_at() { return at; };
    int Process::get_tc() { return tc; };
    int Process::get_cb() { return cb; };
    int Process::get_io() { return io; };
    int Process::get_ft() { return ft; };
    int Process::get_tt() { return tt; };
    int Process::get_it() { return it; };
    int Process::get_prio() { return prio; };
    int Process::get_dynamic_prio() { return dynamic_prio; };
    int Process::get_cw() { return cw; };

    void Process::set_ft(int _ft) { ft = _ft; };
    void Process::set_tt(int _tt) { tt = _tt; };
    void Process::set_it(int _it) { it = _it; };
    void Process::set_prio(int _prio) { prio = _prio; };
    void Process::set_cw(int _cw) { cw = _cw; };
    void Process::set_dynamic_prio(int _dynamic_prio){dynamic_prio=_dynamic_prio;};

