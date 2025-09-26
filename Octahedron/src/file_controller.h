#pragma once
#include <vector>
class FileController {
public:
	static FileController* Singleton();
	bool Initialize();
	bool IsFatalErrored();
	void setActive();

public:
	bool exists(uint32_t filename);
	bool createFile(uint32_t filename);
	bool appendToFile(uint32_t filename, const uint8_t* data, size_t length);
	bool overwriteToFile(uint32_t filename, const uint8_t* data, size_t length);
	bool removeFile(uint32_t filename);
	std::vector<uint16_t> readLines(uint32_t filename, uint16_t line_number, size_t line_count);
};