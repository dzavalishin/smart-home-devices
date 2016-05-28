// logged events


#define LOG_ID_RESTART      	0

#define LOG_ID_NO_PRESSURE      1
#define LOG_ID_LONG_RUN         2
#define LOG_ID_RETRY            3





void log_event( char log_id );

void lcd_put_time( long time_sec );
void lcd_put_event( int pos );

