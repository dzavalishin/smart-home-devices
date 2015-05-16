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

#include <sys/environ.h>

static char empty_envval[1] = {0};

/*!
 * \brief Get the value of an environment variable.
 *
 * \param name Points to a string, which is the name of the variable.
 *
 * \return A pointer to a string containing the value. If the specified 
 *         name cannot be found a null pointer will be returned.
 */
char *getenv(CONST char *name)
{
    NUTENVIRONMENT *envp;

    if ((envp = findenv(name)) == NULL) {
        return NULL;
    }
    if (envp->env_value != NULL) {
        return envp->env_value;
    }
    return empty_envval;
}

/*!
 * \brief Get the name of the next environment variable.
 *
 * \param name Points to a string, which is the name of the previous 
 *             variable. If this pointer is NULL or points to an empty
 *             string, then the name of the first variable is returned.
 *
 * \return A pointer to a string containing the name of the next variable,
 *         or NULL if no more variables are available.
 */
char *nextenv(CONST char *name)
{
    NUTENVIRONMENT *envp;

    if (name && *name) {
        if ((envp = findenv(name)) != NULL && envp->env_next != NULL) {
            return envp->env_next->env_name;
        }
    } 
    else if (nut_environ) {
        return nut_environ->env_name;
    }
    return NULL;
}
