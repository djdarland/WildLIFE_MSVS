/*! \file defs.h
  \brief includes

*/
#define _CRT_SECURE_NO_WARNINGS
#include "def_config.h"
#include <direct.h>
//#include <stdlib.h>
#include <cstdlib>

// #include <unistd.h>
#include <stdio.h>

#include <stdarg.h>
#include <cstring>
// #include <strings.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <setjmp.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
// #include <sys/time.h>
// #include <sys/times.h>
#include <time.h>
#include <ctime>
#include <sys/types.h>
// #include <sys/ioctl.h>
#ifndef NORAW
#include <sgtty.h>
#include <termio.h>

#ifndef TANDEM
#define TANDEM 0
#endif


#endif
#pragma once
// #include <sys/wait.h>
#include <sys/stat.h>
#include <limits.h>
//#include <pwd.h>


#ifdef X11
#include <X11/Xlib.h>
#ifdef NEEDXLIBINT
#include <X11/Xlibint.h>
#endif
#include <X11/Xutil.h>
#include "life_icon"
#endif
#include "str_const.h"

#include "def_const.h"
#include "def_macro.h"
#include "def_struct.h"
#include "wl_alloc.h"
#include "def_glob.h"
#include "def_proto.h"
