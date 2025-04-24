#pragma once

#include <vector>

class Cube
{
public:
	static void generatePN(std::vector<float>& vertices);
	static void generatePNLeft(std::vector<float>& vertices);
	static void generatePNRight(std::vector<float>& vertices);
	static void generatePNBottom(std::vector<float>& vertices);
	static void generatePNTop(std::vector<float>& vertices);
	static void generatePNBack(std::vector<float>& vertices);
	static void generatePNFront(std::vector<float>& vertices);
};