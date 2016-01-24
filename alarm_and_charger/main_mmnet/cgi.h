//#include <io.h>
#include <pro/httpd.h>

//int ShowQuery(FILE * stream, REQUEST * req);
int ShowThreads(FILE * stream, REQUEST * req);
int ShowTimers(FILE * stream, REQUEST * req);
int ShowSockets(FILE * stream, REQUEST * req);
int ShowForm(FILE * stream, REQUEST * req);


int CgiNetIO( FILE * stream, REQUEST * req );
int CgiInputs( FILE * stream, REQUEST * req );
int CgiOutputs( FILE * stream, REQUEST * req );
int CgiNetwork( FILE * stream, REQUEST * req );
int CgiStatus( FILE * stream, REQUEST * req );

