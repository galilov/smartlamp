//
// Created by Alexander Galilov <alexander.galilov@gmail.com>
// on 11/16/19.
//

#ifndef SMART_LAMP_FONT_H
#define SMART_LAMP_FONT_H

#include <stdint.h>
#include <vector>
#include <map>

class FontGlyph {
private:
    std::vector<bool> _data;
    int _w, _h;

public:
    FontGlyph() : _w(0), _h(0) {}

    void setSize(int w, int h);

    ~FontGlyph();

    inline bool at(int row, int col) const {
        return _data[row * _w + col];
    }

    inline void at(int row, int col, bool newValue) {
        _data[row * _w + col] = newValue;
    }

    void clear();

    int getWidth() const { return _w; }

    int getHeight() const { return _h; }

    void dump() const;

};

class Font {
private:
    std::map<int, FontGlyph> _glyphs;
private:
    int _getParamIntValue(const char *line);

public:
    Font();

    bool load(const char *fileName);

    ~Font();

    const std::map<int, FontGlyph> &getGlyphs() const;

private:
    Font(const Font &);

    Font &operator=(Font &);
};


#endif //SMART_LAMP_FONT_H
