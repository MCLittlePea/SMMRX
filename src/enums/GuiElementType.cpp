#include "GuiElementType.hpp"

const map<string, GuiElementType> GuiElementTypeInfo::strMap = {
    {"NUMBER_FONT", NUMBER_FONT}
};

GuiElementType GuiElementTypeInfo::parse(const string& s) {
    auto it = strMap.find(s);
    return it != strMap.end() ? it->second : GUI_ELEMENT_ERROR;
}
