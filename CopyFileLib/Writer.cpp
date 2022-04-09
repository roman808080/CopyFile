#include "pch.h"
#include "Writer.h"

#include "OutputFile.h"

Writer::Writer(std::shared_ptr<OutputFile> outputFile,
			   std::shared_ptr<ThreadsafeQueue<std::vector<char>>> queue)
	: outputFile(outputFile)
	, queue(queue)
{
}

void Writer::operator()()
{
	writeToFile();
}

void Writer::write()
{
	writeToFile();
}

void Writer::writeToFile()
{
	while (!queue->isFinished())
	{
		auto block = std::move(queue->waitAndPop());
		outputFile->write(std::move(block));
	}
}
