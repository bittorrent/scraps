#include "scraps/ArgumentParser.h"

#include "scraps/logging.h"

namespace po = boost::program_options;

namespace scraps {

namespace {

template <typename T, typename U = typename std::remove_reference<typename std::remove_cv<T>::type>::type>
void AssignValues(const po::variables_map& vm, const std::unordered_map<std::string, std::function<void(T)>>& data) {
    for (auto& keyValue : data) {
        if (vm.count(keyValue.first)) {
            auto value = vm[keyValue.first].template as<U>();
            keyValue.second(std::move(value));
        }
    }
}

} // anonymous namespace

ArgumentParser::ArgumentParser(const std::string& description, bool allowUnregistered)
    : _allowUnregistered{allowUnregistered}
{
    _options.emplace_back(description);
}

void ArgumentParser::addSection(const std::string& description) {
    if (_options.size() > 1) {
        _options[0].add(_options.back());
    }
    _options.emplace_back(description);
}

bool ArgumentParser::parse(int argc, const char* const* argv) {
    try {
        po::command_line_parser parser(argc, argv);

        _parse(parser);
        return true;
    } catch (const std::exception& e) {
        fprintf(stderr, "%s\n", e.what());
    }

    return false;
}

void ArgumentParser::_parse(po::command_line_parser& parser) {
    if (_options.size() > 1) {
        _options[0].add(_options.back());
        _options.resize(1);
    }

    parser.options(_options[0]);
    if (_allowUnregistered) {
        parser.allow_unregistered();
    }

    parser.style(po::command_line_style::unix_style & ~po::command_line_style::allow_guessing);

    auto parsed = parser.run();

    po::variables_map vm;
    po::store(parsed, vm);
    po::notify(vm);

    _assignValues(vm);

    _parsed = true;
}

void ArgumentParser::_assignValues(const po::variables_map& vm) {
    for (auto& value : _flags) {
        if (vm.count(value.first)) {
            value.second();
        }
    }

    AssignValues(vm, _bools);
    AssignValues(vm, _shorts);
    AssignValues(vm, _unsignedShorts);
    AssignValues(vm, _ints);
    AssignValues(vm, _unsignedInts);
    AssignValues(vm, _longs);
    AssignValues(vm, _unsignedLongs);
    AssignValues(vm, _longLongs);
    AssignValues(vm, _unsignedLongLongs);
    AssignValues(vm, _floats);
    AssignValues(vm, _doubles);
    AssignValues(vm, _longDoubles);
    AssignValues(vm, _strings);
}

void ArgumentParser::help(std::ostream& ostr) const {
    if (_parsed) {
        ostr << _options[0] << std::endl;
    } else {
        auto root = _options[0];
        root.add(_options.back());
        ostr << root << std::endl;
    }
}

template <> std::unordered_map<std::string, std::function<void(bool              )>>& ArgumentParser::_data<bool              >() { return _bools; }
template <> std::unordered_map<std::string, std::function<void(short             )>>& ArgumentParser::_data<short             >() { return _shorts; }
template <> std::unordered_map<std::string, std::function<void(unsigned short    )>>& ArgumentParser::_data<unsigned short    >() { return _unsignedShorts; }
template <> std::unordered_map<std::string, std::function<void(int               )>>& ArgumentParser::_data<int               >() { return _ints; }
template <> std::unordered_map<std::string, std::function<void(unsigned          )>>& ArgumentParser::_data<unsigned          >() { return _unsignedInts; }
template <> std::unordered_map<std::string, std::function<void(long              )>>& ArgumentParser::_data<long              >() { return _longs; }
template <> std::unordered_map<std::string, std::function<void(unsigned long     )>>& ArgumentParser::_data<unsigned long     >() { return _unsignedLongs; }
template <> std::unordered_map<std::string, std::function<void(long long         )>>& ArgumentParser::_data<long long         >() { return _longLongs; }
template <> std::unordered_map<std::string, std::function<void(unsigned long long)>>& ArgumentParser::_data<unsigned long long>() { return _unsignedLongLongs; }
template <> std::unordered_map<std::string, std::function<void(float             )>>& ArgumentParser::_data<float             >() { return _floats; }
template <> std::unordered_map<std::string, std::function<void(double            )>>& ArgumentParser::_data<double            >() { return _doubles; }
template <> std::unordered_map<std::string, std::function<void(long double       )>>& ArgumentParser::_data<long double       >() { return _longDoubles; }
template <> std::unordered_map<std::string, std::function<void(const std::string&)>>& ArgumentParser::_data<const std::string&>() { return _strings; }


void ArgumentParser::addFlag(const std::string& name, const std::string& description, bool* destination) {
    addFlag(name, description, [=] { *destination = true;});
}

void ArgumentParser::addFlag(const std::string& name, const std::string& description, std::function<void()> callback) {
    _options.back().add_options()(name.c_str(), description.c_str());
    _flags.emplace(name, callback);
}

} // namespace scraps
