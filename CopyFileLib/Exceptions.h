#pragma once
#include <stdexcept>

class Exception : public std::runtime_error
{
public:
    Exception(const std::string& what = "")
        : std::runtime_error(what) {}
};

class EmptyBlock : public Exception
{
public:
    EmptyBlock(const std::string& what = "")
        : Exception(what) {}
};

class FileException : public Exception
{
public:
    FileException(const std::string& what = "")
        : Exception(what) {}
};
