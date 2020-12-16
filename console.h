#pragma once

#include <vector>
#include <string>
#include <iostream>

#include "bg.h"
#include "char.h"
#include "poly.h"
#include "col.h"

enum mode {
	EDITOR,
	CMD
};

class Console {
	private:
		std::string cmd = "asdf";

	public:
		unsigned int _mode = EDITOR;

		std::vector<std::vector<Bg>> _bg;
		std::vector<std::vector<Char>> _txt;

		std::vector<std::string> _buff;

		Bg _cursor;

		Poly* bg;

		glm::vec2 _res;

		unsigned int roof;

		Console(std::vector<std::string> buff, unsigned int ln, glm::vec2 scr);

		void print();
};
