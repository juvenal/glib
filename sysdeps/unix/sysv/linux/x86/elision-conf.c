/* elision-conf.c: Lock elision tunable parameters.
   Copyright (C) 2013-2017 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include "config.h"
#include <pthreadP.h>
#include <init-arch.h>
#include <elision-conf.h>
#include <unistd.h>

/* Macros to register the (TLE) transaction lock elision enablement with
   the new tunables framework.  */

/* Define TLE enablement tunable.  */
#define ELISION_ENABLE (&__pthread_force_elision)
/* Define TLE skip lock busy tunable.  */
#define ELISION_SKIP_LOCK_BUSY (&__elision_aconf.skip_lock_busy)
/* Define TLE skip lock internal abort tunable.  */
#define ELISION_SKIP_LOCK_INTERNAL_ABORT \
  (&__elision_aconf.skip_lock_internal_abort)
/* Define TLE retries tunable.  */
#define ELISION_TRIES (&__elision_aconf.retry_try_xbegin)
/* Define TLE skip trylock internal abort tunable.  */
#define ELISION_SKIP_TRYLOCK_INTERNAL_ABORT \
  (&__elision_aconf.skip_trylock_internal_abort)
/* Define TLE architecture availability check.  */
#define ELISION_CAN_ENABLE (HAS_CPU_FEATURE (RTM))

/* Perform the integration of TLE with tunables framework. The above macros
   should be already defined to became available on the framework.  */
#include "elision-tunables.c"

/* Reasonable initial tuning values, may be revised in the future.
   This is a conservative initial value.  */

struct elision_config __elision_aconf =
  {
    /* How often to not attempt to use elision if a transaction aborted
       because the lock is already acquired.  Expressed in number of lock
       acquisition attempts.  */
    .skip_lock_busy = 3,
    /* How often to not attempt to use elision if a transaction aborted due
       to reasons other than other threads' memory accesses.  Expressed in
       number of lock acquisition attempts.  */
    .skip_lock_internal_abort = 3,
    /* How often we retry using elision if there is chance for the transaction
       to finish execution (e.g., it wasn't aborted due to the lock being
       already acquired.  */
    .retry_try_xbegin = 3,
    /* Same as SKIP_LOCK_INTERNAL_ABORT but for trylock.  */
    .skip_trylock_internal_abort = 3,
  };

/* Force elision for all new locks.  This is used to decide whether existing
   DEFAULT locks should be automatically upgraded to elision in
   pthread_mutex_lock().  Disabled for suid programs.  Only used when elision
   is available.  */

int __pthread_force_elision attribute_hidden = 0;

/* Initialize elision.  */

static void
elision_init (int argc __attribute__ ((unused)),
	      char **argv  __attribute__ ((unused)),
	      char **environ)
{
#ifdef ENABLE_LOCK_ELISION
   /* Note, if the architecture supports lock elision, it is automatically
     activated by default, and should be explicitly turned off by setting the
     appropriate tunable on the supported platform.  */
  __pthread_force_elision = __libc_enable_secure ? 0 : ELISION_CAN_ENABLE;
  elision_init_tunables (environ);
#endif
  if (!__pthread_force_elisions)
    /* Disable elision on rwlocks */
    __elision_aconf.retry_try_xbegin = 0;
}

#ifdef SHARED
# define INIT_SECTION ".init_array"
#else
# define INIT_SECTION ".preinit_array"
#endif

void (*const __pthread_init_array []) (int, char **, char **)
  __attribute__ ((section (INIT_SECTION), aligned (sizeof (void *)))) =
{
  &elision_init
};
