#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include "Constants.h"

struct Block
{
	char buffer[Constants::Kilobyte];
   size_t size;
};

struct shared_memory_buffer
{
   enum { NumItems = 10 };

   shared_memory_buffer()
      : mutex(1), nempty(NumItems), nstored(0)
   {}

   //Semaphores to protect and synchronize access
   boost::interprocess::interprocess_semaphore
      mutex, nempty, nstored;

   //Items to fill
   Block items[NumItems];

};
