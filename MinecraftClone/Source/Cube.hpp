#pragma once

#include <vector>

#include "CubeFace.hpp"

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

	static void generatePNT(std::vector<float>& vertices);

	static void generatePNTTy(std::vector<float>& vertices);
	static void generatePNTTyFace(std::vector<float>& vertices, CubeFace face);

	static void generatePNTTyLeft(std::vector<float>& vertices);
	static void generatePNTTyRight(std::vector<float>& vertices);
	static void generatePNTTyBottom(std::vector<float>& vertices);
	static void generatePNTTyTop(std::vector<float>& vertices);
	static void generatePNTTyBack(std::vector<float>& vertices);
	static void generatePNTTyFront(std::vector<float>& vertices);
};