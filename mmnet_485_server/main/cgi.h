//#include <io.h>
#include <pro/httpd.h>

#include "adc.h"

extern u_char mac[];

int CgiNetIO( FILE * stream, REQUEST * req );



// Returns 0 if nothing to show more
typedef int (*table_print_func_t)( FILE * stream, int row_no ) ;
//typedef int (*table_print_func_t)( int stream ) ;


int
ShowTableCgi(FILE * stream, REQUEST * req,
             prog_char *header, //prog_char *columns,
             table_print_func_t print_next );


// Showtable workers

//int CgiAnalogueInputsRow( FILE * stream, int row_no );
int CgiInputs( FILE * stream, REQUEST * req );
int CgiOutputs( FILE * stream, REQUEST * req );

int CgiNetwork( FILE * stream, REQUEST * req );

