/* Copyright (C) 2016 Free Software Foundation, Inc.
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

#if BUILD_TUNABLES

# define TUNABLE_NAMESPACE pthread
# include <elf/dl-tunables.h>

/* Define a function to parse an integral tunable value and
   set the pointer this value.  */
# define _ELISION_TUNABLE_INT_FUNC(tunable, ptr) \
  void \
  _elision_set_ ## tunable (const char * value) \
  { \
    *ptr = atoi (value); \
  }

/* Define some helper macros to register a tunable, and
   define the setter function.  Note, this defines a
   nested function, and the tunable macro likely resolves
   to a function call.  */
# define _ELISION_TUNABLE_REGISTER(name, mname) \
  _ELISION_TUNABLE_INT_FUNC (name, mname); \
  TUNABLE_REGISTER (pthread, elision_ ## name, \
			     &_elision_set_ ## name)

/* Initialize the tunables based on what the platform has defined as
   being available.  */
static void __attribute__((unused))
elision_init_tunables (char **envp)
{
  /* Don't attempt to do anything if elision isn't supported */
  if (!ELISION_CAN_ENABLE || __libc_enable_secure)
    return;

  _ELISION_TUNABLE_REGISTER (enable, ELISION_ENABLE);

# ifdef ELISION_SKIP_LOCK_BUSY
  _ELISION_TUNABLE_REGISTER (skip_lock_busy, ELISION_SKIP_LOCK_BUSY);
# endif

# ifdef ELISION_SKIP_LOCK_INTERNAL_ABORT
  _ELISION_TUNABLE_REGISTER (skip_lock_internal_abort,
			     ELISION_SKIP_LOCK_INTERNAL_ABORT);
# endif

# ifdef ELISION_SKIP_LOCK_AFTER_RETRIES
  _ELISION_TUNABLE_REGISTER (skip_lock_after_retries,
			     ELISION_SKIP_LOCK_AFTER_RETRIES);
# endif

# ifdef ELISION_TRIES
  _ELISION_TUNABLE_REGISTER (tries, ELISION_TRIES);
# endif

# ifdef ELISION_SKIP_TRYLOCK_INTERNAL_ABORT
  _ELISION_TUNABLE_REGISTER (skip_trylock_internal_abort,
			     ELISION_SKIP_TRYLOCK_INTERNAL_ABORT);
# endif

  tunables_init (envp);
}

#else

# define elision_init_tunables(x)

#endif /* BUILD_TUNABLES */
