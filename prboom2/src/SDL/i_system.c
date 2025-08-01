/* Emacs style mode select   -*- C -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *  Misc system stuff needed by Doom, implemented for Linux.
 *  Mainly timer handling.
 *
 *-----------------------------------------------------------------------------
 */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#include "SDL.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#ifdef HAVE_GETPWUID
#include <sys/types.h>
#include <pwd.h>
#endif
#endif

#ifdef _MSC_VER
#include <io.h>
#endif

#include "lprintf.h"
#include "m_file.h"
#include "doomtype.h"
#include "doomdef.h"
#include "d_player.h"
#include "m_fixed.h"
#include "r_fps.h"
#include "e6y.h"
#include "i_system.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "z_zone.h"

#include "dsda/settings.h"
#include "dsda/signal_context.h"
#include "dsda/time.h"
#include "dsda/utility.h"

void I_uSleep(unsigned long usecs)
{
#if defined (__unix__) || defined(__APPLE__) || defined (UNIXCOMMON)
  struct timespec now, target;
  clock_gettime(CLOCK_MONOTONIC, &now);
  target.tv_sec = now.tv_sec + usecs / 1000000;
  target.tv_nsec = now.tv_nsec + (usecs % 1000000) * 1000;

  if (target.tv_nsec >= 1000000000L) {
    target.tv_sec += 1;
    target.tv_nsec -= 1000000000L;
  }

  int status;
  do status = clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &target, NULL);
  while (status == EINTR);
#else
  SDL_Delay(usecs/1000);
#endif
}

static dboolean InDisplay = false;
static int saved_gametic = -1;
dboolean realframe = false;

dboolean I_StartDisplay(void)
{
  if (InDisplay)
    return false;

  realframe = (!movement_smooth) || (gametic > saved_gametic);

  if (realframe)
    saved_gametic = gametic;

  InDisplay = true;
  DSDA_ADD_CONTEXT(sf_display);
  return true;
}

void I_EndDisplay(void)
{
  InDisplay = false;
  DSDA_REMOVE_CONTEXT(sf_display);
}

int interpolation_method;
fixed_t I_GetTimeFrac (void)
{
  fixed_t frac;

  if (!movement_smooth)
  {
    frac = FRACUNIT;
  }
  else
  {
    static fixed_t last_frac;
    static int last_gametic;
    unsigned long long tic_time;
    const double tics_per_usec = TICRATE / 1000000.0f;

    tic_time = dsda_TickElapsedTime();

    frac = (fixed_t) (tic_time * FRACUNIT * tics_per_usec);
    frac = BETWEEN(0, FRACUNIT, frac);

    if (frac < last_frac && last_gametic == gametic)
    {
      frac = FRACUNIT;
    }

    last_frac = frac;
    last_gametic = gametic;
  }

  return frac;
}

/*
 * I_GetRandomTimeSeed
 *
 * CPhipps - extracted from G_ReloadDefaults because it is O/S based
 */
unsigned long I_GetRandomTimeSeed(void)
{
  return (unsigned long)time(NULL);
}

/* cphipps - I_GetVersionString
 * Returns a version string in the given buffer
 */
const char* I_GetVersionString(char* buf, size_t sz)
{
  snprintf(buf, sz, "%s v%s (https://github.com/kraflab/dsda-doom/)", PROJECT_NAME, PROJECT_VERSION);
  return buf;
}

/* cphipps - I_SigString
 * Returns a string describing a signal number
 */
const char* I_SigString(char* buf, size_t sz, int signum)
{
#ifdef HAVE_STRSIGNAL
  if (strsignal(signum) && strlen(strsignal(signum)) < sz)
    strcpy(buf,strsignal(signum));
  else
#endif
  snprintf(buf,sz,"signal %d",signum);
  return buf;
}

/*
 * I_Read
 *
 * cph 2001/11/18 - wrapper for read(2) which handles partial reads and aborts
 * on error.
 */
void I_Read(int fd, void* vbuf, size_t sz)
{
  unsigned char* buf = (unsigned char*)vbuf;

  while (sz) {
    int rc = read(fd,buf,sz);
    if (rc <= 0) {
      I_Error("I_Read: read failed: %s", rc ? strerror(errno) : "EOF");
    }
    sz -= rc; buf += rc;
  }
}

/*
 * I_Filelength
 *
 * Return length of an open file.
 */

int I_Filelength(int handle)
{
  struct stat   fileinfo;
  if (fstat(handle,&fileinfo) == -1)
    I_Error("I_Filelength: %s",strerror(errno));
  return fileinfo.st_size;
}

// Return the path where the executable lies -- Lee Killough
// proff_fs 2002-07-04 - moved to i_system
#ifdef _WIN32

void I_SwitchToWindow(HWND hwnd)
{
  typedef BOOL (WINAPI *TSwitchToThisWindow) (HWND wnd, BOOL restore);
  static TSwitchToThisWindow SwitchToThisWindow = NULL;

  if (!SwitchToThisWindow)
    SwitchToThisWindow = (TSwitchToThisWindow)GetProcAddress(GetModuleHandle("user32.dll"), "SwitchToThisWindow");

  if (SwitchToThisWindow)
  {
    HWND hwndLastActive = GetLastActivePopup(hwnd);

    if (IsWindowVisible(hwndLastActive))
      hwnd = hwndLastActive;

    SetForegroundWindow(hwnd);
    Sleep(100);
    SwitchToThisWindow(hwnd, TRUE);
  }
}

const char *I_ConfigDir(void)
{
  return I_ExeDir();
}

const char *I_ExeDir(void)
{
  extern char **dsda_argv;

  static char *base;
  if (!base)        // cache multiple requests
    {
      size_t len = strlen(*dsda_argv);
      char *p = (base = (char*)Z_Malloc(len+1)) + len - 1;
      strcpy(base,*dsda_argv);
      while (p > base && *p!='/' && *p!='\\')
        *p--=0;
      if (*p=='/' || *p=='\\')
        *p--=0;
      if (strlen(base) < 2 || !M_WriteAccess(base))
      {
        Z_Free(base);
        base = (char*)Z_Malloc(1024);
        if (!M_getcwd(base, 1024) || !M_WriteAccess(base))
          strcpy(base, ".");
      }
    }
  return base;
}

const char* I_GetTempDir(void)
{
  static const char* tmp_path;

  if (!tmp_path)
  {
    wchar_t wpath[PATH_MAX];
    DWORD result;

    result = GetTempPathW(PATH_MAX, wpath);

    if (result == 0 || result > MAX_PATH)
      I_Error("I_GetTempDir: GetTempPathW failed");
    else
      tmp_path = ConvertWideToUtf8(wpath);
  }

  return tmp_path;
}

#elif defined(AMIGA)

const char *I_ConfigDir(void)
{
  return "PROGDIR:";
}

const char *I_ExeDir(void)
{
  return "PROGDIR:";
}

const char* I_GetTempDir(void)
{
  return "PROGDIR:";
}

#else /* not Windows, not Amiga */

static const char *I_GetHomeDir(void)
{
  const char *home = M_getenv("HOME");

  if (!home)
  {
#ifdef HAVE_GETPWUID
    struct passwd *user_info = getpwuid(getuid());
    if (user_info != NULL)
      home = user_info->pw_dir;
    else
#endif
      home = "/";
  }

  return home;
}

// Reference for XDG directories:
// <https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html>
static const char *I_GetXDGDataHome(void)
{
  static char *datahome = 0;

  if (!datahome)
  {
    const char *xdgdatahome = M_getenv("XDG_DATA_HOME");

    if (!xdgdatahome || !*xdgdatahome)
    {
      const char *home = I_GetHomeDir();
      datahome = Z_Malloc(strlen(home) + 1 + sizeof(".local/share"));
      sprintf(datahome, "%s%s%s", home, !HasTrailingSlash(home) ? "/" : "", ".local/share");
    }
    else
    {
      datahome = Z_Strdup(xdgdatahome);
    }
  }
  return datahome;
}

static const char *I_GetXDGDataDirs(void)
{
  const char *datadirs = M_getenv("XDG_DATA_DIRS");

  if (!datadirs || !*datadirs)
    return "/usr/local/share/:/usr/share/";
  return datadirs;
}

const char *I_ConfigDir(void)
{
  static char *base;

  if (!base)
  {
    const char *home = I_GetHomeDir();

    // First, try legacy directory.
    base = dsda_ConcatDir(home, ".dsda-doom");
    if (access(base, F_OK) != 0)
    {
      // Legacy directory is not accessible. Use XDG directory.
      Z_Free(base);

#ifdef __APPLE__
      base = dsda_ConcatDir(home, "Library/Application Support/dsda-doom");
#else
      base = dsda_ConcatDir(I_GetXDGDataHome(), "dsda-doom");
#endif
    }

    M_MakeDir(base, false);
  }

  return base;
}

const char *I_ExeDir(void)
{
  extern char **dsda_argv;

  static char *base;
  if (!base)        // cache multiple requests
    {
      size_t len = strlen(*dsda_argv);
      char *p = (base = (char*)Z_Malloc(len+1)) + len - 1;
      strcpy(base,*dsda_argv);
      while (p > base && *p!='/' && *p!='\\')
        *p--=0;
      if (*p=='/' || *p=='\\')
        *p--=0;
      if (strlen(base) < 2 || !M_WriteAccess(base))
      {
        Z_Free(base);
        base = (char*)Z_Malloc(1024);
        if (!M_getcwd(base, 1024) || !M_WriteAccess(base))
          strcpy(base, ".");
      }
    }
  return base;
}

const char *I_GetTempDir(void)
{
  return "/tmp";
}

#endif

/*
 * HasTrailingSlash
 *
 * cphipps - simple test for trailing slash on dir names
 */

dboolean HasTrailingSlash(const char* dn)
{
  return ( (dn[strlen(dn)-1] == '/')
#if defined(_WIN32)
        || (dn[strlen(dn)-1] == '\\')
#endif
#if defined(AMIGA)
        || (dn[strlen(dn)-1] == ':')
#endif
          );
}

static const char *I_GetBasePath(void)
{
  static char *executable_dir;
  /* SDL_GetBasePath is an expensive call */
  if (!executable_dir)
    executable_dir = SDL_GetBasePath();
  return executable_dir;
}

/*
 * I_FindFile
 *
 * proff_fs 2002-07-04 - moved to i_system
 *
 * cphipps 19/1999 - writen to unify the logic in FindIWADFile and the WAD
 *      autoloading code.
 * Searches the standard dirs for a named WAD file
 * The dirs are listed at the start of the function
 */

#ifdef _WIN32
#define PATH_SEPARATOR ";"
#else
#define PATH_SEPARATOR ":"
#endif

char* I_FindFileInternal(const char* wfname, const char* ext, dboolean isStatic)
{
  // lookup table of directories to search
  static struct {
    const char *dir; // directory
    const char *sub; // subdirectory
    const char *env; // environment variable
    const char *(*func)(void); // for functions that return the directory
  } search0[] = {
    {NULL, NULL, NULL, I_ExeDir}, // executable directory
#if !defined(_WIN32) && !defined(AMIGA)
    {NULL, NULL, NULL, I_ConfigDir}, // config and autoload directory. on windows/amiga, this is the same as I_ExeDir
#endif
    {NULL}, // current working directory
    {NULL, NULL, "DOOMWADDIR"}, // run-time $DOOMWADDIR
    {DOOMWADDIR}, // build-time configured DOOMWADDIR
    {DSDA_ABSOLUTE_PWAD_PATH}, // build-time configured absolute path to dsda-doom.wad
    {NULL, NULL, NULL, I_GetBasePath}, // search the base path provided by SDL
    {NULL, "../share/games/doom", NULL, I_GetBasePath}, // AppImage
    {NULL, "doom", "HOME"}, // ~/doom
    {NULL, NULL, "HOME"}, // ~
#if !defined(_WIN32) && !defined(AMIGA)
    {NULL, "games/doom", NULL, I_GetXDGDataHome}, // $HOME/.local/share/games/doom
#endif
  }, *search;

  static size_t num_search;
  size_t  i;
  size_t  pl;

  static char static_p[PATH_MAX];
  char * dinamic_p = NULL;
  char *p = (isStatic ? static_p : dinamic_p);

  if (!wfname)
    return NULL;

  if (!num_search)
  {
    int extra = 0;
#if !defined(_WIN32) && !defined(AMIGA)
    int datadirs = 0;
#endif
    const char *dwp;

    // calculate how many extra entries we need to add to the table
#if !defined(_WIN32) && !defined(AMIGA)
    dwp = I_GetXDGDataDirs();
    datadirs++;
    while ((dwp = strchr(dwp, *PATH_SEPARATOR)))
      dwp++, datadirs++;
    extra += datadirs * 2; // two entries for each datadir
#endif
    if ((dwp = M_getenv("DOOMWADPATH")))
    {
      extra++;
      while ((dwp = strchr(dwp, *PATH_SEPARATOR)))
        dwp++, extra++;
    }

    // initialize with the static lookup table
    num_search = sizeof(search0)/sizeof(*search0);
    search = Z_Malloc((num_search + extra) * sizeof(*search));
    memcpy(search, search0, num_search * sizeof(*search));
    memset(&search[num_search], 0, extra * sizeof(*search));

#if !defined(_WIN32) && !defined(AMIGA)
    // add $XDG_DATA_DIRS/games/doom and $XDG_DATA_DIRS/doom
    // by default this includes:
    // - /usr/local/share/games/doom
    // - /usr/share/games/doom
    // - /usr/local/share/doom
    // - /usr/share/doom
    {
      char *ptr, *dup_dwp;

      dup_dwp = Z_Strdup(I_GetXDGDataDirs());
      ptr = strtok(dup_dwp, PATH_SEPARATOR);
      while (ptr)
      {
        search[num_search].dir = Z_Strdup(ptr);
        search[num_search].sub = "games/doom";
        search[num_search + datadirs].dir = Z_Strdup(ptr);
        search[num_search + datadirs].sub = "doom";
        num_search++;
        ptr = strtok(NULL, PATH_SEPARATOR);
      }
      Z_Free(dup_dwp);
      num_search += datadirs;
    }
#endif

    // add each directory from the $DOOMWADPATH environment variable
    if ((dwp = M_getenv("DOOMWADPATH")))
    {
      char *ptr, *dup_dwp;

      dup_dwp = Z_Strdup(dwp);
      ptr = strtok(dup_dwp, PATH_SEPARATOR);
      while (ptr)
      {
        search[num_search].dir = Z_Strdup(ptr);
        num_search++;
        ptr = strtok(NULL, PATH_SEPARATOR);
      }
      Z_Free(dup_dwp);
    }
  }

  /* Precalculate a length we will need in the loop */
  pl = strlen(wfname) + (ext ? strlen(ext) : 0) + 4;

  for (i = 0; i < num_search; i++) {
    const char  * d = NULL;
    const char  * s = NULL;
    /* Each entry in the switch sets d to the directory to look in,
     * and optionally s to a subdirectory of d */
    // switch replaced with lookup table
    if (search[i].env) {
      if (!(d = M_getenv(search[i].env)))
        continue;
    } else if (search[i].func)
      d = search[i].func();
    else
      d = search[i].dir;
    s = search[i].sub;

    if (!isStatic)
      p = (char*)Z_Malloc((d ? strlen(d) : 0) + (s ? strlen(s) : 0) + pl);
    sprintf(p, "%s%s%s%s%s", d ? d : "", (d && !HasTrailingSlash(d)) ? "/" : "",
                             s ? s : "", (s && !HasTrailingSlash(s)) ? "/" : "",
                             wfname);

    if (ext && !M_FileExists(p))
      strcat(p, ext);
    if (M_FileExists(p)) {
      if (!isStatic)
        lprintf(LO_DEBUG, " found %s\n", p);
      return p;
    }
    if (!isStatic)
      Z_Free(p);
  }
  return NULL;
}

char* I_RequireFile(const char* wfname, const char* ext) {
  char* result = I_FindFileInternal(wfname, ext, false);

  if (!result)
    I_Error("Unable to find required file \"%s\"", wfname);

  return result;
}

char* I_FindFile(const char* wfname, const char* ext)
{
  return I_FindFileInternal(wfname, ext, false);
}

const char* I_FindFile2(const char* wfname, const char* ext)
{
  return (const char*) I_FindFileInternal(wfname, ext, true);
}

char* I_RequireAnyFile(const char* wfname, const char** ext)
{
  char* result = NULL;

  for (; *ext; ext++)
  {
    result = I_FindFile(wfname, *ext);
    if (result)
      return result;
  }

  I_Error("Unable to find required file \"%s\"", wfname);
}

char* I_RequireWad(const char* wfname)
{
  return I_RequireFile(wfname, ".wad");
}

char* I_FindWad(const char* wfname)
{
  return I_FindFile(wfname, ".wad");
}

char* I_RequireDeh(const char* wfname)
{
  char* result;

  result = I_FindFile(wfname, ".bex");
  if (result)
    return result;

  return I_RequireFile(wfname, ".deh");
}

char* I_FindDeh(const char* wfname)
{
  char* result;

  result = I_FindFile(wfname, ".bex");
  if (result)
    return result;

  return I_FindFile(wfname, ".deh");
}

char* I_RequireZip(const char* wfname)
{
  return I_RequireFile(wfname, ".zip");
}

char* I_FindZip(const char* wfname)
{
  return I_FindFile(wfname, ".zip");
}
