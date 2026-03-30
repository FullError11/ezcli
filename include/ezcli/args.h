#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace ezcli
{

struct Arg : private std::string
{
    using std::string::string;

    template<class T>
    T get() const
    {
        if constexpr (std::is_integral_v<T>)
        {
            if constexpr (std::is_signed_v<T>)
                return std::stoll(c_str());
            else if constexpr (std::is_unsigned_v<T>)
                return std::stoull(c_str());
        }
        else if constexpr (std::is_floating_point_v<T>)
        {
            return std::stold(c_str());
        }
        else if constexpr (std::is_same_v<T, const char*>)
        {
            return c_str();
        }
        else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
        {
            return *this;
        }
    }
};

struct Args : private std::vector<Arg>
{
private:
    using Base = std::vector<Arg>;
public:
    using Base::begin;
    using Base::end;
    using Base::size;
    using Base::iterator;

    Args(int argc, char* argv[])
        : Base(packArgs(argc, argv)) {}

    template<class IteratorT>
    Args(IteratorT begin, IteratorT end)
        : Base(begin, end) {}

    template<class T>
    T getArg(std::size_t n) const
    {
        return this->at(n).get<T>();
    }

    std::string toString() const
    {
        if (this->empty()) return "<null>";

        std::string result;
        for (auto& arg : *this)
            result.append(arg.get<std::string>() + ", ");
        
        return result.substr(0, result.size() - 2);
    }
private:
    static Base packArgs(int argc, char* argv[])
    {
        Base args {};
        args.reserve(argc);
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);
        return args;
    }
};

class ArgsAccessor
{
public:
    ArgsAccessor(Args::iterator& begin, const Args::iterator& end)
        : begin_(begin), end_(end), parent_cursor_(begin) {}
    
    std::size_t size() const { return end_ - begin_; }

    bool hasNext() const { return parent_cursor_ < end_; }

    const Arg& next()
    {
        auto& result = *parent_cursor_;
        parent_cursor_++;
        return result;
    }

    auto begin() const { return begin_; }
    auto end() const { return end_; }

    std::string toString()
    {
        if (this->size() == 0) return "<null>";

        std::string result;
        for (auto it = begin_; it < end_; ++it)
            result.append(it->get<std::string>() + ", ");
        
        return result.substr(0, result.size() - 2);
    }

    /**
     * 截取剩余部分
     */
    ArgsAccessor capture()
    {
        return ArgsAccessor(parent_cursor_, end_);
    }

private:
    const Args::iterator begin_;
    const Args::iterator end_;
    Args::iterator& parent_cursor_;
};

}