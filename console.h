#pragma once

#include <cstdlib>
#include <map>
#include <GL/glew.h>

#include "prog.h"
#include "layout.h"

class Console {
	private:
		char* _canv;
		char* _hl;

		char* _data;
		char* _blank;

		unsigned char _block[100][100][3];

		std::string _buffName;

		const std::string _ps1 = "[] ";

		GLfloat _vtc[2 * 2 * 2] = {
			-1.0, -1.0,
			1.0, -1.0,
			-1.0, 1.0,
			1.0, 1.0
		};
		GLfloat _st[2 * 2 * 2] = {
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
		};

		GLushort _idc[2 * 3] = {
			0, 1, 2,
			2, 1, 3
		};

		GLuint _vao;

		GLuint _vbo;
		GLuint _stbo;

		GLuint _ibo;

		GLint _attrPos;
		GLint _attrSt;

		GLuint _tex;

		Prog _prog;

		enum id {
			POS,
			ST
		};

	public:
		std::string _cwd;

		std::string _prompt;

		std::vector<std::map<std::string, std::string>> _tree;

		unsigned int _mode;

		unsigned int _cursEditor[2][2];
		unsigned int _cursPrompt[2];
		unsigned int _cursFs;

		bool _rngEditor;
		bool _rngPrompt;

		std::vector<std::string> _buff;

		std::vector<std::string> _clip;

		time_t _raw;
		tm* _info;
		char _timeFmt[64];

		Console(std::string fName, std::string cwd);

		void clear();

		void fmtBuff(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr);

		void fmt();

		void render();

		void open(std::string fName);

		void changeDir(std::string dir);

		void switchMode(unsigned int mode);

		void ins(char c);

		void newLine();

		void del();

		void exec();

		void hl();

		void print(char c, bool b, Coord st);

		void draw();

		unsigned int idxStatic(Coord st, Coord bound);

		unsigned int idxDeterm(std::vector<std::string> buff, Coord st);

		unsigned int clamp(unsigned int i, unsigned int roof);

		void getTime();

		static bool lexicoEntry(std::map<std::string, std::string> lhs, std::map<std::string, std::string> rhs);
		
		enum mode {
			EDITOR,
			PROMPT,
			FS
		};
};
