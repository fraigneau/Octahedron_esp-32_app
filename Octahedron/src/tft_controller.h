#pragma once

class TFTController {
public:
	static TFTController* Singleton();
	bool Initialize();
	void setActive();
	// debug
public:
	void drawLine(int line_number, uint16_t* line_data);
};