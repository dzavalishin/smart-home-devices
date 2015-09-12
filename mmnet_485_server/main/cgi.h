//#include <io.h>
#include <pro/httpd.h>


int CgiNetIO( FILE * stream, REQUEST * req );



int CgiInputs( FILE * stream, REQUEST * req );
int CgiOutputs( FILE * stream, REQUEST * req );

int CgiNetwork( FILE * stream, REQUEST * req );

int CgiStatus( FILE * stream, REQUEST * req );

