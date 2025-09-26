#pragma once

class TFTController {
public:
	static TFTController* Singleton();
	bool Initialize();
	void setActive();
	// debug
public:
	void turnScreen(bool on);
	void drawLine(int line_number, uint16_t* line_data);
	void fillScreen(uint16_t color);
};