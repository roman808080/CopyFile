#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include "Constants.h"

struct Block
{
	char buffer[Constants::Kilobyte];
   size_t size;
};

struct SharedMemoryBuffer
{
   enum { NumItems = 2 };

   SharedMemoryBuffer()
      :  empty(NumItems), stored(0), failed(false)
   {}

   void setFailed()
   {
      boost::interprocess::scoped_lock lock(mutexIsFailed);
      failed = true;
   }

   bool isFailed()
   {
      boost::interprocess::scoped_lock lock(mutexIsFailed);
      return failed;
   }

   //Semaphores to protect and synchronize access
   boost::interprocess::interprocess_semaphore empty, stored;
   boost::interprocess::interprocess_mutex mutexIsFailed;

   //Items to fill
   Block items[NumItems];
   bool failed;
};
