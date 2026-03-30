#pragma once

#include <cassert>

#include <iterator>
#include <memory>
#include <optional>
#include <print>
#include <string>
#include <unordered_map>

#include "args.h"

namespace ezcli
{

constexpr std::string_view ICOMMAND_DEFAULT_NAME {"ICommand-Unnamed"};
constexpr std::string_view ICOMMAND_DEFAULT_DESCRIPTION {"no description"};

struct ICommand;

using SubCommand        = std::shared_ptr<ICommand>;
using SubCommandList    = std::unordered_map<std::string, SubCommand>;
using OptionReturn      = std::optional<int>;
using OptionArgs        = std::optional<ArgsAccessor>;
struct ReturnCode
{
    static constexpr OptionReturn   Continue        = std::nullopt;
    static constexpr OptionReturn   WrongCommand    = -1;
    static constexpr OptionReturn   Success         = 0;
    static constexpr OptionReturn   Failed          = 1;
    static constexpr OptionReturn   IllegalArg      = 2;
};

// 抽离原本属于ICommand的成员变量
// 用于省略复杂的构造函数重载
struct CommandConfig
{
    ICommand*       parent              {nullptr};                      // 父命令
    std::string     name                {ICOMMAND_DEFAULT_NAME};        // 命令名称
    std::string     description         {ICOMMAND_DEFAULT_DESCRIPTION}; // 描述
    SubCommandList  sub_command_list    {};                             // 子命令注册表
};

struct ICommand : public CommandConfig
{
    ICommand(const CommandConfig& config = {}) : CommandConfig(config)
    {
        // 矫正未手动指定的命令名称
        for (auto& [name, cmd] : sub_command_list) {
            if (cmd->name == ICOMMAND_DEFAULT_NAME) cmd->name = name;
        }
    }

    virtual ~ICommand() = default;

    virtual OptionReturn operator()(OptionArgs) = 0;

    virtual void printHelp() const
    {
        std::println("no help message for command {}", name);
    }

    virtual void printUsage() const
    {
        std::println("usage: \ttype `{} help` to get help", name);
    }
};

template<class CommandT>
int call(int argc, char** argv)
{
    Args args(argc, argv);
    
    auto real_begin = std::next(args.begin());
    if (auto result = CommandT{}(ArgsAccessor{real_begin, args.end()});
        result.has_value())
    {
        return result.value();
    }

    assert(false && "Commad should return a value");
    return ReturnCode::WrongCommand.value();
}


// 工具函数
inline bool checkOptionArgs(const OptionArgs& oa, std::size_t n_min = 1)
{
    return oa.has_value() && oa->size() >= n_min;
}

}