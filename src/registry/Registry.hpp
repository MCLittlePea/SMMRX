#ifndef REGISTRY_HPP
#define REGISTRY_HPP

#include <map>
#include <string>
#include <stdexcept>

using namespace std;

template<typename T>
class Registry {
public:
    static void registerItem(const string& id, T item) {
        items()[id] = item;
    }

    static T& get(const string& id) {
        auto it = items().find(id);
        if (it == items().end()) {
            throw runtime_error("Registry: item not found: " + id);
        }
        return it->second;
    }

    static bool has(const string& id) {
        return items().find(id) != items().end();
    }

    static const map<string, T>& getAll() {
        return items();
    }

    static void remove(const string& id) {
        items().erase(id);
    }

    static void clear() {
        items().clear();
    }

private:
    static map<string, T>& items() {
        static map<string, T> instance;
        return instance;
    }
};

#endif
