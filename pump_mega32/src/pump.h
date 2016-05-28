
// 2 R measure inputs for MM385 and 2 0-4.5 v inputs
#define N_SENSORS 4

struct sens_save
{
    char        active;

    // Linear conversion, maps in to out in a linear way

    int         convert_in_L;   // This input value
    int         convert_out_L;  // Maps to this output

    int         convert_in_H;
    int         convert_out_H;

    int         L_level;        // Turn pump on here
    int         H_level;        // Turn pump off here
};

struct sensor
{
    struct sens_save    conf;   // MUST BE FIRST

    char        adc_channel;    // Which ADC channel to read to gen sensor value

    int         in_value;       // Current value as read from ADC


    int         out_value;      // Current value converted (supposed to be atm*100)

    int         max;            // Max converted value we've seen - suppose that pump can't give more

    char        is_below;       // Pressure is below low mark
    char        is_above;       // Pressure is above high mark

};


extern struct sensor   sens[N_SENSORS];

extern long    uptime;


extern char    system_failed;
extern char    active_pump;
extern char    pump_state;


void pump_reset_all(void);


