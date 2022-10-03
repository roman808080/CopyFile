#include "SharedMemory.h"

#include "anonymous_semaphore_shared_data.h"

std::unique_ptr<SharedMemory> SharedMemory::tryCreateSharedMemory(const std::string &sharedMemoryName)
{
    try
    {
        shared_memory_object shm(create_only, sharedMemoryName.c_str(), read_write);
        std::unique_ptr<SharedMemory> sharedMemory(std::make_unique<SharedMemory>(sharedMemoryName, std::move(shm)));

        sharedMemory->initMemoryBuffer();
        return std::move(sharedMemory);
    }
    catch(const std::exception& e)
    {
        return nullptr;
    }
}

 std::unique_ptr<SharedMemory> SharedMemory::attachSharedMemory(const std::string &sharedMemoryName)
{
    // Create a shared memory object.
    shared_memory_object shm(open_only, sharedMemoryName.c_str(), read_write);
    std::unique_ptr<SharedMemory> sharedMemory(std::make_unique<SharedMemory>(sharedMemoryName, std::move(shm)));

    sharedMemory->castMemoryBuffer();
    return std::move(sharedMemory);
}

SharedMemory::SharedMemory(const std::string &sharedMemoryName, shared_memory_object &&shm)
: sharedMemoryName(sharedMemoryName)
, shm(std::move(shm)) {
    this->shm.truncate(sizeof(shared_memory_buffer));
    this->region = std::move(mapped_region(this->shm, read_write));
}

SharedMemory::SharedMemory(SharedMemory&& other)
: sharedMemoryName(std::move(other.sharedMemoryName))
, shm(std::move(other.shm))
, region(std::move(other.region))
, data{other.data}
{
    other.data = nullptr;
}

SharedMemory& SharedMemory::operator=(SharedMemory&& other)
{
    if (&other == this)
    {
        return *this;
    }

    this->sharedMemoryName = std::move(other.sharedMemoryName);
    this->shm = std::move(other.shm);
    this->region = std::move(other.region);

    this->data = other.data;
    other.data = nullptr;

    return *this;
}

SharedMemory::~SharedMemory()
{
    if (sharedMemoryName != "")
    {
        shared_memory_object::remove(this->sharedMemoryName.c_str());
    }
}

shared_memory_buffer* SharedMemory::get()
{
    return data;
}

void SharedMemory::initMemoryBuffer()
{
    // Get the address of the mapped region
    void* addr = region.get_address();

    // Construct the shared structure in memory
    data = new (addr) shared_memory_buffer; 
}

void SharedMemory::castMemoryBuffer()
{
    // Get the address of the mapped region
    void* addr = region.get_address();

    // Obtain the shared structure
    data = static_cast<shared_memory_buffer *>(addr);
}

void readFromFileToSharedMemory(InputFile& inputFile, shared_memory_buffer* data)
{
    int iteration = 0;
    while (!inputFile.isFinished())
    {
        data->nempty.wait();

        iteration = iteration % shared_memory_buffer::NumItems;
        auto item = &data->items[iteration];
        inputFile.readBlock(item);

        data->nstored.post();

        ++iteration;
    }

    data->nempty.wait();
    data->items[iteration % shared_memory_buffer::NumItems].size = 0;
    data->nstored.post();
}

void writeFromSharedMemoryToFile(OutputFile& outputFile, shared_memory_buffer* data)
{
    // Extract the data
    int iteration = 0;
    while (true)
    {
        data->nstored.wait();

        iteration = iteration % shared_memory_buffer::NumItems;
        auto item = &data->items[iteration];
        if (item->size == 0)
        {
            return;
        }

        outputFile.write(item);

        data->nempty.post();
        ++iteration;
    }
}
