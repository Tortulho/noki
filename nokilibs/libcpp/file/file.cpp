#include "file.hpp"

#include "fileObject.hpp"
#include "runtime.hpp"
#include "runtimeobjectTypeInitializer.hpp"
#include "libinitializer.hpp"

#include <filesystem>
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
            "set",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::set
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

        type->registerMethod(
            "rename",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    file::frename
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

        const FileMode mode =
            parseFileMode(
                args[1]
            );

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

    RuntimeValue set(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "file.set() expects 2 arguments."
            );
        }

        if (!args[0].is<std::string>())
        {
            throw std::runtime_error(
                "file.set() path must be a string."
            );
        }

        const std::string& path =
            args[0].get<std::string>();

        if (path.empty())
        {
            throw std::runtime_error(
                "file.set() path cannot be empty."
            );
        }

        const FileMode mode =
            parseFileMode(
                args[1]
            );

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

    RuntimeValue create(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "file.create() expects 1 argument."
            );
        }

        if (!args[0].is<std::string>())
        {
            throw std::runtime_error(
                "file.create() expects a string filename."
            );
        }

        const std::string& filename =
            args[0].get<std::string>();

        FILE* existing =
            std::fopen(
                filename.c_str(),
                "r"
            );

        if (existing != nullptr)
        {
            std::fclose(existing);

            return RuntimeValue(
                RTValue{false}
            );
        }

        FILE* created =
            std::fopen(
                filename.c_str(),
                "w"
            );

        if (created == nullptr)
        {
            throw std::runtime_error(
                "Failed to create file: " +
                filename
            );
        }

        if (std::fclose(created) != 0)
        {
            throw std::runtime_error(
                "Failed to close created file: " +
                filename
            );
        }

        return RuntimeValue(
            RTValue{true}
        );
    }

    RuntimeValue fexists(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "file.exists() expects 1 argument."
            );
        }

        if (!args[0].is<std::string>())
        {
            throw std::runtime_error(
                "file.exists() path must be a string."
            );
        }

        const std::string& path =
            args[0].get<std::string>();

        return RuntimeValue(
            RTValue{
                std::filesystem::is_regular_file(path)
            }
        );
    }

    RuntimeValue frename(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "file.rename() expects 2 arguments."
            );
        }

        if (
            !args[0].is<std::string>() ||
            !args[1].is<std::string>()
        )
        {
            throw std::runtime_error(
                "file.rename() paths must be strings."
            );
        }

        const std::string& oldPath =
            args[0].get<std::string>();

        const std::string& newPath =
            args[1].get<std::string>();

        if (
            oldPath.empty() || newPath.empty()
        )
        {
            return RuntimeValue(
                RTValue{false}
            );
        }

        if (!std::filesystem::is_regular_file(oldPath))
        {
            return RuntimeValue(
                RTValue{false}
            );
        }

        std::filesystem::rename(
            oldPath,
            newPath
        );

        return RuntimeValue(
            RTValue{true}
        );
    }

    RuntimeValue frename(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "file.rename() expects 1 argument."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.rename() receiver is not a File object."
            );
        }

        if (
            args[1] == nullptr ||
            !args[1]->is<std::string>()
        )
        {
            throw std::runtime_error(
                "file.rename() path must be a string."
            );
        }

        const std::string& newPath = args[1]->get<std::string>();

        return file->rename(newPath);
    }

    RuntimeValue fdelete(
        const std::vector<RuntimeValue>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "file.remove() expects 1 argument."
            );
        }

        if (!args[0].is<std::string>())
        {
            throw std::runtime_error(
                "file.remove() path must be a string."
            );
        }

        const std::string& path =
            args[0].get<std::string>();

        return RuntimeValue(
            RTValue{
                std::filesystem::remove(path)
            }
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
                "file.open() expects 0 or 2 arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.open() receiver is not a File object."
            );
        }

        if (args.size() == 1)
        {
            return file->open();
        }

        if (
            args[1] == nullptr ||
            args[2] == nullptr
        )
        {
            throw std::runtime_error(
                "file.open() received a null argument."
            );
        }

        if (!args[1]->is<std::string>())
        {
            throw std::runtime_error(
                "file.open() path must be a string."
            );
        }

        const std::string& path =
            args[1]->get<std::string>();

        if (path.empty())
        {
            throw std::runtime_error(
                "file.open() path cannot be empty."
            );
        }

        const FileMode mode =
            parseFileMode(
                *args[2]
            );

        return file->open(
            path,
            mode
        );
    }

    RuntimeValue set(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 3)
        {
            throw std::runtime_error(
                "file.set() expects 2 arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.set() receiver is not a File object."
            );
        }

        if (
            args[1] == nullptr ||
            args[2] == nullptr
        )
        {
            throw std::runtime_error(
                "file.set() received a null argument."
            );
        }

        if (!args[1]->is<std::string>())
        {
            throw std::runtime_error(
                "file.set() path must be a string."
            );
        }

        const std::string& path =
            args[1]->get<std::string>();

        if (path.empty())
        {
            throw std::runtime_error(
                "file.set() path cannot be empty."
            );
        }

        const FileMode mode =
            parseFileMode(
                *args[2]
            );

        return file->set(
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
                "file.close() expects no arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.close() receiver is not a File object."
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
                "file.load() expects no arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.load() receiver is not a File object."
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
                "file.save() expects 1 argument."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.save() receiver is not a File object."
            );
        }

        if (
            args[1] == nullptr ||
            !args[1]->is<std::string>()
        )
        {
            throw std::runtime_error(
                "file.save() argument must be a string."
            );
        }

        return file->save(
            args[1]->get<std::string>()
        );
    }

    RuntimeValue read(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2 && args.size() != 3)
        {
            throw std::runtime_error(
                "file.read() expects 1 or 2 arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.read() receiver must be a File object."
            );
        }

        if (args[1] == nullptr ||!args[1]->is<int64_t>())
        {
            throw std::runtime_error(
                "file.read() size must be an integer."
            );
        }

        if (args.size() == 2)
        {
            return file->read(
                args[1]->get<int64_t>()
            );
        }

        if (args[2] == nullptr ||!args[2]->is<int64_t>())
        {
            throw std::runtime_error(
                "file.read() position must be an integer."
            );
        }

        return file->read(
            args[1]->get<int64_t>(),
            args[2]->get<int64_t>()
        );
    }

    RuntimeValue write(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "file.write() expects 1 argument."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.write() receiver must be a File object."
            );
        }

        if (args[1] == nullptr ||!args[1]->is<std::string>())
        {
            throw std::runtime_error(
                "file.write() argument must be a string."
            );
        }

        return file->write(
            args[1]->get<std::string>()
        );
    }

    RuntimeValue search(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2 && args.size() != 3)
        {
            throw std::runtime_error(
                "file.search() expects 1 or 2 arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.search() receiver must be a File object."
            );
        }

        if (
            args[1] == nullptr ||
            !args[1]->is<std::string>()
        )
        {
            throw std::runtime_error(
                "file.search() string must be a string."
            );
        }

        if (args.size() == 2)
        {
            return file->search(
                args[1]->get<std::string>()
            );
        }

        if (
            args[2] == nullptr ||
            !args[2]->is<int64_t>()
        )
        {
            throw std::runtime_error(
                "file.search() position must be an integer."
            );
        }

        return file->search(
            args[1]->get<std::string>(),
            args[2]->get<int64_t>()
        );
    }

    RuntimeValue size(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "file.size() expects no arguments."
            );
        }

        FileObject* file = getFileObject(args[0]);

        if (file == nullptr)
        {
            throw std::runtime_error(
                "file.size() receiver must be a File object."
            );
        }

        return RuntimeValue(
            RTValue{
                static_cast<std::int64_t>(
                    file->getSize()
                )
            }
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
            },
            {
                "set",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::set
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
            {
                "exists",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::fexists
                    )
                }
            },
            {
                "rename",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::frename
                    )
                }
            },
            {
                "remove",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        file::fdelete
                    )
                }
            }
        },
        {},
        file::createInitializer
    };
}