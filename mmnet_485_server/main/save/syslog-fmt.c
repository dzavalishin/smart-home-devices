
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


    /* HOSTNAME field. */
    syslog_buf[rc++] = ' ';
    if (confnet.cdn_cip_addr) {
        strcpy(syslog_buf+rc, inet_ntoa(confnet.cdn_cip_addr));
        rc += strlen(syslog_buf+rc);
    }
    else if (confos.hostname[0]) {
        strcpy(syslog_buf+rc, confos.hostname);
        rc += strlen(syslog_buf+rc);
    }
    else if (confnet.cdn_ip_addr) {
        strcpy(syslog_buf+rc, inet_ntoa(confnet.cdn_ip_addr));
        rc += strlen(syslog_buf+rc);
    } else {
        syslog_buf[rc++] = '-';
    }



    /* APP-NAME field. */
    if (syslog_taglen) {
        syslog_buf[rc++] = ' ';
        strcpy(syslog_buf+rc, syslog_tag);
        rc += syslog_taglen;
    }

    /* No PROCID and MSGID fields. */
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';
    syslog_buf[rc++] = ' ';
    syslog_buf[rc++] = '-';


    syslog_buf[rc++] = ' ';
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
        cnt += snprintf(syslog_buf+cnt, SYSLOG_MAXBUF-cnt "%s", fmt);
        syslog_flush(cnt);
    }
}

