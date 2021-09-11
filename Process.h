#ifndef PROCESS
#define PROCESS

using namespace std;

class Process
{
public:
    Process(int _at, int _tc, int _cb, int _io, int _pid, int _state_ts, int _remain_time, int _prio);
    int get_pid();
    int get_at();
    int get_tc();
    int get_cb();
    int get_io();
    int get_ft();
    int get_tt();
    int get_it();
    int get_prio();
    int get_dynamic_prio();

    int get_cw();

    void set_ft(int _ft);
    void set_tt(int _tt);
    void set_it(int _it);
    void set_prio(int _prio);
    void set_dynamic_prio(int _dynamic_prio);

    void set_cw(int _cw);
    int state_ts;
    int remain_time;
    int current_cpu_burst_remaining;
    int cpu_running_time;

private:
    int pid;
    int at;
    int tc;
    int cb;
    int io;
    int ft;
    int tt;
    int it;
    int prio;
    int cw;
    int dynamic_prio;
};

#endif