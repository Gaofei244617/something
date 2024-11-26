#pragma once

#include <string>
#include <fstream>

using Byte = unsigned char;
using u32 = unsigned int;

class MD5
{
public:
	MD5();
	std::string digest(const std::string& file);

private:
	void update(const Byte* input, size_t length);
	void final();
	void transform(const Byte block[64]);

private:
	u32 m_state[4] = { 0 };      // state (ABCD)
	u32 m_count[2] = { 0 };      // number of bits, modulo 2^64 (low-order word first)
	Byte m_buffer[64] = { 0 };   // input buffer
};

std::string FileDigest(const std::string& file);
