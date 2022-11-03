class encoder
{
public:
    encoder();
    void init(bool enc_input1);
    void init_interrupt();
    int position(bool enc_input1, bool enc_input2); //class Digital_out&);
    int get_counter();
    void reset_counter();
private:
    int counter;
    bool last_input1;
};