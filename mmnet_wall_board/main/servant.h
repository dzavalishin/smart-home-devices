/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
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
// Global non-specific prototypes
// -----------------------------------------------------------------------


void lua_init(void);

void init_tunnels(void);
void get_tunnel_stats( uint8_t nTunnel, uint32_t *tx_total, uint32_t *rx_total, uint8_t *active );

void notice_activity(void); // Called by network code to mark that something uses us as IO. If not - we will light fail LED.

extern uint8_t dht11_errorCnt;


