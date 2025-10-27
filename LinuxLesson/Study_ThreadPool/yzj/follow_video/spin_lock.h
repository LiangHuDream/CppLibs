#pragma once

#define SPIN_INIT(q) spin_lock_init(&(q)->lock);
#define SPIN_LOCK(q) spin_lock_lock(&(q)->lock);
#define SPIN_UNLOCK(q) spin_lock_unlock(&(q)->lock);
#define SPIN_DESTROY(q) spin_lock_destroy(&(q)->lock);

#ifndef USE_PTHREAD_LOCK

#ifdef __STDC_NO_ATOMICS__

#define atomic_flag_ int
#define ATOMIC_FLAG_INIT_ 0
#define atomic_flag_test_and_set_(ptr) __sync_lock_test_and_set(ptr, 1)
#define atomic_flag_clear_(ptr) __sync_lock_release(ptr)

struct spin_lock {
  atomic_flag_ lock;
};

static inline void spin_lock_init(struct spin_lock *lock) {
  atomic_flag_ v = ATOMIC_FLAG_INIT_;
  lock->lock = v;
}

static inline void spin_lock_lock(struct spin_lock *lock) {
  while (atomic_flag_test_and_set_(&lock->lock)) {
  }
}

static inline int spin_lock_trylock(struct spin_lock *lock) {
  return atomic_flag_test_and_set_(&lock->lock) == 0;
}

static inline void spin_lock_unlock(struct spin_lock *lock) {
  atomic_flag_clear_(&lock->lock);
}

static inline void spin_lock_destroy(struct spin_lock *lock) { (void)lock; }

#else // __STDC_NO_ATOMICS__

#include "atomic.h"

#define atomic_test_and_set_(ptr)                                              \
  STD_ atomic_exchange_explicit(ptr, 1, STD_ memory_order_acquire)
#define atomic_clear_(ptr)                                                     \
  STD_ atomic_store_explicit(ptr, 0, STD_ memory_order_release);
#define atomic_load_relaxed_(ptr)                                              \
  STD_ atomic_load_explicit(ptr, STD_ memory_order_relaxed)

#if defined(__x86_64__)
#include <immintrin.h> // For _mm_pause
#define atomic_pause_() _mm_pause()
#else
#define atomic_pause_() ((void)0)
#endif

struct spin_lock {
  STD_ atomic_int lock;
};

static inline void spin_lock_init(struct spin_lock *lock) {
  STD_ atomic_init(&lock->lock, 0);
}

static inline void spin_lock_lock(struct spin_lock *lock) {
  for (;;) {
    if (!atomic_test_and_set_(&lock->lock))
      return;
    while (atomic_load_relaxed_(&lock->lock))
      atomic_pause_();
  }
}

static inline int spin_lock_trylock(struct spin_lock *lock) {
  return !atomic_load_relaxed_(&lock->lock) &&
         !atomic_test_and_set_(&lock->lock);
}

static inline void spin_lock_unlock(struct spin_lock *lock) {
  atomic_clear_(&lock->lock);
}

static inline void spin_lock_destroy(struct spin_lock *lock) { (void)lock; }

#endif // __STDC_NO_ATOMICS__

#else

#include <pthread.h>

// we use mutex instead of spin_lock for some reason
// you can also replace to pthread_spin_lock

struct spin_lock {
  pthread_mutex_t lock;
};

static inline void spin_lock_init(struct spin_lock *lock) {
  pthread_mutex_init(&lock->lock, NULL);
}

static inline void spin_lock_lock(struct spin_lock *lock) {
  pthread_mutex_lock(&lock->lock);
}

static inline int spin_lock_trylock(struct spin_lock *lock) {
  return pthread_mutex_trylock(&lock->lock) == 0;
}

static inline void spin_lock_unlock(struct spin_lock *lock) {
  pthread_mutex_unlock(&lock->lock);
}

static inline void spin_lock_destroy(struct spin_lock *lock) {
  pthread_mutex_destroy(&lock->lock);
}

#endif

