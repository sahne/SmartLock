#include "SmartLock.h"
#include <stdlib.h>
#include <pthread.h>
#include <iostream>

using namespace std;

pthread_mutex_t *foo_lock;
pthread_rwlock_t *bar_lock;

void foo();
void bar();
void baf();

int
main (void)
{
	foo_lock = new pthread_mutex_t();
	pthread_mutex_init(foo_lock, NULL);
	std::cout << "calling foo" << std::endl;
	foo();
	std::cout << "done" << std::endl;
	try {
		std::cout << "calling bar uninitialized " << std::endl;
		bar();
		std::cout << "done" << std::endl;
	} catch (SmartLock::InvalidLockEx &e) {
		std::cout << e.what() << std::endl;
	}
	bar_lock = new pthread_rwlock_t();
	pthread_rwlock_init(bar_lock, NULL);
	std::cout << "calling bar" << std::endl;
	bar();
	std::cout << "done" << std::endl;
	std::cout << "calling baf" << std::endl;
	baf();
	std::cout << "done" << std::endl;
	return 0;
}

void
foo ()
{
	SmartLock::SmartLock((pthread_mutex_t *)foo_lock);
}

void
bar ()
{
	SmartLock::SmartRDLock((pthread_rwlock_t*)bar_lock);
}

void
baf ()
{
	try {	
		SmartLock::SmartWRLock((pthread_rwlock_t*)bar_lock);
		SmartLock::SmartRDLock((pthread_rwlock_t*)bar_lock);
		SmartLock::SmartWRLock((pthread_rwlock_t*)bar_lock);
		std::cout << "hmm, why ? " << std::endl;
	} catch (std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}
