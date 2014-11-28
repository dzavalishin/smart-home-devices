
#define TOHOST_PONG 0x00
#define TOHOST_ACK 0x01
#define TOHOST_NAK 0x02

#define TOHOST_SERIAL 0x03 // Sent as long packet
#define TOHOST_DEVTYPE 0x04 // Sent as long packet

#define TOHOST_LONG_PACKET 0x0F

#define TOHOST_DIGITAL_VALUE     0x10
#define TOHOST_ANALOG_VALUE     0x11
#define TOHOST_DIGITAL_OUT_VALUE     0x12
#define TOHOST_ANALOG_OUT_VALUE     0x13
#define TOHOST_FREQ_VALUE       0x14
#define TOHOST_TEMPERATURE_VALUE 0x15




#define FROMHOST_PING 0x00

#define FROMHOST_DIGITAL_REQUEST 0x10
#define FROMHOST_ANALOG_REQUEST 0x11

// TODO: implement
#define FROMHOST_DIGITAL_OUT_REQUEST 0x12
#define FROMHOST_ANALOG_OUT_REQUEST 0x13

#define FROMHOST_FREQ_VALUE_REQUEST 0x14

// TODO: implement
#define FROMHOST_TEMPERATURE_VALUE_REQUEST 0x15

#define FROMHOST_DIGITAL_OUT 0x20
#define FROMHOST_ANALOG_OUT 0x21

#define FROMHOST_DIGITAL_OUT_ENABLE 0x30
#define FROMHOST_ANALOG_OUT_ENABLE 0x31

#define FROMHOST_DIGITAL_WARN_MASK 0x40
#define FROMHOST_ANALOG_WARN_LOW 0x41
#define FROMHOST_DIGITAL_WARN_VALUE 0x42
#define FROMHOST_ANALOG_WARN_HIGH 0x43

#define FROMHOST_DIGITAL_CRIT_MASK 0x40
#define FROMHOST_ANALOG_CRIT_LOW 0x41
#define FROMHOST_DIGITAL_CRIT_VALUE 0x42
#define FROMHOST_ANALOG_CRIT_HIGH 0x43


// Broadcasts/node number setup packets

// These 3 packets are used to tell node which node number it must use
// Each packet contains partial serial number (corresponding 2 bytes in
// value field) and desired node number in port field.

#define FROMHOST_BROADCAST_SETNODE_A 0x81
#define FROMHOST_BROADCAST_SETNODE_B 0x82
#define FROMHOST_BROADCAST_SETNODE_C 0x83

// This broadcast asks all nodes that have no node number yet
// to reply with their serial numbers (TOHOST_SERIAL packets).
// Host shall repeat this request from time to time to discover
// new devices.

#define FROMHOST_BROADCAST_REQUEST_NONSET_NODES 0x83











