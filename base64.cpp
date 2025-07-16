#include <string>
#include <vector>

std::string EncodeBase64(const std::vector<unsigned char>& input)
{
	static constexpr char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	const size_t len = input.size();
	std::string output((len + 2) / 3 * 4, 0); // 预分配结果内存

	// 处理完整的3字节组
	size_t cnt = (len / 3) * 3; // 计算完整的3字节组长度
	for (size_t i = 0, j = 0; i < cnt; i += 3, j += 4)
	{
		const uint32_t triple = (static_cast<uint32_t>(input[i]) << 16) | (static_cast<uint32_t>(input[i + 1]) << 8) | input[i + 2];

		output[j] = base64_chars[(triple >> 18) & 0x3F];
		output[j + 1] = base64_chars[(triple >> 12) & 0x3F];
		output[j + 2] = base64_chars[(triple >> 6) & 0x3F];
		output[j + 3] = base64_chars[triple & 0x3F];
	}

	// 处理剩余字节
	if (len % 3 == 1)
	{
		const uint32_t triple = static_cast<uint32_t>(input[len - 1]) << 16;

		const size_t j = output.size() - 4;
		output[j] = base64_chars[(triple >> 18) & 0x3F];
		output[j + 1] = base64_chars[(triple >> 12) & 0x3F];
		output[j + 2] = '=';
		output[j + 3] = '=';
	}
	else if (len % 3 == 2)
	{
		const uint32_t triple = (static_cast<uint32_t>(input[len - 2]) << 16) | (static_cast<uint32_t>(input[len - 1]) << 8);

		const size_t j = output.size() - 4;
		output[j] = base64_chars[(triple >> 18) & 0x3F];
		output[j + 1] = base64_chars[(triple >> 12) & 0x3F];
		output[j + 2] = base64_chars[(triple >> 6) & 0x3F];
		output[j + 3] = '=';
	}

	return output;
}

std::vector<unsigned char> DecodeBase64(const std::string& input)
{
	// 字符到值的映射表
	static const char base64_map[256] = {
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 0-15
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 16-31
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63, // 32-47
		52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1, // 48-63
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, // 64-79
		15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1, // 80-95
		-1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, // 96-111
		41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1, // 112-127
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 128-143
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 144-159
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 160-175
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 176-191
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 192-207
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 208-223
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 224-239
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1  // 240-255
	};

	const size_t len = input.size();
	if (len == 0)
	{
		return std::vector<unsigned char>();
	}

	// 计算有效字符数和填充数
	size_t pad_count = 0;
	if (input[len - 1] == '=')
	{
		pad_count++;
	}
	if (input[len - 2] == '=')
	{
		pad_count++;
	}

	// 计算输出长度
	const size_t output_len = (input.size() / 4) * 3 - pad_count;

	std::vector<unsigned char> output;
	output.reserve(output_len); // 预分配输出内存

	unsigned char value[4];
	size_t cnt = (len - pad_count) / 4 * 4; 
	for (size_t i = 0; i < cnt; i += 4)
	{
		value[0] = base64_map[input[i]];
		value[1] = base64_map[input[i + 1]];
		value[2] = base64_map[input[i + 2]];
		value[3] = base64_map[input[i + 3]];

		// 将4个6位值转换为3个字节
		output.push_back((value[0] << 2) | (value[1] >> 4));
		output.push_back(((value[1] & 0x0F) << 4) | (value[2] >> 2));
		output.push_back(((value[2] & 0x03) << 6) | value[3]);
	}

	// 处理末尾字节（少于24位的情况）
	if (pad_count == 1)
	{
		value[0] = base64_map[input[len - 4]];
		value[1] = base64_map[input[len - 3]];
		value[2] = base64_map[input[len - 2]];

		output.push_back((value[0] << 2) | (value[1] >> 4));
		output.push_back(((value[1] & 0x0F) << 4) | (value[2] >> 2));
	}
	else if (pad_count == 2)
	{
		value[0] = base64_map[input[len - 4]];
		value[1] = base64_map[input[len - 3]];

		output.push_back((value[0] << 2) | (value[1] >> 4));
	}

	return output;
}

bool IsValidBase64(const std::string& input)
{
	// 检查长度是否为4的倍数
	size_t len = input.length();
	if (len == 0 || len % 4 != 0)
	{
		return false;
	}

	size_t paddCnt = 0;
	if (input[len - 1] == '=')
	{
		paddCnt++;
	}
	if (input[len - 2] == '=')
	{
		paddCnt++;
	}

	if (paddCnt == 1 && input[len - 1] != '=')
	{
		return false; // 如果有一个填充字符，必须是最后一个字符
	}

	// 检查非填充字符是否在Base64字符集中
	for (int i = 0; i < len - paddCnt; i++)
	{
		const char c = input[i];
		if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '+' || c == '/'))
		{
			return false;
		}
	}

	return true;
}
