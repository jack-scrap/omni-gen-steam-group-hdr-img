#pragma once

#include <cstdlib>
#include <map>
#include <GL/glew.h>

#include "prog.h"
#include "layout.h"

uint32_t epoch(uint32_t inter, void* param);

class Console {
	private:
		SDL_Surface* _map;

		char* _canv;
		char* _hl;

		char* _data;
		char* _blank;

		unsigned char _block[100][100][3];

		std::string _buffName;
		std::string _perm;

		const std::string _ps1 = "[] ";

		const std::string _home = "player";

		bool _diff = false;

		GLfloat _vtc[2 * 2][2] = {
			-1.0, -1.0,
			1.0, -1.0,
			-1.0, 1.0,
			1.0, 1.0
		};

		GLfloat _st[2 * 2][2] = {
			0.0, 0.0,
			1.0, 0.0,
			0.0, 1.0,
			1.0, 1.0
		};

		GLushort _idc[2][3] = {
			0, 1, 2,
			2, 1, 3
		};

		GLuint _id[4];

		GLint _attr[2];

		GLuint _tex;

		Prog _prog;

		enum {
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

		bool _rngEditor = false;
		bool _rngPrompt = false;

		std::vector<std::string> _buff = {
			""
		};

		std::vector<std::vector<std::string>> _hist;
		unsigned int _h = _hist.size();

		std::vector<std::string> _clip;

		time_t _raw;
		tm* _info;
		char _timeFmt[64];

		Console(std::string fName, std::string dir);

		void clear();

		void fmt(std::vector<std::string> buff, Coord loc, Coord view, Coord ptr);

		void fmtScr();

		unsigned int maxFs();

		unsigned int maxNo();

		void render();

		void open(std::string fName);

		void changeDir(std::string dir);

		void switchMode(unsigned int mode);

		void ins(char c);

		void newLine();

		void del();

		void addHist();

		void exec();

		void hl();

		void print(char c, bool b, Coord st);

		void draw();

		std::vector<std::string> log();

		void getTime();

		static bool lexicoEntry(std::map<std::string, std::string> lhs, std::map<std::string, std::string> rhs);

		static std::vector<std::string> slice(std::vector<std::string> buff, unsigned int rng[2][2]);
		
		enum {
			EDITOR,
			PROMPT,
			FS
		};
};
