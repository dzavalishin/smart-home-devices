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
#include <errno.h>
#include <memdebug.h>

#include <sys/environ.h>
#include <cfg/crt.h>

/*
 * Save environment in non-volatile memory.
 */
static int save_env(void)
{
    int rc = 0;

    if (nut_environ) {
        NUTENVIRONMENT *envp;
        size_t len;
        uint32_t magic = ENVIRON_MAGIC;
        int addr = ENVIRON_EE_OFFSET;

        if (NutNvMemSave(addr, &magic, sizeof(magic)) == 0) {
            addr += sizeof(magic);
            for (envp = nut_environ; envp; envp = envp->env_next) {
                len = strlen(envp->env_name) + 1;
                if ((rc = NutNvMemSave(addr, envp->env_name, len)) != 0) {
                    break;
                }
                addr += len;
                if (envp->env_value) {
                    len = strlen(envp->env_value) + 1;
                    if ((rc = NutNvMemSave(addr, envp->env_value, len)) != 0) {
                        break;
                    }
                } else {
                    len = 1;
                    if ((rc = NutNvMemSave(addr, "", len)) != 0) {
                        break;
                    }
                }
                addr += len;
            }
            if (rc == 0) {
                rc = NutNvMemSave(addr, "", 1);
            }
        }
    }
    return rc;
}

/*!
 * \brief Remove an environment variable.
 *
 * \param name  Points to a string, which is the name of the variable. 
 * \param value Points to a string, which is the value of the variable.
 * \param force If not zero, existing values will be updated.
 *
 * \return 0 upon successful completion. Otherwise, -1 is returned and 
 *         errno is set to indicate the error.
 */
int setenv(CONST char *name, CONST char *value, int force)
{
    NUTENVIRONMENT *envp;
    NUTENVIRONMENT *nxtp;
    NUTENVIRONMENT *prvp = NULL;

    if ((envp = findenv(name)) == NULL) {
        if ((envp = malloc(sizeof(NUTENVIRONMENT))) == NULL) {
            return -1;
        }
        memset(envp, 0, sizeof(NUTENVIRONMENT));
        if ((envp->env_name = strdup(name)) == NULL) {
            free(envp);
            return -1;
        }

	for (nxtp = nut_environ; nxtp; nxtp = nxtp->env_next) {
	    if (strcmp(envp->env_name, nxtp->env_name) < 0) {
		if (nxtp->env_prev) {
		    nxtp->env_prev->env_next = envp;
		} else {
		    nut_environ = envp;
		}
		envp->env_next = nxtp;
		envp->env_prev = nxtp->env_prev;
		nxtp->env_prev = envp;
		break;
	    }
            prvp = nxtp;
	}
	if (nxtp == NULL) {
	    if (prvp) {
		prvp->env_next = envp;
		envp->env_prev = prvp;
	    } else {
		nut_environ = envp;
	    }
	}
        force = 1;
    }
    if (force) {
        if (envp->env_value) {
            if (strcmp(envp->env_value, value)) {
                free(envp->env_value);
            }
            else {
                force = 0;
            }
        }
        if (force) {
            if ((envp->env_value = strdup(value)) == NULL) {
                return -1;
            }
            return save_env();
        }
    }
    return 0;
}



/*!
 * \brief Remove an environment variable.
 *
 * \param name Points to a string, which is the name of the variable to 
 *             be removed.
 *
 * \return 0 upon successful completion. Otherwise, -1 is returned and 
 *         errno is set to indicate the error.
 */
#ifdef CRT_UNSETENV_POSIX
int unsetenv(CONST char *name)
{
    NUTENVIRONMENT *envp;

    if ((envp = findenv(name)) == NULL) {
        errno = ENOENT;
        return -1;
    }
    if (envp->env_prev) {
        envp->env_prev->env_next = envp->env_next;
    }
    if (envp->env_next) {
	envp->env_next->env_prev = envp->env_prev;
    }
    if (nut_environ == envp) {
	nut_environ = envp->env_next;
    }
    free(envp->env_name);
    free(envp->env_value);
    free(envp);

    save_env();
    return 0;
}


#else 

void unsetenv(CONST char *name)
{
    NUTENVIRONMENT *envp;

    if ((envp = findenv(name)) == NULL) {
        errno = ENOENT;
        return;
    }
    if (envp->env_prev) {
        envp->env_prev->env_next = envp->env_next;
    }
    if (envp->env_next) {
	envp->env_next->env_prev = envp->env_prev;
    }
    if (nut_environ == envp) {
	nut_environ = envp->env_next;
    }
    free(envp->env_name);
    free(envp->env_value);
    free(envp);

    save_env();
}

#endif
