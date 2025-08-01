#include "md5.h"

MD5::MD5() {}

static std::string bytesToHexString(const Byte* input, size_t length)
{
	constexpr char HEX[16] = { '0', '1', '2', '3','4', '5', '6', '7','8', '9', 'a', 'b','c', 'd', 'e', 'f' };

	std::string str;
	str.reserve(length << 1);
	for (size_t i = 0; i < length; i++)
	{
		int a = input[i] / 16;
		int b = input[i] % 16;
		str.append(1, HEX[a]);
		str.append(1, HEX[b]);
	}
	return str;
}

/* Encodes input (ulong) into output (byte). Assumes length is a multiple of 4. */
static void encode(const u32* input, size_t length, Byte* output)
{
	for (size_t i = 0, j = 0; i < length; i++, j += 4)
	{
		output[j] = (Byte)(input[i] & 0x00ff);
		output[j + 1] = (Byte)((input[i] >> 8) & 0x00ff);
		output[j + 2] = (Byte)((input[i] >> 16) & 0x00ff);
		output[j + 3] = (Byte)((input[i] >> 24) & 0x00ff);
	}
}

/* Return the message-digest */
std::string MD5::digest(const std::string& file)
{
	std::ifstream in(file, std::ios::binary);
	if (!in)
	{
		return "";
	}

	m_count[0] = m_count[1] = 0; // reset number of bits.

	/* magic initialization constants. */
	m_state[0] = 0x67452301;
	m_state[1] = 0xefcdab89;
	m_state[2] = 0x98badcfe;
	m_state[3] = 0x10325476;

	char buffer[1024];
	while (!in.eof())
	{
		in.read(buffer, 1024);
		size_t length = in.gcount();
		if (length > 0)
		{
			update((const Byte*)buffer, length);
		}
	}
	in.close();

	final();

	/* Store state in digest */
	Byte digest[16];
	encode(m_state, 4, digest);

	return bytesToHexString(digest, sizeof(digest));
}

std::string MD5::digest(const std::vector<uint8_t>& data)
{
	m_count[0] = m_count[1] = 0; // reset number of bits.

	/* magic initialization constants. */
	m_state[0] = 0x67452301;
	m_state[1] = 0xefcdab89;
	m_state[2] = 0x98badcfe;
	m_state[3] = 0x10325476;

	update(data.data(), data.size());
	final();

	/* Store state in digest */
	Byte digest[16];
	encode(m_state, 4, digest);

	return bytesToHexString(digest, sizeof(digest));
}

/* MD5 block update operation. Continues an MD5 message-digest
operation, processing another message block, and updating the context. */
void MD5::update(const Byte* input, size_t length)
{
	/* Compute number of bytes mod 64 */
	u32 index = (u32)((m_count[0] >> 3) & 0x3f);

	/* update number of bits */
	if ((m_count[0] += ((u32)length << 3)) < ((u32)length << 3))
	{
		m_count[1]++;
	}

	m_count[1] += ((u32)length >> 29);

	u32 i = 0;
	u32 partLen = 64 - index;
	if (length >= partLen)
	{
		memcpy(&m_buffer[index], input, partLen);
		transform(m_buffer);

		for (i = partLen; i + 63 < length; i += 64)
		{
			transform(&input[i]);
		}
		index = 0;
	}

	memcpy(&m_buffer[index], &input[i], length - i);
}

/* MD5 finalization. Ends an MD5 message-_digest operation, writing the the message _digest and zeroizing the context. */
void MD5::final()
{
	/* Save number of bits */
	Byte bits[8];
	encode(m_count, 2, bits);

	/* Pad out to 56 mod 64. */
	u32 index = (u32)((m_count[0] >> 3) & 0x3f);
	u32 padLen = (index < 56) ? (56 - index) : (120 - index);
	const Byte PADDING[64] = { 0x80 };
	update(PADDING, padLen);

	/* Append length (before padding) */
	update(bits, 8);
}

inline static u32 rotate_left(u32 x, int n) { return (x << n) | (x >> (32 - n)); }

inline static u32 F(u32 x, u32 y, u32 z) { return (((x) & (y)) | ((~x) & (z))); }
inline static u32 G(u32 x, u32 y, u32 z) { return (((x) & (z)) | ((y) & (~z))); }
inline static u32 H(u32 x, u32 y, u32 z) { return ((x) ^ (y) ^ (z)); }
inline static u32 I(u32 x, u32 y, u32 z) { return ((y) ^ ((x) | (~z))); }

inline static void FF(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac)
{
	a = rotate_left(a + F(b, c, d) + x + ac, s) + b;
}

inline static void GG(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac)
{
	a = rotate_left(a + G(b, c, d) + x + ac, s) + b;
}

inline static void HH(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac)
{
	a = rotate_left(a + H(b, c, d) + x + ac, s) + b;
}

inline static void II(u32& a, u32 b, u32 c, u32 d, u32 x, u32 s, u32 ac)
{
	a = rotate_left(a + I(b, c, d) + x + ac, s) + b;
}

/* Decodes input (byte) into output (ulong). Assumes length is a multiple of 4. */
static void decode(const Byte* input, size_t length, u32* output)
{
	for (size_t i = 0, j = 0; j < length; i++, j += 4)
	{
		output[i] = ((u32)input[j]) | (((u32)input[j + 1]) << 8) | (((u32)input[j + 2]) << 16) | (((u32)input[j + 3]) << 24);
	}
}

/* MD5 basic transformation. Transforms _state based on block. */
void MD5::transform(const Byte block[64])
{
	u32 x[16];
	decode(block, 64, x);

	u32 a = m_state[0], b = m_state[1], c = m_state[2], d = m_state[3];

	constexpr u32 S11 = 7, S12 = 12, S13 = 17, S14 = 22;
	constexpr u32 S21 = 5, S22 = 9, S23 = 14, S24 = 20;
	constexpr u32 S31 = 4, S32 = 11, S33 = 16, S34 = 23;
	constexpr u32 S41 = 6, S42 = 10, S43 = 15, S44 = 21;

	/* Round 1 */
	FF(a, b, c, d, x[0], S11, 0xd76aa478);  /* 1 */
	FF(d, a, b, c, x[1], S12, 0xe8c7b756);  /* 2 */
	FF(c, d, a, b, x[2], S13, 0x242070db);  /* 3 */
	FF(b, c, d, a, x[3], S14, 0xc1bdceee);  /* 4 */
	FF(a, b, c, d, x[4], S11, 0xf57c0faf);  /* 5 */
	FF(d, a, b, c, x[5], S12, 0x4787c62a);  /* 6 */
	FF(c, d, a, b, x[6], S13, 0xa8304613);  /* 7 */
	FF(b, c, d, a, x[7], S14, 0xfd469501);  /* 8 */
	FF(a, b, c, d, x[8], S11, 0x698098d8);  /* 9 */
	FF(d, a, b, c, x[9], S12, 0x8b44f7af);  /* 10 */
	FF(c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
	FF(b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
	FF(a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
	FF(d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
	FF(c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
	FF(b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

	/* Round 2 */
	GG(a, b, c, d, x[1], S21, 0xf61e2562);  /* 17 */
	GG(d, a, b, c, x[6], S22, 0xc040b340);  /* 18 */
	GG(c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
	GG(b, c, d, a, x[0], S24, 0xe9b6c7aa);  /* 20 */
	GG(a, b, c, d, x[5], S21, 0xd62f105d);  /* 21 */
	GG(d, a, b, c, x[10], S22, 0x2441453);  /* 22 */
	GG(c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
	GG(b, c, d, a, x[4], S24, 0xe7d3fbc8);  /* 24 */
	GG(a, b, c, d, x[9], S21, 0x21e1cde6);  /* 25 */
	GG(d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
	GG(c, d, a, b, x[3], S23, 0xf4d50d87);  /* 27 */
	GG(b, c, d, a, x[8], S24, 0x455a14ed);  /* 28 */
	GG(a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
	GG(d, a, b, c, x[2], S22, 0xfcefa3f8);  /* 30 */
	GG(c, d, a, b, x[7], S23, 0x676f02d9);  /* 31 */
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

	/* Round 3 */
	HH(a, b, c, d, x[5], S31, 0xfffa3942);  /* 33 */
	HH(d, a, b, c, x[8], S32, 0x8771f681);  /* 34 */
	HH(c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
	HH(b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
	HH(a, b, c, d, x[1], S31, 0xa4beea44);  /* 37 */
	HH(d, a, b, c, x[4], S32, 0x4bdecfa9);  /* 38 */
	HH(c, d, a, b, x[7], S33, 0xf6bb4b60);  /* 39 */
	HH(b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
	HH(a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
	HH(d, a, b, c, x[0], S32, 0xeaa127fa);  /* 42 */
	HH(c, d, a, b, x[3], S33, 0xd4ef3085);  /* 43 */
	HH(b, c, d, a, x[6], S34, 0x4881d05);   /* 44 */
	HH(a, b, c, d, x[9], S31, 0xd9d4d039);  /* 45 */
	HH(d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
	HH(b, c, d, a, x[2], S34, 0xc4ac5665);  /* 48 */

	/* Round 4 */
	II(a, b, c, d, x[0], S41, 0xf4292244);  /* 49 */
	II(d, a, b, c, x[7], S42, 0x432aff97);  /* 50 */
	II(c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
	II(b, c, d, a, x[5], S44, 0xfc93a039);  /* 52 */
	II(a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
	II(d, a, b, c, x[3], S42, 0x8f0ccc92);  /* 54 */
	II(c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
	II(b, c, d, a, x[1], S44, 0x85845dd1);  /* 56 */
	II(a, b, c, d, x[8], S41, 0x6fa87e4f);  /* 57 */
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
	II(c, d, a, b, x[6], S43, 0xa3014314);  /* 59 */
	II(b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
	II(a, b, c, d, x[4], S41, 0xf7537e82);  /* 61 */
	II(d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
	II(c, d, a, b, x[2], S43, 0x2ad7d2bb);  /* 63 */
	II(b, c, d, a, x[9], S44, 0xeb86d391);  /* 64 */

	m_state[0] += a;
	m_state[1] += b;
	m_state[2] += c;
	m_state[3] += d;
}

std::string FileDigest(const std::string& file)
{
	return MD5().digest(file);
}