/*-
 * Copyright (c) 2012 Daniel Walter <d.walter@0x90.at>
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#ifndef _SMART_LOCK_H
#define _SMART_LOCK_H

#include <pthread.h>
#include <errno.h>
#include <exception>

namespace SmartLock {

/* Exceptions */ 

class InvalidLockEx: public std::exception {
public:
	virtual const char *what() const throw() {
		return "Invalid lock";
	}
};

class DeadLockEx : public std::exception {
public:
	virtual const char *what() const throw() {
		return "Deadlock";
	}
};

class NumExceededEx : public std::exception {
public:
	virtual const char *what() const throw() {
		return "Number of readers";
	}
};

/* LOCKING classes */

/**
 * SmartLock
 */
class SmartLock {
public:
	SmartLock(pthread_mutex_t *lk) throw (InvalidLockEx, DeadLockEx) {
		if (!lk)
			throw InvalidLockEx();
		lock = lk;
		switch (pthread_mutex_lock(lock)) {
		case EINVAL:
			throw InvalidLockEx();
		case EDEADLK:
			throw DeadLockEx();
		default:
			break;
		}
	};
	~SmartLock() {
		if (lock)
			pthread_mutex_unlock(lock);
		lock = NULL;
	}
protected:
	pthread_mutex_t		*lock;
};

class SmartRWLock {
public:
	SmartRWLock(pthread_rwlock_t *lk) {
		if (!lk)
			throw InvalidLockEx();
		lock = lk;
	};
	~SmartRWLock() {
		if (lock)
			pthread_rwlock_unlock(lock);
		lock = NULL;
	};
protected:
	pthread_rwlock_t	*lock;
};

class SmartRDLock : public SmartRWLock {
public:
	SmartRDLock(pthread_rwlock_t *lk) throw (InvalidLockEx, DeadLockEx, NumExceededEx) : SmartRWLock(lk) {
		switch (pthread_rwlock_rdlock(lock)) {
		case EINVAL:
			throw InvalidLockEx();
		case EAGAIN:
			throw NumExceededEx();
		case EDEADLK:
			throw DeadLockEx();
		default:
			break;
		}
	}
};

class SmartWRLock : public SmartRWLock {
public:
	SmartWRLock(pthread_rwlock_t *lk) throw (InvalidLockEx, DeadLockEx) : SmartRWLock(lk)  {
		switch (pthread_rwlock_wrlock(lock)) {
		case EINVAL:
			throw InvalidLockEx();
		case EDEADLK:
			throw DeadLockEx();
		default:
			break;
		}
	}
};

} /*namespace SmartLock end*/

#endif
