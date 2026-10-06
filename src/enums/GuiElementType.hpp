#ifndef ENUMS_GUI_ELEMENT_TYPE_HPP
#define ENUMS_GUI_ELEMENT_TYPE_HPP

#include <map>
#include <string>

using namespace std;

enum GuiElementType {
    NUMBER_FONT,
    GUI_ELEMENT_TYPE_COUNT,
    GUI_ELEMENT_ERROR
};

class GuiElementTypeInfo {
public:
    static const map<string, GuiElementType> strMap;
    static GuiElementType parse(const string& s);
};

#endif
