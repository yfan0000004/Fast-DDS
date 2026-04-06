#ifndef GATEWAY_JSON_H
#define GATEWAY_JSON_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <stdexcept>

namespace gateway {

// Minimal JSON value type for config parsing (C++11, no dependencies).
// Supports: string, number(int), bool, object, array. Enough for config files.
class JsonValue {
public:
    enum Type { Null, String, Number, Bool, Object, Array };

    JsonValue() : type_(Null), num_(0), bool_(false) {}

    Type type() const { return type_; }

    const std::string& str() const { return str_; }
    int num() const { return num_; }
    bool boolean() const { return bool_; }

    const JsonValue& operator[](const std::string& key) const {
        static JsonValue null_val;
        auto it = obj_.find(key);
        return (it != obj_.end()) ? it->second : null_val;
    }

    const JsonValue& operator[](size_t idx) const {
        static JsonValue null_val;
        return (idx < arr_.size()) ? arr_[idx] : null_val;
    }

    size_t size() const { return (type_ == Array) ? arr_.size() : obj_.size(); }
    const std::vector<JsonValue>& arr() const { return arr_; }
    const std::map<std::string, JsonValue>& obj() const { return obj_; }

    static JsonValue parse(const std::string& text) {
        size_t pos = 0;
        return parse_value(text, pos);
    }

    static JsonValue from_file(const std::string& path) {
        std::ifstream f(path.c_str());
        if (!f.is_open()) throw std::runtime_error("cannot open: " + path);
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        return parse(content);
    }

private:
    Type type_;
    std::string str_;
    int num_;
    bool bool_;
    std::map<std::string, JsonValue> obj_;
    std::vector<JsonValue> arr_;

    static void skip_ws(const std::string& s, size_t& p) {
        while (p < s.size() && (s[p]==' '||s[p]=='\n'||s[p]=='\r'||s[p]=='\t')) ++p;
    }

    static JsonValue parse_value(const std::string& s, size_t& p) {
        skip_ws(s, p);
        if (p >= s.size()) return JsonValue();
        if (s[p] == '"') return parse_string(s, p);
        if (s[p] == '{') return parse_object(s, p);
        if (s[p] == '[') return parse_array(s, p);
        if (s[p] == 't' || s[p] == 'f') return parse_bool(s, p);
        if (s[p] == 'n') { p += 4; return JsonValue(); }
        return parse_number(s, p);
    }

    static JsonValue parse_string(const std::string& s, size_t& p) {
        JsonValue v; v.type_ = String;
        ++p; // skip "
        while (p < s.size() && s[p] != '"') {
            if (s[p] == '\\') { ++p; v.str_ += s[p]; }
            else { v.str_ += s[p]; }
            ++p;
        }
        ++p; // skip closing "
        return v;
    }

    static JsonValue parse_number(const std::string& s, size_t& p) {
        JsonValue v; v.type_ = Number;
        size_t start = p;
        if (s[p] == '-') ++p;
        while (p < s.size() && s[p] >= '0' && s[p] <= '9') ++p;
        v.num_ = std::stoi(s.substr(start, p - start));
        return v;
    }

    static JsonValue parse_bool(const std::string& s, size_t& p) {
        JsonValue v; v.type_ = Bool;
        if (s[p] == 't') { v.bool_ = true; p += 4; }
        else { v.bool_ = false; p += 5; }
        return v;
    }

    static JsonValue parse_object(const std::string& s, size_t& p) {
        JsonValue v; v.type_ = Object;
        ++p; // skip {
        skip_ws(s, p);
        while (p < s.size() && s[p] != '}') {
            JsonValue key = parse_string(s, p);
            skip_ws(s, p);
            ++p; // skip :
            v.obj_[key.str_] = parse_value(s, p);
            skip_ws(s, p);
            if (s[p] == ',') ++p;
            skip_ws(s, p);
        }
        ++p; // skip }
        return v;
    }

    static JsonValue parse_array(const std::string& s, size_t& p) {
        JsonValue v; v.type_ = Array;
        ++p; // skip [
        skip_ws(s, p);
        while (p < s.size() && s[p] != ']') {
            v.arr_.push_back(parse_value(s, p));
            skip_ws(s, p);
            if (s[p] == ',') ++p;
            skip_ws(s, p);
        }
        ++p; // skip ]
        return v;
    }
};

} // namespace gateway

#endif // GATEWAY_JSON_H
