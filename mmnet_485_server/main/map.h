
struct minor
{
    uint8_t     number;
    char *	name;
    char *      (*to_string)(void);
};


struct iomap
{
    void        (*init)(struct iomap *dev);     // Called on system init
    void        (*start)(struct iomap *dev);    // Called after all inits if dev is enabled
    void        (*stop)(struct iomap *dev);    	// Called if dev is disabled runtime
    void        (*timer)(struct iomap *dev);    // Called once a second

    uint16_t    minor_count; // Number of sub-devices found
    struct minor *subdevices;

    uint16_t    io_count;
    uint16_t    err_count;

};

typedef struct iomap iomap;

