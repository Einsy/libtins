/*
 * Copyright (c) 2012, Nasel
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
#ifndef TINS_HWADDRESS_H
#define TINS_HWADDRESS_H

#include <stdint.h>
#include <stdexcept>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Tins {
/**
 * \class HWAddress
 * \brief Represents a hardware address.
 */
template<size_t n, typename Storage = uint8_t>
class HWAddress {
public:
    /**
     * \brief The type of the elements stored in the hardware address.
     * 
     * This is the same as the template parameter Storage.
     */
    typedef Storage storage_type;
    
    /**
     * \brief The random access iterator type.
     */
    typedef storage_type* iterator;
    
    /**
     * \brief Const iterator type.
     */
    typedef const storage_type* const_iterator;
    
    /**
     * \brief Non-member constant indicating the amount of storage_type
     * elements in this address.
     */
    static const size_t address_size = n;
    
    /**
     * \brief Constructor from a const storage_type*.
     * 
     * If no pointer or a null pointer is provided, the address is 
     * initialized to 00:00:..... 
     * This constructor is very usefull when passing zero initialized
     * addresses as arguments to other functions. You can use a 
     * literal 0, which will be implicitly converted to the empty address.
     * 
     * If a pointer is provided, address_size storage_type elements 
     * are copied from the pointer, into the internal address representation.
     * 
     * \param ptr The pointer from which to construct this address.
     */
    HWAddress(const storage_type* ptr = 0) {
        if(ptr)
            std::copy(ptr, ptr + address_size, buffer);
        else
            std::fill(begin(), end(), storage_type());
    }
    
    /**
     * \brief Constructs an address from a hex-notation address.
     * 
     * This constructor will parse strings in the form:
     * 
     * "00:01:da:fa:..."
     * 
     * And initialize the internal representation accordingly.
     * 
     * \param address The hex-notation address to be parsed.
     */
    HWAddress(const std::string &address) {
        convert(address, buffer);
    }
    
    /**
     * \brief Overload provided basically for string literals.
     * 
     * This constructor takes a const char array of i elements in
     * hex-notation. \sa HWAddress::HWAddress(const std::string &address)
     * 
     * This is mostly used when providing string literals. If this where 
     * a const char*, then there would be an ambiguity when providing 
     * a null pointer. 
     * 
     * \param address The array of chars containing the hex-notation
     * cstring to be parsed.
     */
    template<size_t i>
    HWAddress(const char (&address)[i]) {
        convert(address, buffer);
    }
    
    /**
     * \brief Copy construct from a HWAddress of length i.
     * 
     * If i is lower or equal than address_size, then i storage_type 
     * elements are copied, and the last (n - i) are initialized to 
     * the default storage_type value(0 most of the times).
     * 
     * If i is larger than address_size, then only the first address_size 
     * elements are copied.
     * 
     * \param rhs The HWAddress to be constructed from.
     */
    template<size_t i>
    HWAddress(const HWAddress<i> &rhs) {
        std::copy(
            rhs.begin(),
            rhs.begin() + std::min(i, n),
            begin()
        );
    }
    
    /**
     * \brief Retrieves an iterator pointing to the begining of the 
     * address.
     * 
     * \return iterator.
     */
    iterator begin() {
        return buffer;
    }
    
    /**
     * \brief Retrieves a const iterator pointing to the begining of 
     * the address.
     * 
     * \return const_iterator.
     */
    const_iterator begin() const {
        return buffer;
    }
    
    /**
     * \brief Retrieves an iterator pointing one-past-the-end of the 
     * address.
     * 
     * \return iterator.
     */
    iterator end() {
        return buffer + address_size;
    }

    /**
     * \brief Retrieves a const iterator pointing one-past-the-end of 
     * the address.
     * 
     * \return const_iterator.
     */
    const_iterator end() const {
        return buffer + address_size;
    }
    
    /**
     * \brief Compares this HWAddress for equality.
     * 
     * \param rhs The HWAddress to be compared to.
     * 
     * \return bool indicating whether addresses are equal.
     */
    bool operator==(const HWAddress &rhs) const {
        return std::equal(begin(), end(), rhs.begin());
    }
    
    /**
     * \brief Compares this HWAddress for in-equality.
     * 
     * \param rhs The HWAddress to be compared to.
     * 
     * \return bool indicating whether addresses are distinct.
     */
    bool operator!=(const HWAddress &rhs) const {
        return !(*this == rhs);
    }
    
    /**
     * \brief Retrieves the size of this address.
     * 
     * This effectively returns the address_size constant.
     */
    const size_t size() const {
        return address_size;
    }
    
    /**
     * \brief Convert this address to a hex-notation std::string address.
     * 
     * \return std::string containing the hex-notation address.
     */
    std::string to_string() const {
        std::ostringstream oss;
        oss << *this;
        return oss.str();
    }
    
    /**
     * \brief Writes this HWAddress in hex-notation to a std::ostream.
     * 
     * \param os The stream in which to write the address.
     * \param addr The parameter to be written.
     * \return std::ostream& pointing to the os parameter.
     */
    friend std::ostream &operator<<(std::ostream &os, const HWAddress &addr) {
        std::transform(
            addr.begin(), 
            addr.end() - 1,
            std::ostream_iterator<std::string>(os, ":"),
            &HWAddress::storage_to_string
        );
        return os << storage_to_string(addr.buffer[HWAddress::address_size-1]);
    }
    
    /**
     * \brief Helper function which copies the address into an output
     * iterator.
     * 
     * This is the same as:
     * 
     * std::copy(begin(), end(), iter);
     * 
     * But since some PDUs return a HWAddress<> by value, this function
     * can be used to avoid temporaries. 
     * 
     * \param iter The output iterator in which to store this address.
     * \return OutputIterator pointing to one-past the last position
     * written.
     */
    template<typename OutputIterator>
    OutputIterator copy(OutputIterator iter) const {
        return std::copy(begin(), end(), iter);
    }
private:
    template<typename OutputIterator>
    static void convert(const std::string &hw_addr, OutputIterator output);
    
    static std::string storage_to_string(storage_type element) {
        std::ostringstream oss;
        oss << std::hex;
        if(element < 0x10)
            oss << '0';
        oss << (unsigned)element;
        return oss.str();
    }

    storage_type buffer[n];
};

template<size_t n, typename Storage>
template<typename OutputIterator>
void HWAddress<n, Storage>::convert(const std::string &hw_addr, 
  OutputIterator output) 
{
    unsigned i(0);
    storage_type tmp;
    while(i < hw_addr.size()) {
        const unsigned end = i+2;
        tmp = storage_type();
        while(i < end) {
            if(hw_addr[i] >= 'a' && hw_addr[i] <= 'f')
                tmp = (tmp << 4) | (hw_addr[i] - 'a' + 10);
            else if(hw_addr[i] >= '0' && hw_addr[i] <= '9')
                tmp = (tmp << 4) | (hw_addr[i] - '0');
            else if(hw_addr[i] == ':')
                break;
            else
                throw std::runtime_error("Invalid byte found");
            i++;
        }
        *(output++) = tmp;
        if(i < hw_addr.size()) {
            if(hw_addr[i] == ':')
                i++;
            else
                throw std::runtime_error("Invalid separator");
        }
    }
}
}
#endif // TINS_HWADDRESS_H
