/*
 * Copyright 2008 by egnite Software GmbH
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

#include <dev/nvmem.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <sys/nutdebug.h>
#include <sys/environ.h>

NUTENVIRONMENT *nut_environ;

/*
 * Read string from non-volatile memory.
 */
static size_t read_string(int *addr, char *buf, size_t siz)
{
    size_t rc = 0;
    char ch;

    while (rc <= siz) {
        if (NutNvMemLoad(*addr, &ch, 1)) {
            rc = 0;
            break;
        }
        (*addr)++;
        if (ch == 0) {
            break;
        }
        if (buf) {
            *buf++ = ch;
        }
        rc++;
    }
    if (buf) {
        *buf = 0;
    }
    return rc;
}

/*!
 * \brief Find an NUTENVIRONMENT entry by name.
 *
 * \param name Points to a string, which is the name of the entry.
 *
 * \return A pointer to the entry with the specified name. If the name 
 *         cannot be found, then a null pointer is returned.
 */
NUTENVIRONMENT *findenv(CONST char *name)
{
    NUTENVIRONMENT *envp;
    size_t len;
    size_t max_len = 0;
    char *buf;

    NUTASSERT(name != NULL);

    if (nut_environ == NULL) {
        /* Load environment from non-volatile memory. */
        uint32_t magic;
        int addr = ENVIRON_EE_OFFSET;
        NUTENVIRONMENT *prvp = NULL;

        if (NutNvMemLoad(addr, &magic, sizeof(magic)) == 0 && magic == ENVIRON_MAGIC) {
            addr += 4;
            /* Determine the maximum string length. */
            while ((len = read_string(&addr, NULL, MAX_ENVIRON_ITEM_SIZE)) > 0) {
                if (len > max_len) {
                    max_len = len;
                }
                len = read_string(&addr, NULL, MAX_ENVIRON_ITEM_SIZE);
                if (len > max_len) {
                    max_len = len;
                }
            }
            /* Read the environment. */
            addr = ENVIRON_EE_OFFSET + sizeof(magic);
            if (max_len && (buf = malloc(max_len + 1)) != NULL) {
                while ((len = read_string(&addr, buf, max_len)) > 0) {
                    if ((envp = malloc(sizeof(NUTENVIRONMENT))) == NULL) {
                        break;
                    }
                    memset(envp, 0, sizeof(NUTENVIRONMENT));
                    if ((envp->env_name = malloc(len + 1)) == NULL) {
                        free(envp);
                        break;
                    }
	            strcpy(envp->env_name, buf);
                    len = read_string(&addr, buf, max_len);
                    if ((envp->env_value = malloc(len + 1)) == NULL) {
                        break;
                    }
                    strcpy(envp->env_value, buf);
        	    if (prvp) {
	                prvp->env_next = envp;
                        envp->env_prev = prvp;
	            } else {
		        nut_environ = envp;
	            }
                    prvp = envp;
                }
                free(buf);
            }
        }
    }
    for (envp = nut_environ; envp; envp = envp->env_next) {
        if (strcmp(envp->env_name, name) == 0) {
	    break;
        }
    }
    return envp;
}

