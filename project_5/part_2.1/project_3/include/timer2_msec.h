class Timer2_msec
{
public:
    Timer2_msec();
    void init(int period_microsec);
    void init(int period_microsec, float duty_cycle);
    void set(float duty_cycle);
};