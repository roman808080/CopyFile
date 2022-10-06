## Links
* [new](http://eel.is/c++draft/expr.new)
* [Windows](https://docs.microsoft.com/en-us/windows/win32/memory/creating-named-shared-memory)
* [Linux](https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c)
* [Boost](https://www.boost.org/doc/libs/1_56_0/doc/html/interprocess/sharedmemorybetweenprocesses.html)

## My Links
* [TutorialPoint Shared Memory](https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_shared_memory.htm)
* [Designing a Queue to be a shared memory](https://stackoverflow.com/questions/8478802/designing-a-queue-to-be-a-shared-memory)
* [Semaphores](https://beej.us/guide/bgipc/html/multi/semaphores.html)
* [Inter process synchronization mechanisms - semaphores](https://www.boost.org/doc/libs/1_38_0/doc/html/interprocess/synchronization_mechanisms.html#interprocess.synchronization_mechanisms.semaphores)
* [Semaphores anonymous example](https://www.boost.org/doc/libs/1_38_0/doc/html/interprocess/synchronization_mechanisms.html#interprocess.synchronization_mechanisms.semaphores.semaphores_anonymous_example)
* [Named mutex removal](https://stackoverflow.com/questions/7555253/boost-named-mutex-and-remove-command)
* [Boost interprocess mutexes and checking for abandonment](https://stackoverflow.com/questions/15772768/boost-interprocess-mutexes-and-checking-for-abandonment)
* [A description of what robust futexes are¶](https://www.kernel.org/doc/html/next/locking/robust-futexes.html)
* [Semaphores are Surprisingly Versatile](https://preshing.com/20150316/semaphores-are-surprisingly-versatile/)
* []()


Removing shared memory with python
------------

~~~python
from multiprocessing import shared_memory

# for shared memory
shm = shared_memory.SharedMemory('crazy')
shm.close()
shm.unlink()

# for named semaphore (implemented by using shared memory)
shm = shared_memory.SharedMemory('sem.crazy')
shm.close()
shm.unlink()
~~~