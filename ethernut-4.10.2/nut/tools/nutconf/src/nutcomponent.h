#ifndef _NUTCOMPONENT_H_
#define _NUTCOMPONENT_H_

/*
 * Copyright (C) 2008-2012 by egnite GmbH
 * Copyright (C) 2004-2007 by egnite Software GmbH
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

/*!
 * $Id: nutcomponent.h 3878 2012-02-15 15:46:22Z haraldkipp $
 */

/* This construct avoids automatic indention. */
#ifndef __BEGIN_DECLS
#ifdef  __cplusplus
# define __BEGIN_DECLS  extern "C" {
# define __END_DECLS    }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif
#endif

/*
 * Options
 *
 * enabled - can be modified.
 * active - macro is included.
 * active_if - automatically activated.
 *
 * requires - needs this function (may result in conflict)
 * provides - provides this funktion
 */

typedef struct _NUTCOMPONENTOPTION NUTCOMPONENTOPTION;
typedef struct _NUTCOMPONENT NUTCOMPONENT;

struct _NUTCOMPONENTOPTION {
    /*! \brief Pointer to next option. */
    NUTCOMPONENTOPTION *nco_nxt;
    /*! \brief Pointer to previous option. */
    NUTCOMPONENTOPTION *nco_prv;
    /*! \brief Component reference. */
    NUTCOMPONENT *nco_compo;
    /*! \brief C macro name of this option. */
    char *nco_name;
    /*! \brief Enable flag. */
    int  nco_enabled;
    /*! \brief Active flag. */
    int  nco_active;
    /*! \brief Not yet used. */
    char *nco_active_if;
    /*! \brief Edited data value. */
    char *nco_value;
    /*! \brief Exclusivity list, static, deprecated. */
    char **nco_exclusivity;
};

struct _NUTCOMPONENT {
    /*! \brief Pointer to next sibling component. */
    NUTCOMPONENT *nc_nxt;
    /*! \brief Pointer to previous sibling component. */
    NUTCOMPONENT *nc_prv;
    /*! \brief Pointer to first parent component. */
    NUTCOMPONENT *nc_parent;
    /*! \brief Pointer to first child component. */
    NUTCOMPONENT *nc_child;
    /*! \brief Pointer to first option. */
    NUTCOMPONENTOPTION *nc_opts;
    /*! \brief Name of this component. */
    char *nc_name;
    /*! \brief Enable flag.
     *
     * A component is enabled, if all requirements of this component and
     * all parent components are provided by other components or options.
     */
    int  nc_enabled;
    /*! \brief Not yet used. */
    char *nc_active_if;
    /*! \brief Exclusivity list, static, deprecated. */
    char **nc_exclusivity;
};

typedef struct _NUTREPOSITORY NUTREPOSITORY;

struct _NUTREPOSITORY {
    /*! \brief Lua state. */
    void *nr_ls;
    /*! \brief Root directory of the repository.
     *
     * Typically points to nut/conf within the source tree.
     */
    char *nr_dir;
    /*! \brief Filename of the top Lua script.
     *
     * Contains the top level components. Default is repository.nut.
     */
    char *nr_name;
};

__BEGIN_DECLS
/* Function prototypes */
extern NUTREPOSITORY *OpenRepository(const char *pathname);
extern void CloseRepository(NUTREPOSITORY *repo);

extern int RegisterSourcePath(NUTREPOSITORY *repo, const char *path);
extern int RegisterBuildPath(NUTREPOSITORY *repo, const char *path);
extern int RegisterLibPath(NUTREPOSITORY *repo, const char *path);
extern int RegisterSamplePath(NUTREPOSITORY *repo, const char *path);
extern int RegisterCompilerPlatform(NUTREPOSITORY *repo, const char *platform);

extern NUTCOMPONENT *LoadComponents(NUTREPOSITORY *repo);
extern void ReleaseComponents(NUTCOMPONENT *comp);

extern void ReleaseStringArray(char **stringarray);

extern int ConfigureComponents(NUTREPOSITORY *repo, NUTCOMPONENT *root, const char *pathname);
extern char * GetComponentBrief(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char * GetComponentDescription(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char * GetComponentSubdir(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char **GetComponentRequirements(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char **GetComponentProvisions(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char **GetComponentSources(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char **GetComponentTargets(NUTREPOSITORY *repo, NUTCOMPONENT *comp);
extern char **GetComponentMakedefs(NUTREPOSITORY *repo, NUTCOMPONENT *comp);

extern char * GetOptionBrief(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char * GetOptionDescription(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char * GetOptionDefault(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char * GetOptionFile(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char * GetOptionFlavour(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char * GetOptionTypeString(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char **GetOptionRequirements(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char **GetOptionProvisions(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char **GetOptionChoices(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);
extern char **GetOptionMakedefs(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);

extern char * GetConfigValue(NUTREPOSITORY *repo, char * name);
extern char * GetConfigValueOrDefault(NUTREPOSITORY *repo, NUTCOMPONENT *comp, char * name);


extern int RefreshComponents(NUTREPOSITORY *repo, NUTCOMPONENT *root);

extern int CreateMakeFiles(NUTREPOSITORY *repo, NUTCOMPONENT *root, const char *bld_dir, const char *src_dir, 
                           const char *mak_ext, const char *ifirst_dir, const char *ilast_dir, const char *ins_dir);
extern int CreateHeaderFiles(NUTREPOSITORY *repo, NUTCOMPONENT * root, const char *bld_dir);
extern int CreateSampleDirectory(NUTREPOSITORY *repo, NUTCOMPONENT * root, const char *bld_dir, const char *app_dir, 
                                 const char *src_dir, const char *lib_dir, const char *mak_ext, const char *prg_ext,
                                 const char *ifirst_dir, const char *ilast_dir);

const int GetScriptStatus(void);
const char *GetScriptErrorString(void);

__END_DECLS                     /* */
#endif
