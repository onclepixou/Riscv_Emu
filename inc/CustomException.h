#ifndef CUSTOM_EXCEPTION_H
#define CUSTOM_EXCEPTION_H

#include <exception>
#include <string>
#include <cstdint>


class DecodeException: public std::exception {

    public:


        explicit DecodeException(uint8_t op);

        virtual ~DecodeException() noexcept {}

        virtual const char* what() const noexcept { return msg_.c_str();}

    protected:

        std::string msg_;
};

#endif