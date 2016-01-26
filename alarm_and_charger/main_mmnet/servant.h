/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * Function prototypes that have no own place. Mostly init.
 *
**/

#include "defs.h"
#include <inttypes.h>


// -----------------------------------------------------------------------
// Debug
// -----------------------------------------------------------------------

#if DEBUG
#  define DPUTS(___s) puts(___s)
#else
#  define DPUTS(___s) 
#endif

// -----------------------------------------------------------------------
// Service threads
// -----------------------------------------------------------------------


#include <sys/thread.h>

THREAD(ModbusService, arg);

// -----------------------------------------------------------------------
// All the CGI entries.
// -----------------------------------------------------------------------

#include <pro/httpd.h>

int ShowThreads(FILE * stream, REQUEST * req);
int ShowTimers(FILE * stream, REQUEST * req);
int ShowSockets(FILE * stream, REQUEST * req);
int ShowForm(FILE * stream, REQUEST * req);


int CgiNetIO( FILE * stream, REQUEST * req );
int CgiInputs( FILE * stream, REQUEST * req );
int CgiOutputs( FILE * stream, REQUEST * req );
int CgiNetwork( FILE * stream, REQUEST * req );
int CgiStatus( FILE * stream, REQUEST * req );



// -----------------------------------------------------------------------
// Global non-specific prototypes
// -----------------------------------------------------------------------


void lua_init(void);

void init_tunnels(void);
void get_tunnel_stats( uint8_t nTunnel, uint32_t *tx_total, uint32_t *rx_total, uint8_t *active );

void notice_activity(void); // Called by network code to mark that something uses us as IO. If not - we will light fail LED.

extern uint8_t dht11_errorCnt;


