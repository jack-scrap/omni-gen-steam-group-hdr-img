#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "bg.h"
#include "char.h"
#include "poly.h"
#include "col.h"

class Console {
	public:
		std::vector<std::vector<Bg>> _bg;
		std::vector<std::vector<Char>> _buff;

		Poly* bg;

		glm::vec2 _res;

		Console(std::vector<std::string> buff, unsigned int ln, glm::vec2 scr);

		void print();
};
