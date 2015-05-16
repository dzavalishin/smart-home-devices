/*
 * Copyright 2010 by egnite GmbH
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*
 * \file gorp/buffer/perci.c
 * \brief Persistent circular buffer.
 *
 * Applications may use this module for logging any kind of data.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <sys/nutdebug.h>
#include <sys/event.h>

#include <gorp/perci.h>

/*!
 * \addtogroup xgPerci
 */
/*@{*/

//#define NUTDEBUG

/*!
 * \brief Dump ring buffer file to given stream.
 *
 * \param path   Path name of the file.
 * \param stream Stream to dump the contents to.
 */
void PerCiDump(FILE *stream, char *path)
{
#ifdef NUTDEBUG
    int fd;
    int got;
    perci_fast_reclen_t i;
    perci_fast_reclen_t ll;
    perci_recnum_t recnum;
    perci_reclen_t reclen;
    uint8_t *recbuf;

    fd = _open(path, _O_RDWR | _O_BINARY);
    if (fd == -1) {
        fprintf(stream, "Error %d opening %s\n", errno, path);
        return;
    } else {
        fprintf(stream, "Dump %ld bytes in %s\n", _filelength(fd), path);
        recbuf = malloc(PERCI_DATASIZE);
        for (recnum = 0;; recnum++) {
            got = _read(fd, &reclen, sizeof(perci_reclen_t));
            if (got <= 0) {
                break;
            }
            fprintf(stream, "%03u %03u ", recnum, (unsigned int)reclen);
            got = _read(fd, recbuf, PERCI_DATASIZE);
            for (i = 0, ll = 0; i < (perci_fast_reclen_t) got && i <= (perci_fast_reclen_t) reclen; i++) {
                if (recbuf[i] < ' ') {
                    ll += 2;
                    if (recbuf[i] == '\n') {
                        fputs("\\n", stream);
                    }
                    else if (recbuf[i] == '\r') {
                        fputs("\\r", stream);
                    }
                    else if (recbuf[i] == '\t') {
                        fputs("\\t", stream);
                    } else {
                        fprintf(stream, "\\x%02x", recbuf[i]);
                        ll += 2;
                    }
                } else {
                    fputc(recbuf[i], stream);
                    ll++;
                }
                if (ll > 80) {
                    fprintf(stream, "...");
                    break;
                }
            }
            fputc('\n', stream);
        }
        _close(fd);
        free(recbuf);
    }
#endif
}

/*!
 * \brief Initialize a ring buffer file.
 *
 * If the file doesn't exist, a new file will be created. If the file 
 * exists, any buffered data will be erased.
 *
 * The total size of the file is
 * \code
 * recs * PERCI_RECSIZE + sizeof(perci_reclen_t)
 * \endcode
 * bytes.
 *
 * The available data space is
 * \code
 * recs * PERCI_DATASIZE
 * \endcode
 * bytes.
 *
 * \param path Path name of the file.
 * \param recs Number of buffer records, at least 2.
 *
 * \return 0 on success and -1 on failure.
 */
int PerCiInit(char *path, int recs)
{
    int fd;
    int i;
    PERCI_RECORD *rec;

    /* Check function parameters. */
    NUTASSERT(path != NULL);
    NUTASSERT(recs >= 2);

    fd = _open(path, _O_RDWR | _O_CREAT | _O_TRUNC | _O_BINARY);
    if (fd == -1) {
        return -1;
    }
    rec = calloc(PERCI_RECSIZE, 1);
    if (rec) {
        for (i = 0; i < recs; i++) {
            _write(fd, rec, PERCI_RECSIZE);
        }
        _write(fd, rec, sizeof(perci_reclen_t));
        free(rec);
    }
    _close(fd);

    return 0;
}

/*!
 * \brief Open a ring buffer file.
 *
 * The file must have been created by a previous call to PerCiInit().
 *
 * The function failes, if the file doesn't exist or if it is corrupted.
 * Applications may use the following sequence to open a circular buffer.
 * \code
 * PERCI_WRITER *perci;
 * char *path = "UFLASH0:data.log";
 * 
 * while ((perci = PerCiOpen(path)) == NULL) {
 *     if (PerCiInit(path, 128)) {
 *         printf("Error %d creating %s\n", errno, path);
 *         break;
 *     }
 * }
 * \endcode
 *
 * \param path Path name of the file.
 *
 * \return A pointer to a PERCI_WRITER structure on success. The return
 *         of a NULL pointer indicates a failure.
 */
PERCI_WRITER *PerCiOpen(char *path)
{
    uint_fast8_t ok = 0;
    PERCI_WRITER *writer;
    perci_reclen_t reclen;

    /* Check function parameter. */
    NUTASSERT(path != NULL);

    /* Allocate a writer structure and open the file. */
    writer = calloc(1, sizeof(PERCI_WRITER));
    if (writer) {
        /* Open the file. If this fails, release the writer structure
           and return a NULL pointer. */
        writer->pcw_fd = _open(path, _O_RDWR | _O_BINARY);
        if (writer->pcw_fd != -1) {
            /* File exists, determine its size. If it doesn't contain at
               least 2 records, we consider it corrupted. */
            writer->pcw_size = _filelength(writer->pcw_fd);
            if (writer->pcw_size >= 2 * PERCI_RECSIZE + sizeof(perci_reclen_t)) {
                writer->pcw_size -= sizeof(perci_reclen_t);

                /* 
                 * Scan the file for the next available record. 
                 */
                _seek(writer->pcw_fd, 0, SEEK_SET);
                for (writer->pcw_recnum = 0; writer->pcw_recnum < PERCI_MAX_RECORDS; writer->pcw_recnum++) {
                    /* Read the length of the current record. */
                    if (_read(writer->pcw_fd, &reclen, sizeof(perci_reclen_t)) != sizeof(perci_reclen_t)) {
                        /* Corrupted. */
                        break;
                    }
                    /* If this record isn't completely filled, then we continue
                       writing at this point. */
                    if (reclen < PERCI_DATASIZE) {
                        /* Fill the record buffer. */
                        writer->pcw_rec.pcd_len = reclen;
                        if (reclen && _read(writer->pcw_fd, writer->pcw_rec.pcd_data, reclen) != reclen) {
                            break;
                        }
                        NutEventPost(&writer->pcw_mutex);
                        ok = 1;
                        break;
                    }
                    /* This record is filled, move to the next one. */
                    _seek(writer->pcw_fd, PERCI_DATASIZE, SEEK_CUR);
                }
            }
        }
    }
    /* Release resources on error. */
    if (!ok && writer) {
        if (writer->pcw_fd != -1) {
            _close(writer->pcw_fd);
        }
        free(writer);
        writer = NULL;
    }
    return writer;
}

/*!
 * \brief Flush the current write buffer.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 */
void PerCiFlush(PERCI_WRITER * writer)
{
    /* Check function parameter. */
    NUTASSERT(writer != NULL);
    NUTASSERT(writer->pcw_fd != -1);

    _seek(writer->pcw_fd, writer->pcw_recnum * PERCI_RECSIZE, SEEK_SET);
    _write(writer->pcw_fd, &writer->pcw_rec, sizeof(perci_reclen_t) + writer->pcw_rec.pcd_len);
}

/*!
 * \brief Close a ring buffer file.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 */
void PerCiClose(PERCI_WRITER * writer)
{
    /* Check function parameter. */
    NUTASSERT(writer != NULL);

    if (writer->pcw_rec.pcd_len) {
        PerCiFlush(writer);
    }
    _close(writer->pcw_fd);
    free(writer);
}

/*!
 * \brief Write to a ring buffer file.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 *
 * \return The number of bytes successfully written or -1 on failure.
 */
int PerCiWrite(PERCI_WRITER * writer, CONST char *data, int len)
{
    perci_fast_reclen_t cnt = 0;
    perci_fast_reclen_t num;
    perci_fast_reclen_t reclen;

    /* Check parameters. */
    NUTASSERT(writer != NULL);
    NUTASSERT(writer->pcw_fd != -1);
    NUTASSERT(writer->pcw_rec.pcd_len <= PERCI_DATASIZE);

    NutEventWait(&writer->pcw_mutex, NUT_WAIT_INFINITE);
    while ((perci_fast_reclen_t) len > cnt) {
        /* Calculate the number of bytes to write in the next step. */
        reclen = (perci_fast_reclen_t) writer->pcw_rec.pcd_len;
        num = (perci_fast_reclen_t) len - cnt;
        if (num > PERCI_DATASIZE - reclen) {
            num = PERCI_DATASIZE - reclen;
        }
        /* Move the bytes to the record buffer. */
        memcpy(&writer->pcw_rec.pcd_data[reclen], data, num);
        writer->pcw_rec.pcd_len += num;
        cnt += num;
        data += num;

        /* If the buffered record is completely filled, then write it 
           back to the file and claim the next one. Note, that we write
           a whole record, which is sizeof(perci_reclen_t) larger than
           the real record size. This way we automatically override the
           length of the following record with zero. */
        if (writer->pcw_rec.pcd_len == PERCI_DATASIZE) {
            _seek(writer->pcw_fd, writer->pcw_recnum * PERCI_RECSIZE, SEEK_SET);
            if (_write(writer->pcw_fd, &writer->pcw_rec, sizeof(PERCI_RECORD)) != sizeof(PERCI_RECORD)) {
                NutEventPost(&writer->pcw_mutex);
                return -1;
            }
            writer->pcw_rec.pcd_len = 0;
            writer->pcw_recnum++;
            /* Check for wrap around. */
            if (writer->pcw_recnum * PERCI_RECSIZE >= writer->pcw_size) {
                writer->pcw_recnum = 0;
                _seek(writer->pcw_fd, 0, SEEK_SET);
                if (_write(writer->pcw_fd, &writer->pcw_rec.pcd_len, sizeof(perci_reclen_t)) != sizeof(perci_reclen_t)) {
                    NutEventPost(&writer->pcw_mutex);
                    return -1;
                }
            }
        }
    }
    NutEventPost(&writer->pcw_mutex);
    return cnt;
}

/*!
 * \brief Write formatted line to a ring buffer file.
 *
 * Alternate form of PerCiWriteFormat(), in which the arguments have 
 * already been captured using the variable-length argument facilities.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 * \param fmt    Format string containing conversion specifications 
 *               like printf.
 * \param ap     Pointer to the list of arguments.
 *
 * \return The number of bytes successfully written or -1 on failure.
 */
int PerCiWriteVarList(PERCI_WRITER * writer, CONST char *fmt, va_list ap)
{
    int cnt;
    char *line;

    /* Check function parameter. */
    NUTASSERT(fmt != NULL);

    line = malloc(PERCI_DATASIZE);
    if (line) {
        /* Potentially dangerous. We need vsnprintf() */
        cnt = vsprintf(line, fmt, ap);
        NUTASSERT(cnt < PERCI_DATASIZE);
        cnt = PerCiWrite(writer, line, strlen(line));
        free(line);
    } else {
        cnt = -1;
    }
    return cnt;
}


/*!
 * \brief Write formatted line to a ring buffer file.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 * \param fmt    Format string containing conversion specifications 
 *               like printf.
 *
 * \return The number of bytes successfully written or -1 on failure.
 */
int PerCiWriteFormat(PERCI_WRITER * writer, CONST char *fmt, ...)
{
    int rc;
    va_list ap;

    /* Check function parameter. */
    NUTASSERT(fmt != NULL);

    va_start(ap, fmt);
    rc = PerCiWriteVarList(writer, fmt, ap);
    va_end(ap);

    return rc;
}

/*!
 * \brief Find next record with data.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 * \param recnum Pointer to a variable, which contains the number of
 *               the start record upon entry and which will contain
 *               the number of the record found upon exit.
 *
 * \return The number of bytes contained in the record found, or 0 if
 *         no more records with data are available.
 */
static perci_fast_reclen_t FindNextData(PERCI_WRITER * writer, perci_fast_recnum_t * recnum)
{
    perci_reclen_t reclen;
    long pos;
    int got;

    /* Check parameters. */
    NUTASSERT(writer != NULL);

    NutEventWait(&writer->pcw_mutex, NUT_WAIT_INFINITE);
    pos = *recnum * PERCI_RECSIZE;
    for (;;) {
        /* If we reached the end of the file, then continue at its
           beginning. */
        if (pos >= writer->pcw_size) {
            pos = 0;
            *recnum = 0;
        }
        /* If we reached the latest record, then return the number
           of bytes in the write buffer. */
        if (*recnum == writer->pcw_recnum) {
            reclen = writer->pcw_rec.pcd_len;
            break;
        }
        /* Otherwise read the number of bytes available in the record.
           This value is stored at the beginning of each record. */
        _seek(writer->pcw_fd, pos, SEEK_SET);
        got = _read(writer->pcw_fd, &reclen, sizeof(perci_reclen_t));
        if (got == sizeof(perci_reclen_t) && reclen) {
            break;
        }
        /* Either this record is empty or the read failed, which we
           sliently ignore for now. Move on to the next record. */
        pos += PERCI_RECSIZE;
        (*recnum)++;
    }
    NutEventPost(&writer->pcw_mutex);
    return reclen;
}

/*!
 * \brief Start reading from a ring buffer file.
 *
 * It is assumed, that the ring buffer is kept open while the application
 * is running. From time to time the buffered data will be retrieved.
 * Therefore, a reader is attached to a previously open file and reading
 * starts at the oldest record.
 *
 * When all data has been read, the application must call PerCiDetachReader()
 * to relase all allocated resources for reading.
 *
 * Multiple readers may be concurrently attached to the same file.
 *
 * \param writer Pointer to a PERCI_WRITER structure obtained by a 
 *               previous call to PerCiOpen().
 *
 * \return A pointer to a PERCI_READER structure on success. The return
 *         of a NULL pointer indicates a failure.
 */
PERCI_READER *PerCiAttachReader(PERCI_WRITER * writer)
{
    PERCI_READER *reader;

    /* Check parameters. */
    NUTASSERT(writer != NULL);

    reader = malloc(sizeof(PERCI_READER));
    reader->pcr_cil = writer;
    reader->pcr_recpos = 0;
    /* Search the oldest record that contains data. Start with the one 
       above the current record of the writer. */
    reader->pcr_recnum = writer->pcw_recnum + 1;
    reader->pcr_reclen = FindNextData(writer, &reader->pcr_recnum);

    return reader;
}

/*!
 * \brief Stop reading from a ring buffer file.
 *
 * \param reader Pointer to a PERCI_READER structure obtained by a 
 *               previous call to PerCiAttachReader().
 */
void PerCiDetachReader(PERCI_READER * reader)
{
    /* Check parameters. */
    NUTASSERT(reader != NULL);
    free(reader);
}

/*!
 * \brief Read data from a ring buffer file.
 *
 * \param reader Pointer to a PERCI_READER structure obtained by a 
 *               previous call to PerCiAttachReader().
 * \param data   Pointer to the buffer that receives the data.
 * \param len    Number of bytes to read.
 *
 * \return The number of bytes read. If this is lower than the
 *         requested length, then we reached the end.
 */
int PerCiRead(PERCI_READER * reader, char *data, int len)
{
    int cnt = 0;
    perci_fast_reclen_t num;
    int got;

    /* Check parameters. */
    NUTASSERT(reader != NULL);

    /*
     * Loop for for number of requested bytes.
     */
    while (len > cnt) {
        /* Calculate the number of bytes to read in the next step. */
        num = len - cnt;
        if (num > reader->pcr_reclen - reader->pcr_recpos) {
            num = reader->pcr_reclen - reader->pcr_recpos;
        }
        /* Check if we reached the last record. This is the one currently
           used by the writer. */
        if (reader->pcr_recnum == reader->pcr_cil->pcw_recnum) {
            if (num) {
                /* Get the data from the write buffer. */
                memcpy(data, &reader->pcr_cil->pcw_rec.pcd_data[reader->pcr_recpos], num);
                data += num;
                cnt += num;
                reader->pcr_recpos += num;
            } else {
                /* All data read. */
                break;
            }
        }
        /* Not the last record. Continue reading from the file. */
        else {
            /* Check if we reached the end of the current record. */
            if (num) {
                got = _read(reader->pcr_cil->pcw_fd, data, num);
                if (got != num) {
                    break;
                }
                data += num;
                cnt += num;
                reader->pcr_recpos += num;
            } else {
                /* We consumed all data in this record. Step to the next. */
                reader->pcr_recnum++;
                reader->pcr_recpos = 0;
                /* Find the next record containing data. */
                reader->pcr_reclen = FindNextData(reader->pcr_cil, &reader->pcr_recnum);
                if (reader->pcr_reclen == 0) {
                    /* No more data available. */
                    break;
                }
            }
        }
    }
    return cnt;
}

/*!
 * \brief Read a text line from a ring buffer file.
 *
 * \param reader Pointer to a PERCI_READER structure obtained by a 
 *               previous call to PerCiAttachReader().
 * \param data   Pointer to the buffer that receives the data.
 * \param len    Number of bytes to read.
 *
 * \return The number of bytes read. If this is lower than the
 *         requested length, then we reached the end.
 */
int PerCiReadLine(PERCI_READER * reader, char *line, int len)
{
    int cnt = 0;
    char *cp = line;

    /* Check function parameters. */
    NUTASSERT(reader != NULL);
    NUTASSERT(line != NULL);

    while (cnt < len) {
        if (PerCiRead(reader, cp, 1) != 1) {
            break;
        }
        cnt++;
        if (*cp++ == '\n') {
            break;
        }
    }
    *cp = 0;

    return cnt;
}

/*@}*/
