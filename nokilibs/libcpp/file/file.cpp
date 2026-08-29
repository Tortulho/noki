#include "file.hpp"

#include "fileObject.hpp"
#include "runtime.hpp"
#include "runtimeobjectTypeInitializer.hpp"
#include "libinitializer.hpp"

#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{
    FileObject* getFileObject(
        RuntimeValue* value
    )
    {
        if (value == nullptr)
            return nullptr;

        if (!value->isObject())
            return nullptr;

        return dynamic_cast<FileObject*>(
            value->getObject()
        );
    }

    FileMode parseFileMode(
        const RuntimeValue& value
    )
    {
        if (!value.is<std::string>())
        {
            throw std::runtime_error(
                "File mode must be a string."
            );
        }

        const std::string& mode =
            value.get<std::string>();

        if (mode == "r")
            return FileMode::READ;

        if (mode == "w")
            return FileMode::WRITE;

        if (mode == "rw")
            return FileMode::READ_WRITE;

        throw std::runtime_error(
            "Invalid file mode '" +
            mode +
            "'. Expected 'r', 'w' or 'rw'."
        );
    }
}

class FileLibraryInitializer final
    : public LibraryInitializer,
      public RuntimeObjectTypeInitializer
{
public:

    explicit FileLibraryInitializer(
        RuntimeObjectManager& manager
    )
        : RuntimeObjectTypeInitializer(
            manager,
            "File"
        )
    {
        RuntimeObjectType* type =
            getType();

        if (type == nullptr)
        {
            throw std::runtime_error(
                "Failed to initialize File type."
            );
        }

        type->registerMethod(
            "open",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::open
                )
            }
        );

        type->registerMethod(
            "close",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::close
                )
            }
        );

        type->registerMethod(
            "load",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::load
                )
            }
        );

        type->registerMethod(
            "save",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::save
                )
            }
        );
    }
};

namespace file
{
    std::unique_ptr<LibraryInitializer>
    createInitializer(
        RuntimeObjectManager& manager
    )
    {
        return std::make_unique<FileLibraryInitializer>(
            manager
        );
    }

    RuntimeValue open(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "file.open() expects 2 arguments."
            );
        }

        if (!args[0].is<std::string>())
        {
            throw std::runtime_error(
                "file.open() path must be a string."
            );
        }

        const std::string& path =
            args[0].get<std::string>();

        if (path.empty())
        {
            throw std::runtime_error(
                "file.open() path cannot be empty."
            );
        }

        FileMode mode =
            parseFileMode(args[1]);

        RuntimeObjectType* type =
            runtime.objects.findType("File");

        if (type == nullptr)
        {
            throw std::runtime_error(
                "File type is not initialized."
            );
        }

        auto object =
            runtime.objects.createObject<FileObject>(
                type->getID(),
                path,
                mode
            );

        object->open();

        return RuntimeValue::fromObject(
            std::move(object)
        );
    }

    RuntimeValue open(
        std::vector<RuntimeValue*>& args
    )
    {
        if (
            args.size() != 1 &&
            args.size() != 3
        )
        {
            throw std::runtime_error(
                "File.open() expects 0 or 2 arguments."
            );
        }

        FileObject* file =
            getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "File.open() receiver is not a File object."
            );
        }

        if (args.size() == 1)
        {
            return file->open();
        }

        RuntimeValue* pathValue =
            args[1];

        RuntimeValue* modeValue =
            args[2];

        if (
            pathValue == nullptr ||
            modeValue == nullptr
        )
        {
            throw std::runtime_error(
                "File.open() received a null argument."
            );
        }

        if (!pathValue->is<std::string>())
        {
            throw std::runtime_error(
                "File.open() path must be a string."
            );
        }

        const std::string& path =
            pathValue->get<std::string>();

        if (path.empty())
        {
            throw std::runtime_error(
                "File.open() path cannot be empty."
            );
        }

        FileMode mode =
            parseFileMode(*modeValue);

        return file->open(
            path,
            mode
        );
    }

    RuntimeValue close(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "File.close() expects no arguments."
            );
        }

        FileObject* file =
            getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "File.close() receiver is not a File object."
            );
        }

        return file->close();
    }

    RuntimeValue load(
    std::vector<RuntimeValue*>& args
)
{
    if (args.size() != 1)
    {
        throw std::runtime_error(
            "File.load() expects no arguments."
        );
    }

    FileObject* file =
        getFileObject(args[0]);

    if (file == nullptr)
    {
        throw std::runtime_error(
            "File.load() receiver is not a File object."
        );
    }

    return file->load();
}

    RuntimeValue save(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "File.save() expects 1 argument."
            );
        }

        FileObject* file =
            getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "File.save() receiver is not a File object."
            );
        }

        RuntimeValue* value =
            args[1];

        if (value == nullptr)
        {
            throw std::runtime_error(
                "File.save() argument cannot be null."
            );
        }

        if (!value->is<std::string>())
        {
            throw std::runtime_error(
                "File.save() argument must be a string."
            );
        }

        return file->save(
            value->get<std::string>()
        );
    }

}

Library createFileLibrary()
{
    return Library
    {
        "file",
        {
            {
                "open",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::open
                    )
                }
            }
        },
        {},
        file::createInitializer
    };
}