#pragma once

#include <cstdlib>
#include <map>
#include <GL/glew.h>

#include "prog.h"
#include "layout.h"

class Console {
	private:
		char
			* _canv,

			* _data;

		std::string _buffName;

		std::vector<std::string> _buff;

		std::string _cwd;

		std::vector<std::map<std::string, std::string>> _tree;

		const std::string _ps1 = "[] ";
		std::string _prompt;

		unsigned int
			_cursEditor[2][2],
			_cursPrompt[2],
			_cursFs;

		bool
			_rngEditor,
			_rngPrompt;

		unsigned int _mode;

		time_t _raw;
		tm* _info;
		char _timeFmt[64];

		GLfloat
			_vtc[2 * 2 * 2] = {
				-1.0, -1.0,
				1.0, -1.0,
				-1.0, 1.0,
				1.0, 1.0
			},
			_st[2 * 2 * 2] = {
				0.0, 0.0,
				1.0, 0.0,
				0.0, 1.0,
				1.0, 1.0
			};

		GLushort _idc[2 * 3] = {
			0, 1, 2,
			2, 1, 3
		};

		GLuint
			_vao,

			_vbo,
			_stbo,

			_ibo;

		GLint
			_attrPos,
			_attrSt;

		GLuint _tex;

		Prog _prog;

		enum id {
			POS,
			ST
		};

	public:
		unsigned int _res[2];

		Console(std::string fName, std::string cwd, unsigned int res[2]);

		void clear();

		void fmtBuff(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr);

		void fmt();

		void render();

		void open(std::string fName);

		void changeDir(std::string dir);

		void print(char c, bool b, Coord st);

		void draw();

		unsigned int idxStatic(Coord st, unsigned int bound[2]);

		static bool numeric(std::map<std::string, std::string> lhs, std::map<std::string, std::string> rhs);
		
		enum mode {
			EDITOR,
			PROMPT,
			FS
		};
};
