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

    //UTILS
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

        type->registerMethod(
            "read",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::read
                )
            }
        );

        type->registerMethod(
            "write",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::write
                )
            }
        );

        type->registerMethod(
            "search",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::search
                )
            }
        );

        type->registerMethod(
            "size",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::size
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

    RuntimeValue read(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2 && args.size() != 3)
            throw std::runtime_error(
                "Syntax error: file.read() expects 1 or 2 arguments."
            );

        RuntimeValue* receiver = args[0];

        if (receiver == nullptr ||
            !receiver->isObject())
            throw std::runtime_error(
                "Runtime error: file.read() receiver must be a File object."
            );

        RuntimeObject* object =
            receiver->getObject();

        FileObject* file =
            dynamic_cast<FileObject*>(object);

        if (file == nullptr)
            throw std::runtime_error(
                "Runtime error: file.read() receiver must be a File object."
            );

        if (!args[1]->is<int64_t>())
            throw std::runtime_error(
                "Runtime error: file.read() size must be an integer."
            );

        if (args.size() == 2)
        {
            return file->read(
                args[1]->get<int64_t>()
            );
        }

        if (args[2] == nullptr ||
            !args[2]->is<int64_t>())
            throw std::runtime_error(
                "Runtime error: file.read() position must be an integer."
            );

        return file->read(
            args[1]->get<int64_t>(),
            args[2]->get<int64_t>()
        );
    }

    RuntimeValue search(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2 && args.size() != 3)
            throw std::runtime_error(
                "Syntax error: file.search() expects 1 or 2 arguments."
            );

        RuntimeValue* receiver = args[0];

        if (receiver == nullptr ||
            !receiver->isObject())
            throw std::runtime_error(
                "Runtime error: file.search() receiver must be a File object."
            );

        RuntimeObject* object =
            receiver->getObject();

        FileObject* file =
            dynamic_cast<FileObject*>(object);

        if (file == nullptr)
            throw std::runtime_error(
                "Runtime error: file.search() receiver must be a File object."
            );

        if (args[1] == nullptr ||
            !args[1]->is<std::string>())
            throw std::runtime_error(
                "Runtime error: file.search() string must be a string."
            );

        if (args.size() == 2)
        {
            return file->search(
                args[1]->get<std::string>()
            );
        }

        if (args[2] == nullptr ||
            !args[2]->is<int64_t>())
            throw std::runtime_error(
                "Runtime error: file.search() position must be an integer."
            );

        return file->search(
            args[1]->get<std::string>(),
            args[2]->get<int64_t>()
        );
    }

    RuntimeValue write(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
            throw std::runtime_error(
                "Syntax error: file.write() expects 1 argument."
            );

        RuntimeValue* receiver = args[0];
        RuntimeValue* value = args[1];

        if (receiver == nullptr ||
            !receiver->isObject())
            throw std::runtime_error(
                "Runtime error: file.write() receiver must be a File object."
            );

        RuntimeObject* object =
            receiver->getObject();

        FileObject* file =
            dynamic_cast<FileObject*>(object);

        if (file == nullptr)
            throw std::runtime_error(
                "Runtime error: file.write() receiver must be a File object."
            );

        if (value == nullptr ||
            !value->is<std::string>())
            throw std::runtime_error(
                "Runtime error: file.write() argument must be a string."
            );

        return file->write(
            value->get<std::string>()
        );
    }

    RuntimeValue size(std::vector<RuntimeValue*>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error("File.size() expects 1 argument.");

        RuntimeValue* receiver = args[0];

        if (receiver == nullptr || !receiver->isObject())
            throw std::runtime_error("File.size() receiver must be a FileObject.");

        RuntimeObject* object = receiver->getObject();

        FileObject* file = dynamic_cast<FileObject*>(object);

        if (file == nullptr)
            throw std::runtime_error("File.size() receiver must be a FileObject.");

        return RuntimeValue(static_cast<int64_t>(file->getSize()));
    }

    RuntimeValue create(const std::vector<RuntimeValue>& args)
    {
        if (args.size() != 1)
            throw std::runtime_error("file.create() expects 1 argument.");

        if (!args[0].is<std::string>())
            throw std::runtime_error("file.create() expects a string filename.");

        const std::string& filename = args[0].get<std::string>();

        FILE* existing = std::fopen(filename.c_str(), "r");

        if (existing != nullptr)
        {
            std::fclose(existing);
            return RuntimeValue(false);
        }

        FILE* created = std::fopen(filename.c_str(), "w");

        if (created == nullptr)
            throw std::runtime_error("Failed to create file: " + filename);

        std::fclose(created);

        return RuntimeValue(true);
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
            },
            {
                "create",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::create
                    )
                }
            },
        },
        {},
        file::createInitializer
    };
}