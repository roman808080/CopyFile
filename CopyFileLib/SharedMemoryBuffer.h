#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include "Constants.h"

struct Block
{
	char buffer[Constants::Kilobyte];
   size_t size;
};

struct SharedMemoryBuffer
{
   enum { NumItems = 10 };

   SharedMemoryBuffer()
      :  empty(NumItems), stored(0)
   {}

   //Semaphores to protect and synchronize access
   boost::interprocess::interprocess_semaphore empty, stored;

   //Items to fill
   Block items[NumItems];

};
