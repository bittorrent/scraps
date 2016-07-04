#pragma once

#include "scraps/config.h"

SCRAPS_IGNORE_WARNINGS_PUSH
#include <boost/program_options.hpp>
SCRAPS_IGNORE_WARNINGS_POP

#include "scraps/stdts/optional.h"

#include <unordered_map>

namespace scraps {

/**
 * Not thread safe.
 *
 * Easily binds arguments to the parser.
 *
 * Usage:
 *
 *    bool flag = false;
 *    std::string str;
 *
 *    ArgumentParser parser{"some description"};
 *    parser.addFlag("help", "help me", flag);
 *    parser.addFlag("text", "some text", text);
 *
 *    parser.parse(argc, argv);
 *
 *   // flag & str are set accordingly
 */
class ArgumentParser {
public:
    ArgumentParser(const std::string& description, bool allowUnregistered = true);

    /**
     * Parse the given arguments.
     *
     * Returns true if successful, false otherwise.
     */
    bool parse(int argc, const char* const* argv);

    /**
     * Add a --flag as opposed to a --bool=true argument.
     */
    void addFlag(const std::string& name, const std::string& description, bool* destination);
    void addFlag(const std::string& name, const std::string& description, std::function<void()> callback);

    /**
     * Add an argument and set the destination value when parsed.
     *
     * Use the original value in destination as the default.
     */
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type
        addArgument(const std::string& name, const std::string& description, T* destination);
    void addArgument(const std::string& name, const std::string& description, std::string* destination);

    /**
     * Add an argument and set the destination value when parsed.
     */
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type
        addArgument(const std::string& name, const std::string& description, T* destination,           const stdts::optional<T>& defaultValue);
    void addArgument(const std::string& name, const std::string& description, std::string* destination, const stdts::optional<std::string>& defaultValue);

    /**
     * Add an argument and specify a callback to be called when the values are parsed.
     */
    template <typename T>
    typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type
        addArgument(const std::string& name, const std::string& description, std::function<void(T)> callback,                  const stdts::optional<T>& defaultValue = {});
    void addArgument(const std::string& name, const std::string& description, std::function<void(const std::string&)> callback, const stdts::optional<std::string>& defaultValue = {});

    /**
    * @param args pairs of string representations and their corresponding enum values
    */
    template <typename T, typename... Args>
    void addEnum(const std::string& name, const std::string& description, T* destination, Args&&... args);

    template <typename T, typename... Args>
    void addEnum(const std::string& name, const std::string& description, std::function<void(const T&)> callback, Args&&... args);

    /**
     * Add a new section to the help description.
     */
    void addSection(const std::string& description);

    /**
     * Write the option description to the given stream.
     */
    void help(std::ostream& ostr) const;

private:
    void _parse(boost::program_options::command_line_parser& parser);
    void _assignValues(const boost::program_options::variables_map& vm);

    template <typename K, typename V>
    void _buildMap(std::unordered_map<K, V>* map) {}

    template <typename K, typename V, typename... Args>
    void _buildMap(std::unordered_map<K, V>* map, K key, V value, Args&&... args) {
        (*map)[key] = value;
        _buildMap<K, V>(map, std::forward<Args>(args)...);
    }

    template <typename T, typename U = typename std::remove_reference<typename std::remove_cv<T>::type>::type>
    void _addArgument(const std::string& name, const std::string& description, std::function<void(T)> callback, const stdts::optional<U>& defaultValue);

    template <typename T>
    std::unordered_map<std::string, std::function<void(T)>>& _data();

    bool _parsed = false;
    bool _allowUnregistered = true;
    std::vector<boost::program_options::options_description> _options;

    std::unordered_map<std::string, std::function<void()>>                   _flags;
    std::unordered_map<std::string, std::function<void(bool)>>               _bools;
    std::unordered_map<std::string, std::function<void(short)>>              _shorts;
    std::unordered_map<std::string, std::function<void(unsigned short)>>     _unsignedShorts;
    std::unordered_map<std::string, std::function<void(int)>>                _ints;
    std::unordered_map<std::string, std::function<void(unsigned)>>           _unsignedInts;
    std::unordered_map<std::string, std::function<void(long)>>               _longs;
    std::unordered_map<std::string, std::function<void(unsigned long)>>      _unsignedLongs;
    std::unordered_map<std::string, std::function<void(long long)>>          _longLongs;
    std::unordered_map<std::string, std::function<void(unsigned long long)>> _unsignedLongLongs;
    std::unordered_map<std::string, std::function<void(float)>>              _floats;
    std::unordered_map<std::string, std::function<void(double)>>             _doubles;
    std::unordered_map<std::string, std::function<void(long double)>>        _longDoubles;
    std::unordered_map<std::string, std::function<void(const std::string&)>> _strings;
};

/**
 * Some reusable arguments.
 */
struct CommonArguments {
    bool help = false;
    bool version = false;
    std::string logName;

    /**
     * Bind these arguments to the given parser.
     */
    void bindArguments(ArgumentParser& parser);
};

template <typename T, typename U>
void ArgumentParser::_addArgument(const std::string& name, const std::string& description, std::function<void(T)> callback, const stdts::optional<U>& defaultValue) {
    auto value = boost::program_options::value<U>();
    if (defaultValue) {
        value->default_value(*defaultValue);
    }
    _options.back().add_options()(name.c_str(), value, description.c_str());
    _data<T>().emplace(name, callback);
}

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type ArgumentParser::addArgument(const std::string& name, const std::string& description, T* destination) {
    using U = typename std::remove_reference<typename std::remove_cv<T>::type>::type;
    _addArgument<T>(name, description, [=] (T value) { *destination = value; }, stdts::optional<U>{*destination});
}

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type ArgumentParser::addArgument(const std::string& name, const std::string& description, std::function<void(T)> callback, const stdts::optional<T>& defaultValue) {
    _addArgument<T>(name, description, callback, defaultValue);
}

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value || std::is_floating_point<T>::value, void>::type ArgumentParser::addArgument(const std::string& name, const std::string& description, T* destination, const stdts::optional<T>& defaultValue) {
    _addArgument<T>(name, description, [=] (T value) { *destination = value; }, defaultValue);
}

inline void ArgumentParser::addArgument(const std::string& name, const std::string& description, std::string* destination) {
    _addArgument<const std::string&>(name, description, [=] (const std::string& value) { *destination = value; }, stdts::optional<std::string>{*destination});
}

inline void ArgumentParser::addArgument(const std::string& name, const std::string& description, std::string* destination, const stdts::optional<std::string>& defaultValue) {
    _addArgument<const std::string&>(name, description, [=] (const std::string& value) { *destination = value; }, defaultValue);
}

inline void ArgumentParser::addArgument(const std::string& name, const std::string& description, std::function<void(const std::string&)> callback, const stdts::optional<std::string>& defaultValue) {
    _addArgument<const std::string&>(name, description, callback, defaultValue);
}

template <typename T, typename... Args>
void ArgumentParser::addEnum(const std::string& name, const std::string& description, T* destination, Args&&... args) {
    addEnum(name, description, std::function<void(const T&)>{[=] (const T& value) { *destination = value; }}, std::forward<Args>(args)...);
}

template <typename T, typename... Args>
void ArgumentParser::addEnum(const std::string& name, const std::string& description, std::function<void(const T&)> callback, Args&&... args) {
    std::unordered_map<std::string, T> map;
    _buildMap<std::string, T>(&map, std::forward<Args>(args)...);
    auto value = boost::program_options::value<std::string>()->notifier([=] (const std::string& value) {
        if (!map.count(value)) {
            throw boost::program_options::validation_error::invalid_option_value;
        }
        callback(map.at(value));
    });
    _options.back().add_options()(name.c_str(), value, description.c_str());
}

} // namespace scraps
