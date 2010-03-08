/* $Id$ */

/*
 * Copyright (c) 2010 SURFnet bv
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*****************************************************************************
 ByteString.cpp

 A string class for byte strings stored in securely allocated memory
 *****************************************************************************/

#include <algorithm>
#include <string>
#include "config.h"
#include "log.h"
#include "ByteString.h"

// Constructors
ByteString::ByteString(size_t initialSize /* = 0 */)
{
	byteString.resize(initialSize);
}

ByteString::ByteString(const unsigned char* bytes, const size_t bytesLen)
{
	byteString.resize(bytesLen);

	memcpy(&byteString[0], bytes, bytesLen);
}

ByteString::ByteString(const char* hexString)
{
	std::string hex = std::string(hexString);

	if (hex.size() % 2 != 0)
	{
		hex = "0" + hex;
	}

	for (size_t i = 0; i < hex.size(); i += 2)
	{
		std::string byteStr;
		byteStr += hex[i];
		byteStr += hex[i+1];

		unsigned char byteVal = (unsigned char) strtoul(byteStr.c_str(), NULL, 16);

		this->operator+=(byteVal);
	}
}

ByteString::ByteString(const ByteString& in)
{
	this->byteString = in.byteString;
}

// Append data
ByteString& ByteString::operator+=(const ByteString& append)
{
	size_t curLen = byteString.size();
	size_t toAdd = append.byteString.size();
	size_t newLen = curLen + toAdd;

	byteString.resize(newLen);

	memcpy(&byteString[curLen], &append.byteString[0], toAdd);

	return *this;
}

ByteString& ByteString::operator+=(const unsigned char byte)
{
	byteString.push_back(byte);

	return *this;
}

// Return a substring
ByteString ByteString::substr(const size_t start, const size_t len /* = SIZE_T_MAX */) const
{
	size_t retLen = std::min(len, byteString.size() - start);

	if (start >= byteString.size())
	{
		return ByteString();
	}
	else
	{
		return ByteString(&byteString[start], retLen);
	}
}

// Add data
ByteString operator+(const ByteString& lhs, const ByteString& rhs)
{
	ByteString rv = lhs;
	rv += rhs;

	return rv;
}

ByteString operator+(const unsigned char lhs, const ByteString& rhs)
{
	ByteString rv(&lhs, 1);
	rv += rhs;

	return rv;
}

ByteString operator+(const ByteString& lhs, const unsigned char rhs)
{
	ByteString rv = lhs;
	rv += rhs;

	return rv;
}

// Array operator
unsigned char& ByteString::operator[](size_t pos)
{
	return byteString[pos];
}

// Return the byte string data
unsigned char* ByteString::byte_str()
{
	return &byteString[0];
}

// Return the const byte string
const unsigned char* ByteString::const_byte_str() const
{
	return (const unsigned char*) &byteString[0];
}

// The size of the byte string
size_t ByteString::size() const
{
	return byteString.size();
}

void ByteString::resize(const size_t newSize)
{
	byteString.resize(newSize);
}

void ByteString::wipe(const size_t newSize /* = 0 */)
{
	this->resize(newSize);

	memset(&byteString[0], 0x00, byteString.size());
}

// Comparison
bool ByteString::operator==(const ByteString& compareTo) const
{
	if (compareTo.size() != this->size())
	{
		return false;
	}

	return (memcmp(&byteString[0], &compareTo.byteString[0], this->size()) == 0);
}

bool ByteString::operator!=(const ByteString& compareTo) const
{
	if (compareTo.size() != this->size())
	{
		return true;
	}

	return (memcmp(&byteString[0], &compareTo.byteString[0], this->size()) != 0);
}

// XOR data
ByteString operator^(const ByteString& lhs, const ByteString& rhs)
{
	size_t xorLen = std::min(lhs.size(), rhs.size());
	ByteString rv;

	for (size_t i = 0; i < xorLen; i++)
	{
		rv += lhs.const_byte_str()[i] ^ rhs.const_byte_str()[i];
	}

	return rv;
}
