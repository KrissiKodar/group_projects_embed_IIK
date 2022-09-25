class Timer0_msec
{
public:
    Timer0_msec();
    void init(int period_msec);
    void init(int period_msec, float duty_cycle);
    void set(float duty_cycle);
};