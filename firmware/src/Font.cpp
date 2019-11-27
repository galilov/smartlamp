//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/16/19.
//

#include "Font.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FontGlyph::setSize(int w, int h) {
    _w = w;
    _h = h;
    clear();
}

FontGlyph::~FontGlyph() {
    //
}

void FontGlyph::clear() {
    _data.clear();
    _data.reserve(_h * _w);
    for (auto i = 0; i < _h * _w; i++) {
        _data.emplace_back(false);
    }
}

void FontGlyph::dump() const {
    for (auto row = 0; row < _h; row++) {
        for (auto col = 0; col < _w; col++) {
            if (at(row, col)) {
                putchar('@');
            } else {
                putchar('.');
            }
        }
        printf("\n");
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int Font::_getParamIntValue(const char *line) {
    int val = 0;
    const char *sVal = strchr(line, ' ');
    if (sVal) {
        val = atoi(sVal);
    }
    return val;
}

bool Font::load(const char *fileName) {
    FILE *f = fopen(fileName, "rt");
    if (f == nullptr) {
        perror("Open font file");
        return false;
    }

    char line[128];
    int fh = 0, fw = 0, ch = 0;
    int currentSymbolLine = 0;
    _glyphs.clear();
    FontGlyph glyph;
    while (!feof(f)) {
        if (fgets(line, sizeof(line) - 1, f)) {
            if (currentSymbolLine == 0) {
                if (strstr(line, "@FH") == line) {
                    fh = _getParamIntValue(line);
                } else if (strstr(line, "@FW")) {
                    fw = _getParamIntValue(line);
                } else if (strstr(line, "@CH")) {
                    ch = _getParamIntValue(line);
                    currentSymbolLine = fh;
                    glyph.setSize(fw, fh);
                    continue;
                }
            } else { //currentSymbolLine > 0
                bool eol = false;
                for (int i = 0; i < fw; i++) {
                    char c = eol ? 0 : line[i];
                    if (c == 0) {
                        eol = true;
                        glyph.at(fh - currentSymbolLine, i, false);
                    } else {
                        glyph.at(fh - currentSymbolLine, i, c > ' ');
                    }
                }
                if (--currentSymbolLine == 0) {
                    _glyphs.emplace(ch, glyph);
                }
            }
        }
    }
    fclose(f);
    return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Font::Font() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Font::~Font() {

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::map<int, FontGlyph> &Font::getGlyphs() const {
    return _glyphs;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

