#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
#include <variant>
#include <vector>

using namespace std;

template <class opts>
class cmd_opts : opts
{
public:
    using my_prop = variant<string opts::*, int opts::*, double opts::*, bool opts::*>;
    using my_arg = pair<string, my_prop>;

    ~cmd_opts() = default;

    opts parse(int argc, const char *argv[])
    {
        vector<string_view> vargv(argv, argv + argc);
        for (int i = 0; i < argc; i++)
        {
            for (auto &cb : callbacks)
            {
                cb.second(i, vargv);
            }
        }
        return static_cast<opts>(*this);
    }

    static unique_ptr<cmd_opts> create(initializer_list<my_arg> args)
    {
        auto cmdopts = unique_ptr<cmd_opts>(new cmd_opts());
        for (auto arg : args)
        {
            cmdopts->register_callback(arg);
        }
        return cmdopts;
    }

private:
    using callback_t = function<void(int, const vector<string_view> &)>;
    map<string, callback_t> callbacks;

    cmd_opts() = default;
    cmd_opts(const cmd_opts &) = delete;
    cmd_opts(cmd_opts &&) = delete;
    cmd_opts &operator=(const cmd_opts &) = delete;
    cmd_opts &operator=(cmd_opts &&) = delete;

    auto register_callback(string name, my_prop prop)
    {
        callbacks[name] = [this, name, prop](int idx, const vector<string_view> &argv)
        {
            if (argv[idx] == name)
            {
                visit(
                    [this, idx, &argv](auto &&arg)
                    {
                        if (idx < argv.size() - 1)
                        {
                            stringstream value;
                            value << argv[idx + 1];
                            value >> this->*arg;
                        }
                    },
                    prop);
            }
        };
    };

    auto register_callback(my_arg arg)
    {
        return register_callback(arg.first, arg.second);
    }
};