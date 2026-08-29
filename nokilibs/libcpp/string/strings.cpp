#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include "lib.hpp"
#include "runtimeval.hpp"
#include "strings.hpp"
#include "runtimeobjectTypeInitializer.hpp"
#include "runtimeobjectManager.hpp"
#include "runtimevalType.hpp"
#include "libinitializer.hpp"

class StringLibraryInitializer final
    : public LibraryInitializer
{
public:

    explicit StringLibraryInitializer(
        RuntimeObjectManager& manager
    )
    {
        RuntimeValueType* type =
            manager.findValueType(
                RuntimeValueTypeID::STRING
            );

        if (type == nullptr)
        {
            throw std::runtime_error(
                "Failed to initialize String value type."
            );
        }

        type->registerMethod(
            "format",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::format
                )
            }
        );

        type->registerMethod(
            "cat",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::cat
                )
            }
        );

        type->registerMethod(
            "find",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::find
                )
            }
        );

        type->registerMethod(
            "replace",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::replace
                )
            }
        );

        type->registerMethod(
            "rfind",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::rfind
                )
            }
        );

        type->registerMethod(
            "rreplace",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::rreplace
                )
            }
        );

        type->registerMethod(
            "remove",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::remove
                )
            }
        );

        type->registerMethod(
            "at",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::at
                )
            }
        );

        type->registerMethod(
            "copy",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::copy
                )
            }
        );

        type->registerMethod(
            "strip",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::strip
                )
            }
        );

        type->registerMethod(
            "reserve",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::reserve
                )
            }
        );

        type->registerMethod(
            "reverse",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::reverse
                )
            }
        );

        type->registerMethod(
            "popback",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::popback
                )
            }
        );

        type->registerMethod(
            "size",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::size
                )
            }
        );

        type->registerMethod(
            "isempty",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::isempty
                )
            }
        );

        type->registerMethod(
            "empty",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::empty
                )
            }
        );

        type->registerMethod(
            "pop",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    string::pop
                )
            }
        );
    }
};

std::unique_ptr<LibraryInitializer>
string::createInitializer(
    RuntimeObjectManager& manager
)
{
    return std::make_unique<StringLibraryInitializer>(
        manager
    );
}

#define INT int64_t
#define FLOAT double
#define STRING std::string

Library createStringLibrary()
{
    return Library
    {
        "string",
        {},
        {},
        string::createInitializer
    };
}

std::string vectorToString(const RuntimeValue_Vector& vector)
{
    std::string result = "[";

    for (std::size_t i = 0; i < vector.size(); ++i)
    {
        if (i > 0)
            result += ", ";

        const RuntimeValue& element =
            vector.at(i);

        if (element.is<INT>())
        {
            result += std::to_string(
                element.get<INT>()
            );
        }
        else if (element.is<FLOAT>())
        {
            result += std::to_string(
                element.get<FLOAT>()
            );
        }
        else if (element.is<bool>())
        {
            result +=
                element.get<bool>()
                    ? "true"
                    : "false";
        }
        else if (element.is<STRING>())
        {
            result +=
                element.get<STRING>();
        }
        else if (element.isVector())
        {
            const RuntimeValue_Vector* nested = element.getVector();

            if (nested == nullptr)
            {
                result += "null";
            }
            else
            {
                result += vectorToString(*nested);
            }
        }
        else if (element.isObject())
        {
            result += "<object>";
        }
        else
        {
            result += "null";
        }
    }

    result += "]";

    return result;
}

namespace string {

    RuntimeValue cat(std::vector<RuntimeValue*>& args) {

        if (args.size() != 2)
            throw std::runtime_error("Syntax error: string.cat() expects 1 argument.");

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtval = args[1];

        if (rtstr == nullptr || rtval == nullptr)
            throw std::runtime_error("Runtime error: string.cat() received a null argument.");

        if (!rtstr->is<std::string>())
            throw std::runtime_error("Runtime error: string.cat() receiver must be a string.");
        
        std::string& str = rtstr->get<std::string>();

        if (rtval->is<std::string>()) {
            str += rtval->get<std::string>();
        } else if (rtval->is<int64_t>()) {
            str += rtval->get<int64_t>();
        } else if (rtval->is<double>()) {
            str += rtval->get<double>();
        } else if (rtval->is<bool>()) {
            bool cond = rtval->get<bool>();
            if (cond) {
                str += "true";
            } else {
                str += "false";
            }
        } else {
            str += "null";
        }
        
        return RuntimeValue(nullptr);

    }

    RuntimeValue find(std::vector<RuntimeValue*>& args) {

        //simple find
        if (args.size() == 2) {

            RuntimeValue* rtstr = args[0];
            RuntimeValue* rtval = args[1];

            if (rtstr == nullptr || rtval == nullptr)
                throw std::runtime_error("Runtime error: string.find() received a null argument.");

            if (!rtstr->is<std::string>())
                throw std::runtime_error("Runtime error: string.find() receiver must be a string.");

            if (!rtval->is<std::string>())
                throw std::runtime_error("Runtime error: string.find() argument must be a string.");

            std::string& str = rtstr->get<std::string>();
            std::string& val = rtval->get<std::string>();

            int64_t index = str.find(val,0);
            if (index < 0) index = -1;

            return RuntimeValue(index); //INT
            
        } else if (args.size() == 3) {

            RuntimeValue* rtstr = args[0];
            RuntimeValue* rtval = args[1];
            RuntimeValue* rtidx = args[2];

            if (rtstr == nullptr || rtval == nullptr || rtidx == nullptr)
                throw std::runtime_error("Runtime error: string.find() received a null argument.");

            if (!rtstr->is<std::string>())
                throw std::runtime_error("Runtime error: string.find() receiver must be a string.");

            if (!rtval->is<std::string>())
                throw std::runtime_error("Runtime error: string.find() 'string to found' argument must be a string.");
            
            if (!rtidx->is<std::int64_t>())
                throw std::runtime_error("Runtime error: string.find() index argument must be a integer.");

            std::string& str = rtstr->get<std::string>();
            std::string& val = rtval->get<std::string>();
            INT idx = rtidx->get<INT>();
            if (idx < 0) idx = str.size() + idx;
            if (idx > str.size() || idx < 0) 
                throw std::runtime_error(("Runtime error: Unexpected index to string.find() to search " + idx + '.'));

            int64_t index = str.find(val,idx);
            if (index < 0) index = -1;

            return RuntimeValue(index); //INT

        } else {
            throw std::runtime_error("Syntax error: string.find() expects 1 or 2 arguments.");
        }
    }

    RuntimeValue rfind(std::vector<RuntimeValue*>& args) {

        //simple find
        if (args.size() == 2) {

            RuntimeValue* rtstr = args[0];
            RuntimeValue* rtval = args[1];

            if (rtstr == nullptr || rtval == nullptr)
                throw std::runtime_error("Runtime error: string.rfind() received a null argument.");

            if (!rtstr->is<std::string>())
                throw std::runtime_error("Runtime error: string.rfind() receiver must be a string.");

            if (!rtval->is<std::string>())
                throw std::runtime_error("Runtime error: string.rfind() argument must be a string.");

            std::string& str = rtstr->get<std::string>();
            std::string& val = rtval->get<std::string>();
            int64_t index = -1;
            if (str.size()>0) index = str.rfind(val,str.size()-1);
            if (index < 0) index = -1;

            return RuntimeValue(index); //INT
            
        } else if (args.size() == 3) {

            RuntimeValue* rtstr = args[0];
            RuntimeValue* rtval = args[1];
            RuntimeValue* rtidx = args[2];

            if (rtstr == nullptr || rtval == nullptr || rtidx == nullptr)
                throw std::runtime_error("Runtime error: string.rfind() received a null argument.");

            if (!rtstr->is<std::string>())
                throw std::runtime_error("Runtime error: string.rfind() receiver must be a string.");

            if (!rtval->is<std::string>())
                throw std::runtime_error("Runtime error: string.rfind() 'string to found' argument must be a string.");
            
            if (!rtidx->is<std::int64_t>())
                throw std::runtime_error("Runtime error: string.rfind() index argument must be a integer.");

            std::string& str = rtstr->get<std::string>();
            std::string& val = rtval->get<std::string>();
            INT idx = rtidx->get<INT>();
            if (idx < 0) idx = str.size() + idx;
            if (idx > str.size() || idx < 0) 
                throw std::runtime_error(("Runtime error: Unexpected index to string.rfind() to search " + idx + '.'));

            int64_t index = str.rfind(val,idx);
            if (index < 0) index = -1;

            return RuntimeValue(index); //INT

        } else {
            throw std::runtime_error("Syntax error: string.rfind() expects 1 or 2 arguments.");
        }
    }

    RuntimeValue format(std::vector<RuntimeValue*>& args)
    {
        if (args.empty())
            throw std::runtime_error(
                "Syntax error: string.format() expects a string receiver."
            );

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            throw std::runtime_error(
                "Runtime error: string.format() received a null receiver."
            );

        if (!rtstr->is<STRING>())
            throw std::runtime_error(
                "Runtime error: string.format() receiver must be a string."
            );

        const std::string& formatString =
            rtstr->get<STRING>();

        std::string result;
        std::size_t argumentIndex = 1;

        for (std::size_t i = 0; i < formatString.size(); ++i)
        {
            if (formatString[i] != '%')
            {
                result += formatString[i];
                continue;
            }

            if (i + 1 >= formatString.size())
            {
                throw std::runtime_error(
                    "Runtime error: string.format() has an incomplete format specifier."
                );
            }

            const char specifier =
                formatString[++i];

            if (specifier == '%')
            {
                result += '%';
                continue;
            }

            if (argumentIndex >= args.size())
            {
                throw std::runtime_error(
                    "Runtime error: string.format() received too few arguments."
                );
            }

            RuntimeValue* value =
                args[argumentIndex++];

            if (value == nullptr)
            {
                throw std::runtime_error(
                    "Runtime error: string.format() received a null argument."
                );
            }

            switch (specifier)
            {
                case 'd':
                {
                    if (!value->is<INT>())
                    {
                        throw std::runtime_error(
                            "Runtime error: string.format() '%d' expects an integer."
                        );
                    }

                    result += std::to_string(
                        value->get<INT>()
                    );

                    break;
                }

                case 'f':
                {
                    if (!value->is<FLOAT>())
                    {
                        throw std::runtime_error(
                            "Runtime error: string.format() '%f' expects a float."
                        );
                    }

                    result += std::to_string(
                        value->get<FLOAT>()
                    );

                    break;
                }

                case 's':
                {
                    if (!value->is<STRING>())
                    {
                        throw std::runtime_error(
                            "Runtime error: string.format() '%s' expects a string."
                        );
                    }

                    result += value->get<STRING>();

                    break;
                }

                case 'v':
                {
                    if (!value->isVector())
                    {
                        throw std::runtime_error(
                            "Runtime error: string.format() '%v' expects a vector."
                        );
                    }

                    RuntimeValue_Vector* vector =
                        value->getVector();

                    if (vector == nullptr)
                    {
                        result += "null";
                    }
                    else
                    {
                        result += vectorToString(*vector);
                    }

                    break;
                }

                default:
                {
                    throw std::runtime_error(
                        "Runtime error: string.format() invalid format specifier '%" +
                        std::string(1, specifier) +
                        "'."
                    );
                }
            }
        }

        if (argumentIndex != args.size())
        {
            throw std::runtime_error(
                "Runtime error: string.format() received too many arguments."
            );
        }

        rtstr->get<STRING>() = std::move(result);

        return RuntimeValue(true);
    }

    RuntimeValue size(std::vector<RuntimeValue*>& args) {

        if (args.size() != 1)
            throw std::runtime_error("Syntax error: string.size() expects 0 arguments.");

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            return RuntimeValue(static_cast<int64_t>(0));

        if (!rtstr->is<std::string>())
            throw std::runtime_error("Runtime error: string.size() receiver must be a string.");
        
        std::string& str = rtstr->get<std::string>();
        
        return RuntimeValue(static_cast<int64_t>(str.size()));

    }

    RuntimeValue replace(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 3 && args.size() != 4)
        {
            throw std::runtime_error(
                "string.replace() expects 2 or 3 arguments."
            );
        }

        RuntimeValue *rtstr = args[0];
        RuntimeValue *rtfrom = args[1];
        RuntimeValue *rtto = args[2];

        if (rtstr == nullptr || rtfrom == nullptr || rtto == nullptr)
            throw std::runtime_error("Runtime error: string.replace() received a null argument.");

        if (!rtstr->is<std::string>())
            throw std::runtime_error("Runtime error: string.replace() receiver must be a string.");

        if ((!rtfrom->is<std::string>()) || (!rtto->is<std::string>()))
            throw std::runtime_error("Runtime error: string.replace() argument (from/to) must be a string.");

        std::string& str = rtstr->get<std::string>();
        const std::string& from = rtfrom->get<std::string>();
        const std::string& to = rtto->get<std::string>();

        std::size_t limit = std::string::npos;

        if (args.size() == 4) {

            if (!args[3]->is<INT>()) throw std::runtime_error("Runtime error: string.replace() argument (count) must be a integer.");

            const std::int64_t count = args[3]->get<INT>();
            limit = static_cast<std::size_t>(count);
        }

        if (from.empty()) return RuntimeValue(static_cast<int64_t>(0));

        std::size_t position = 0;
        std::int64_t occurrences = 0;

        while (position < str.size()) {

            const std::size_t found = str.find(from,position);

            if (found == std::string::npos) break;
            if (limit != std::string::npos && static_cast<std::size_t>(occurrences) >= limit) break;

            str.replace(found,from.size(),to);

            position = found + to.size();
            ++occurrences;
        }

        return RuntimeValue(occurrences);
    }

    RuntimeValue rreplace(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 3 && args.size() != 4)
        {
            throw std::runtime_error(
                "string.replace() expects 2 or 3 arguments."
            );
        }

        RuntimeValue *rtstr = args[0];
        RuntimeValue *rtfrom = args[1];
        RuntimeValue *rtto = args[2];

        if (rtstr == nullptr || rtfrom == nullptr || rtto == nullptr)
            throw std::runtime_error("Runtime error: string.replace() received a null argument.");

        if (!rtstr->is<std::string>())
            throw std::runtime_error("Runtime error: string.replace() receiver must be a string.");

        if ((!rtfrom->is<std::string>()) || (!rtto->is<std::string>()))
            throw std::runtime_error("Runtime error: string.replace() argument (from/to) must be a string.");

        std::string& str = rtstr->get<std::string>();
        const std::string& from = rtfrom->get<std::string>();
        const std::string& to = rtto->get<std::string>();

        std::size_t limit = std::string::npos;

        if (args.size() == 4) {

            if (!args[3]->is<INT>()) throw std::runtime_error("Runtime error: string.replace() argument (count) must be a integer.");

            const std::int64_t count = args[3]->get<INT>();
            limit = static_cast<std::size_t>(count);
        }

        if (from.empty()) return RuntimeValue(static_cast<int64_t>(0));

        std::int64_t occurrences = 0;
        std::size_t searchPosition = str.size();

        while (searchPosition >= from.size())
        {
            const std::size_t found =
                str.rfind(
                    from,
                    searchPosition - from.size()
                );

            if (found == std::string::npos)
                break;

            if (
                limit != std::string::npos &&
                static_cast<std::size_t>(occurrences) >= limit
            )
            {
                break;
            }

            str.replace(found,from.size(),to);

            ++occurrences;
            if (found == 0) break;
            searchPosition = found;
        }

        return RuntimeValue(occurrences);
    }

    RuntimeValue at(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "string.at() expects 1 argument."
            );
        }

        if (args[0] == nullptr || args[1] == nullptr)
            throw std::runtime_error("Runtime error: string.at() received a null receiver/argument.");

        if (!args[0]->is<std::string>())
            throw std::runtime_error("Runtime error: string.at() receiver must be a string.");

        const std::string& str = args[0]->get<STRING>();

        if (!args[1]->is<INT>())
            throw std::runtime_error("Runtime error: string.at() argument (index) must be a valid integer.");

        const std::int64_t index = args[1]->get<INT>();

        if (index < 0 ||static_cast<std::size_t>(index) >= str.size())
        {
            throw std::runtime_error(
                "string.at() index out of bounds."
            );
        }

        return RuntimeValue(
            std::string(1,str[static_cast<std::size_t>(index)])
        );
    }

    RuntimeValue remove(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2)
            throw std::runtime_error(
                "Syntax error: string.remove() expects 1 argument."
            );

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtval = args[1];

        if (rtstr == nullptr || rtval == nullptr)
            throw std::runtime_error(
                "Runtime error: string.remove() received a null argument."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.remove() receiver must be a string."
            );

        if (!rtval->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.remove() argument must be a string."
            );

        std::string& str = rtstr->get<STRING>();
        const std::string& val = rtval->get<STRING>();

        if (val.empty())
            return RuntimeValue(false);

        std::size_t position = 0;
        bool removed = false;

        while (true)
        {
            std::size_t index = str.find(val, position);

            if (index == std::string::npos)
                break;

            str.erase(index, val.size());
            position = index;
            removed = true;
        }

        return RuntimeValue(removed);
    }

    RuntimeValue copy(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2 && args.size() != 3)
            throw std::runtime_error(
                "Syntax error: string.copy() expects 1 or 2 arguments."
            );

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtsize = args[1];

        if (rtstr == nullptr || rtsize == nullptr)
            throw std::runtime_error(
                "Runtime error: string.copy() received a null argument."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.copy() receiver must be a string."
            );

        if (!rtsize->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.copy() size argument must be an integer."
            );

        std::string& str = rtstr->get<STRING>();
        INT size = rtsize->get<INT>();

        if (size < 0)
            throw std::runtime_error(
                "Runtime error: string.copy() size cannot be negative."
            );

        if (args.size() == 2)
        {
            return RuntimeValue(
                str.substr(
                    0,
                    static_cast<std::size_t>(size)
                )
            );
        }

        RuntimeValue* rtidx = args[2];

        if (rtidx == nullptr)
            throw std::runtime_error(
                "Runtime error: string.copy() received a null index."
            );

        if (!rtidx->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.copy() index argument must be an integer."
            );

        INT index = rtidx->get<INT>();

        if (index < 0)
            index = static_cast<INT>(str.size()) + index;

        if (
            index < 0 ||
            index > static_cast<INT>(str.size())
        )
        {
            throw std::runtime_error(
                "Runtime error: string.copy() index out of bounds."
            );
        }

        return RuntimeValue(
            str.substr(
                static_cast<std::size_t>(index),
                static_cast<std::size_t>(size)
            )
        );
    }

    RuntimeValue strip(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error(
                "Syntax error: string.strip() expects 0 arguments."
            );

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            throw std::runtime_error(
                "Runtime error: string.strip() received a null receiver."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.strip() receiver must be a string."
            );

        std::string& str = rtstr->get<STRING>();

        std::size_t start = 0;

        while (
            start < str.size() &&
            std::isspace(
                static_cast<unsigned char>(str[start])
            )
        )
        {
            ++start;
        }

        std::size_t end = str.size();

        while (
            end > start &&
            std::isspace(
                static_cast<unsigned char>(str[end - 1])
            )
        )
        {
            --end;
        }

        str.erase(end);

        if (start > 0)
            str.erase(0, start);

        return RuntimeValue(nullptr);
    }

    RuntimeValue reserve(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2)
            throw std::runtime_error(
                "Syntax error: string.reserve() expects 1 argument."
            );

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtsize = args[1];

        if (rtstr == nullptr || rtsize == nullptr)
            throw std::runtime_error(
                "Runtime error: string.reserve() received a null argument."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.reserve() receiver must be a string."
            );

        if (!rtsize->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.reserve() size argument must be an integer."
            );

        INT size = rtsize->get<INT>();

        if (size < 0)
            throw std::runtime_error(
                "Runtime error: string.reserve() size cannot be negative."
            );

        std::string& str = rtstr->get<STRING>();

        str.reserve(
            static_cast<std::size_t>(size)
        );

        return RuntimeValue(nullptr);
    }

    RuntimeValue reverse(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error(
                "Syntax error: string.reverse() expects 0 arguments."
            );

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            throw std::runtime_error(
                "Runtime error: string.reverse() received a null receiver."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.reverse() receiver must be a string."
            );

        std::string& str = rtstr->get<STRING>();

        std::reverse(
            str.begin(),
            str.end()
        );

        return RuntimeValue(nullptr);
    }

    RuntimeValue popback(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2)
            throw std::runtime_error(
                "Syntax error: string.popback() expects 1 argument."
            );

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtsize = args[1];

        if (rtstr == nullptr || rtsize == nullptr)
            throw std::runtime_error(
                "Runtime error: string.popback() received a null argument."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.popback() receiver must be a string."
            );

        if (!rtsize->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.popback() size argument must be an integer."
            );

        INT size = rtsize->get<INT>();

        if (size < 0)
            throw std::runtime_error(
                "Runtime error: string.popback() size cannot be negative."
            );

        std::string& str = rtstr->get<STRING>();

        if (
            static_cast<std::size_t>(size) > str.size()
        )
        {
            return RuntimeValue(false);
        }

        str.erase(
            str.size() - static_cast<std::size_t>(size),
            static_cast<std::size_t>(size)
        );

        return RuntimeValue(true);
    }

    RuntimeValue isempty(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error(
                "Syntax error: string.isempty() expects 0 arguments."
            );

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            throw std::runtime_error(
                "Runtime error: string.isempty() received a null receiver."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.isempty() receiver must be a string."
            );

        std::string& str = rtstr->get<STRING>();

        return RuntimeValue(
            str.empty()
        );
    }

    RuntimeValue empty(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error(
                "Syntax error: string.empty() expects 0 arguments."
            );

        RuntimeValue* rtstr = args[0];

        if (rtstr == nullptr)
            throw std::runtime_error(
                "Runtime error: string.empty() received a null receiver."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.empty() receiver must be a string."
            );

        std::string& str = rtstr->get<STRING>();

        str.clear();

        return RuntimeValue(nullptr);
    }

    RuntimeValue pop(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 2 && args.size() != 3)
            throw std::runtime_error(
                "Syntax error: string.pop() expects 1 or 2 arguments."
            );

        RuntimeValue* rtstr = args[0];
        RuntimeValue* rtstart = args[1];

        if (rtstr == nullptr || rtstart == nullptr)
            throw std::runtime_error(
                "Runtime error: string.pop() received a null argument."
            );

        if (!rtstr->is<std::string>())
            throw std::runtime_error(
                "Runtime error: string.pop() receiver must be a string."
            );

        if (!rtstart->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.pop() index must be an integer."
            );

        std::string& str = rtstr->get<STRING>();

        INT start = rtstart->get<INT>();

        if (start < 0)
            start = static_cast<INT>(str.size()) + start;

        if (
            start < 0 ||
            start >= static_cast<INT>(str.size())
        )
        {
            return RuntimeValue(false);
        }

        if (args.size() == 2)
        {
            str.erase(
                static_cast<std::size_t>(start),
                1
            );

            return RuntimeValue(true);
        }

        RuntimeValue* rtend = args[2];

        if (rtend == nullptr)
            throw std::runtime_error(
                "Runtime error: string.pop() received a null end index."
            );

        if (!rtend->is<INT>())
            throw std::runtime_error(
                "Runtime error: string.pop() end index must be an integer."
            );

        INT end = rtend->get<INT>();

        if (end < 0)
            end = static_cast<INT>(str.size()) + end;

        if (
            end < 0 ||
            end >= static_cast<INT>(str.size()) ||
            start > end
        )
        {
            return RuntimeValue(false);
        }

        /*
        * end é inclusivo.
        */
        str.erase(
            static_cast<std::size_t>(start),
            static_cast<std::size_t>(
                end - start + 1
            )
        );

        return RuntimeValue(true);
    }

}