#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/syslog.h>
#include <sys/time.h>

#ifdef TEST_SUITE
#define _tm tm
#endif

#define SYSLOG_MAXBUF 1024

char *syslog_buf;

static int syslog_fac = LOG_USER;
static int syslog_mask = 0xFF;

static int syslog_stat;
static size_t syslog_taglen;
static char *syslog_tag;

void syslog_flush(size_t len);


#define BUFADD(___l) do {     strlcpy(syslog_buf+rc, ___l, SYSLOG_MAXBUF-rc ); rc += strlen(syslog_buf+rc); } while(0)
#define SPACE() (syslog_buf[rc++] = ' ')
#define DASH() (syslog_buf[rc++] = '-')


/*!
 * \brief Assemble syslog header.
 *
 * For internal use only.
 *
 * \param pri Value of the syslog PRI part.
 *
 * \return Number of characters in the global syslog buffer.
 */
size_t syslog_header(int pri)
{
    size_t rc;

    /* Remove invalid bits. */
    pri &= LOG_PRIMASK | LOG_FACMASK;

    /* Check priority against setlog mask values. */
    if ((LOG_MASK(LOG_PRI(pri)) & syslog_mask) == 0) {
        return 0;
    }

    /* Set default facility if none specified. */
    if ((pri & LOG_FACMASK) == 0) {
        pri |= syslog_fac;
    }

    /* Open log if not done before. */
    if (syslog_buf == 0) {
        openlog(0, syslog_stat | LOG_NDELAY, syslog_fac);
    }

    if(syslog_buf == 0) return 0;

    /* PRI field.
    ** This is common to all syslog formats. */
    rc = sprintf(syslog_buf, "<%d>", pri);

    /* VERSION field.
    ** Note, that there is no space separator. */

    syslog_buf[rc++] = '1';


    /* TIMESTAMP field. */
    {
        time_t now;
        struct _tm *tip;

        time(&now);

        tip = gmtime(&now);
        rc += sprintf(syslog_buf+rc, " %04d-%02d-%02dT%02d:%02d:%02dZ",
            tip->tm_year + 1900, tip->tm_mon + 1, tip->tm_mday,
            tip->tm_hour, tip->tm_min, tip->tm_sec);
    }


#ifdef TEST_SUITE
    //syslog_buf[rc++] = ' ';
    //strcpy(syslog_buf+rc, "hostname" );    rc += strlen(syslog_buf+rc);
    SPACE();
    BUFADD( "hostname" );
#else
    /* HOSTNAME field. */
    //syslog_buf[rc++] = ' ';
    SPACE();
    if (confnet.cdn_cip_addr) {
        //strcpy(syslog_buf+rc, inet_ntoa(confnet.cdn_cip_addr));
        //rc += strlen(syslog_buf+rc);
        BUFADD( inet_ntoa(confnet.cdn_cip_addr) );
    }
    else if (confos.hostname[0]) {
        //strcpy(syslog_buf+rc, confos.hostname);
        //rc += strlen(syslog_buf+rc);
        BUFADD( confos.hostname );
    }
    else if (confnet.cdn_ip_addr) {
        //strcpy(syslog_buf+rc, inet_ntoa(confnet.cdn_ip_addr));
        //rc += strlen(syslog_buf+rc);
        BUFADD( inet_ntoa(confnet.cdn_ip_addr) );
    } else {
        //syslog_buf[rc++] = '-';
        DASH();
    }
#endif



    /* APP-NAME field. */
    if (syslog_taglen) {
        //syslog_buf[rc++] = ' ';
        //strcpy(syslog_buf+rc, syslog_tag);
        //rc += syslog_taglen;
        SPACE();
        BUFADD( syslog_tag );
    }

    /* No PROCID and MSGID fields. */
    //syslog_buf[rc++] = ' ';
    //syslog_buf[rc++] = '-';
    //syslog_buf[rc++] = ' ';
    //syslog_buf[rc++] = '-';
    SPACE();
    DASH();
    SPACE();
    DASH();


    SPACE();
    //syslog_buf[rc++] = ' ';
    syslog_buf[rc] = '\0';

    return rc;
}


/*!
 * \brief Print log message, no args.
 *
 */
void syslogs(int pri, const char *fmt)
{
    /* Build the header. */
    size_t cnt = syslog_header(pri);

    if (cnt)
    {
        cnt += snprintf(syslog_buf+cnt, SYSLOG_MAXBUF-cnt, "%s", fmt);
        syslog_flush(cnt);
    }
}


#ifdef TEST_SUITE

int main(void)
{
	syslog_buf = malloc(SYSLOG_MAXBUF+1);

        syslog_tag = "mmNet485";
        syslog_taglen = strlen(syslog_tag);

	size_t rc = syslog_header(0);
	printf("'%s'\n", syslog_buf );
}
void syslog_flush(size_t len)
{
    /* Output to stderr if requested */

    write(1, syslog_buf, len);
    write(1, "\n", 1);


}

#endif // TEST_SUITE
