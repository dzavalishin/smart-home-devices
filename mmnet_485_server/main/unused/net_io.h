void send_pack(unsigned char send_type, unsigned char send_port, unsigned int send_data);
void send_long_packet(unsigned char ext_type, unsigned char long_len, unsigned char *long_data );

void send_protocol_replies(void);


void net_request_pong(void);

void init_udp_net_io(void);

// Not ours, but...

void triggerSendOut(void);

void fail_led(void); // debug


