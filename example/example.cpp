#include "ezcli/ezcli.h"
#include <optional>
#include <print>
#include <string>

struct EzCliExample : public ezcli::ICommand
{
private:
    struct Help : public ezcli::ICommand
    {
        Help(ezcli::ICommand* parent) : ICommand({parent}) {}

        virtual ezcli::OptionReturn operator()(ezcli::OptionArgs = std::nullopt) override {
            for (const auto& cmd : parent->sub_command_list)
                std::println("{}\n {}", cmd.first, cmd.second->description);

            return ezcli::ReturnCode::Success;
        }
    };

    struct Version : public ezcli::ICommand
    {
        virtual ezcli::OptionReturn operator()(ezcli::OptionArgs = std::nullopt) override {
            std::println("Version: UnSet");

            return ezcli::ReturnCode::Success;
        }
    };

public:
    EzCliExample() : ICommand({
        nullptr, 
        "EzCliExample", 
        "Easy CommandLine Parser Example",
        {
            {"help",    std::make_unique<Help>(this)},
            {"version", std::make_unique<Version>()}
        }
    })
    {}

    virtual ezcli::OptionReturn operator()(ezcli::OptionArgs args_) override {
        if (!ezcli::checkOptionArgs(args_)) {
            return ezcli::ReturnCode::IllegalArg;
        }

        auto& args = args_.value();
        while (args.hasNext())
        {
            auto arg = args.next();
            if (auto cmd = sub_command_list.find(arg.get<std::string>());
                cmd != sub_command_list.end())
            {
                auto ret = cmd->second->operator()(args);
                if (ret != ezcli::ReturnCode::Continue) return ret;
            }
            else
            {
                printUsage();
                return ezcli::ReturnCode::Failed;
            }
        }

        return ezcli::ReturnCode::Success;
    }
};

int main(int argc, char** argv) {
    return ezcli::call<EzCliExample>(argc, argv);
}