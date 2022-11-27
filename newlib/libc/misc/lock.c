/*
FUNCTION
<<__retarget_lock_init>>, <<__retarget_lock_init_recursive>>, <<__retarget_lock_close>>, <<__retarget_lock_close_recursive>>, <<__retarget_lock_acquire>>, <<__retarget_lock_acquire_recursive>>, <<__retarget_lock_try_acquire>>, <<__retarget_lock_try_acquire_recursive>>, <<__retarget_lock_release>>, <<__retarget_lock_release_recursive>>---locking routines

INDEX
	__lock___sinit_recursive_mutex
INDEX
	__lock___sfp_recursive_mutex
INDEX
	__lock___atexit_recursive_mutex
INDEX
	__lock___at_quick_exit_mutex
INDEX
	__lock___malloc_recursive_mutex
INDEX
	__lock___env_recursive_mutex
INDEX
	__lock___tz_mutex
INDEX
	__lock___dd_hash_mutex
INDEX
	__lock___arc4random_mutex

INDEX
	__retarget_lock_init
INDEX
	__retarget_lock_init_recursive
INDEX
	__retarget_lock_close
INDEX
	__retarget_lock_close_recursive
INDEX
	__retarget_lock_acquire
INDEX
	__retarget_lock_acquire_recursive
INDEX
	__retarget_lock_try_acquire
INDEX
	__retarget_lock_try_acquire_recursive
INDEX
	__retarget_lock_release
INDEX
	__retarget_lock_release_recursive

SYNOPSIS
	#include <lock.h>
	struct __lock __lock___sinit_recursive_mutex;
	struct __lock __lock___sfp_recursive_mutex;
	struct __lock __lock___atexit_recursive_mutex;
	struct __lock __lock___at_quick_exit_mutex;
	struct __lock __lock___malloc_recursive_mutex;
	struct __lock __lock___env_recursive_mutex;
	struct __lock __lock___tz_mutex;
	struct __lock __lock___dd_hash_mutex;
	struct __lock __lock___arc4random_mutex;

	void __retarget_lock_init (_LOCK_T * <[lock_ptr]>);
	void __retarget_lock_init_recursive (_LOCK_T * <[lock_ptr]>);
	void __retarget_lock_close (_LOCK_T <[lock]>);
	void __retarget_lock_close_recursive (_LOCK_T <[lock]>);
	void __retarget_lock_acquire (_LOCK_T <[lock]>);
	void __retarget_lock_acquire_recursive (_LOCK_T <[lock]>);
	int __retarget_lock_try_acquire (_LOCK_T <[lock]>);
	int __retarget_lock_try_acquire_recursive (_LOCK_T <[lock]>);
	void __retarget_lock_release (_LOCK_T <[lock]>);
	void __retarget_lock_release_recursive (_LOCK_T <[lock]>);

DESCRIPTION
Newlib was configured to allow the target platform to provide the locking
routines and static locks at link time.  As such, a dummy default
implementation of these routines and static locks is provided for
single-threaded application to link successfully out of the box on bare-metal
systems.

For multi-threaded applications the target platform is required to provide
an implementation for @strong{all} these routines and static locks.  If some
routines or static locks are missing, the link will fail with doubly defined
symbols.

PORTABILITY
These locking routines and static lock are newlib-specific.  Supporting OS
subroutines are required for linking multi-threaded applications.
*/

/* dummy lock routines and static locks for single-threaded apps */

#ifndef __SINGLE_THREAD__

#include <sys/lock.h>
#include <stdlib.h>

struct __lock {
	int num_taken;
	int is_taken;
	int thread;  // actually stores thread+1, as 0 is reserved for empty
};

struct __lock __lock___sinit_recursive_mutex;
struct __lock __lock___sfp_recursive_mutex;
struct __lock __lock___atexit_recursive_mutex;
struct __lock __lock___at_quick_exit_mutex;
struct __lock __lock___malloc_recursive_mutex;
struct __lock __lock___env_recursive_mutex;
struct __lock __lock___tz_mutex;
struct __lock __lock___dd_hash_mutex;
struct __lock __lock___arc4random_mutex;

void
__retarget_lock_init (_LOCK_T *lock)
{
  	__retarget_lock_init_recursive(lock);
}

void
__retarget_lock_init_recursive(_LOCK_T *lock)
{
	*lock = (_LOCK_T) calloc(1, sizeof(struct __lock));
}

void
__retarget_lock_close(_LOCK_T lock)
{
  	__retarget_lock_close_recursive(lock);
}

void
__retarget_lock_close_recursive(_LOCK_T lock)
{
  	free(lock);
}

void
__retarget_lock_acquire (_LOCK_T lock)
{
	__retarget_lock_acquire_recursive(lock);
}

void
__retarget_lock_acquire_recursive (_LOCK_T lock)
{
	int tid;
  	asm("csrr %0, mhartid" : "=r"(tid));
	tid += 1;
	if (__atomic_load_n(&lock->thread, __ATOMIC_RELAXED) != tid) {{
		int val = 1;
		while (val != 0) {{
			__atomic_exchange(&lock->is_taken, &val, &val, __ATOMIC_ACQUIRE);
		}}
		__atomic_store_n(&lock->thread, tid, __ATOMIC_RELAXED);
	}}
	lock->num_taken++;
}

int
__retarget_lock_try_acquire(_LOCK_T lock)
{
	return __retarget_lock_try_acquire_recursive(lock);
}

int
__retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
	int tid;
  	asm("csrr %0, mhartid" : "=r"(tid));
	tid += 1;
	if (__atomic_load_n(&lock->thread, __ATOMIC_RELAXED) != tid) {{
		int val = 1;
		__atomic_exchange(&lock->is_taken, &val, &val, __ATOMIC_ACQUIRE);
		if (val != 0) {
			return 0;
		}
		__atomic_store_n(&lock->thread, tid, __ATOMIC_RELAXED);
	}}
	lock->num_taken++;
	return 1;
}

void
__retarget_lock_release (_LOCK_T lock)
{
  __retarget_lock_release_recursive(lock);
}

void
__retarget_lock_release_recursive (_LOCK_T lock)
{
	lock->num_taken--;
	if (lock->num_taken == 0) {
		__atomic_store_n(&lock->thread, 0, __ATOMIC_RELAXED);
		__atomic_store_n(&lock->is_taken, 0, __ATOMIC_RELEASE);
	}
}

#endif /* !defined(__SINGLE_THREAD__) */
