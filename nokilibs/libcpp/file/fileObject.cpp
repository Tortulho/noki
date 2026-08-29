#include "fileObject.hpp"

#include <stdexcept>
#include <utility>

#include "runtimeobjectManager.hpp"
#include "runtimeval.hpp"

namespace
{
    const char* getOpenMode(FileMode mode) {
        switch (mode)
        {
            case FileMode::READ:
                return "r";

            case FileMode::WRITE:
                return "w";

            case FileMode::READ_WRITE:
                return "r+";
        }

        throw std::runtime_error(
            "Invalid file mode."
        );
    }
}

FileObject::FileObject(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID typeID,
    std::string path,
    FileMode mode
)
    : RuntimeObject(
        manager,
        typeID
    ),
      path(std::move(path)),
      mode(mode)
{}

FileObject::~FileObject()
{
    if (file != nullptr)
    {
        std::fclose(file);
        file = nullptr;
    }

    isManual = false;
}

std::unique_ptr<RuntimeObject> FileObject::clone() const
{
    RuntimeObjectManager* manager =
        getObjectManager();

    if (manager == nullptr)
    {
        auto object =
            std::make_unique<FileObject>(
                nullptr,
                getTypeID(),
                path,
                mode
            );

        object->size = size;

        object->position = position;

        object->nextLinePosition = nextLinePosition;

        /*
         * O FILE* não é clonado.
         */
        object->file = nullptr;
        object->isManual = false;

        return object;
    }

    auto object =
        manager->createObject<FileObject>(
            getTypeID(),
            path,
            mode
        );

    object->size = size;
    object->position = position;
    object->nextLinePosition = nextLinePosition;

    /*
     * O clone começa fechado.
     */
    object->file = nullptr;
    object->isManual = false;

    return object;
}

RuntimeValue FileObject::open()
{
    if (file != nullptr) {
        throw std::runtime_error(
            "File is already open: " +
            path
        );
    }

    file = std::fopen(
        path.c_str(),
        getOpenMode(mode)
    );

    if (file == nullptr) {
        throw std::runtime_error(
            "Failed to open file: " +
            path
        );
    }

    isManual = true;

    /*
     * Determinar o tamanho actual do ficheiro.
     */
    if (std::fseek(file,0,SEEK_END) != 0) {
        std::fclose(file);
        file = nullptr;
        isManual = false;
        throw std::runtime_error(
            "Failed to seek file: " +
            path
        );
    }

    const long fileSize = std::ftell(file);

    if (fileSize < 0)
    {
        std::fclose(file);
        file = nullptr;
        isManual = false;
        throw std::runtime_error(
            "Failed to determine file size: " + path
        );
    }

    size = static_cast<std::int64_t>(fileSize);

    /*
     * Posicionar no início do ficheiro.
     */
    if (std::fseek(file,0,SEEK_SET) != 0) {

        std::fclose(file);
        file = nullptr;
        isManual = false;

        throw std::runtime_error(
            "Failed to reset file position: " +
            path
        );
    }

    position = 0;
    nextLinePosition = 0;

    return RuntimeValue(nullptr);
}

RuntimeValue FileObject::open(
    const std::string& newPath,
    FileMode newMode
)
{
    if (file != nullptr)
    {
        throw std::runtime_error(
            "File is already open: " +
            path
        );
    }

    path = newPath;
    mode = newMode;

    return open();
}

RuntimeValue FileObject::close()
{
    if (file == nullptr) {
        throw std::runtime_error(
            "File is not open: " +
            path
        );
    }

    if (std::fclose(file) != 0)
    {
        file = nullptr;
        isManual = false;

        throw std::runtime_error(
            "Failed to close file: " +
            path
        );
    }

    file = nullptr;
    isManual = false;

    return RuntimeValue(nullptr);
}

const std::string& FileObject::getPath() const noexcept
{
    return path;
}

FileMode FileObject::getMode() const noexcept
{
    return mode;
}

bool FileObject::IsManual() const noexcept
{
    return isManual;
}

std::int64_t FileObject::getSize() const noexcept
{
    return size;
}

std::int64_t FileObject::getPosition() const noexcept
{
    return position;
}

std::int64_t FileObject::getNextLinePosition() const noexcept
{
    return nextLinePosition;
}

RuntimeValue FileObject::load()
{
    FILE* readFile =
        std::fopen(
            path.c_str(),
            "r"
        );

    if (readFile == nullptr)
    {
        throw std::runtime_error(
            "Failed to open file for reading: " +
            path
        );
    }

    if (
        std::fseek(
            readFile,
            0,
            SEEK_END
        ) != 0
    )
    {
        std::fclose(readFile);

        throw std::runtime_error(
            "Failed to seek file: " +
            path
        );
    }

    const long fileSize =
        std::ftell(readFile);

    if (fileSize < 0)
    {
        std::fclose(readFile);

        throw std::runtime_error(
            "Failed to determine file size: " +
            path
        );
    }

    if (
        std::fseek(
            readFile,
            0,
            SEEK_SET
        ) != 0
    )
    {
        std::fclose(readFile);

        throw std::runtime_error(
            "Failed to reset file position: " +
            path
        );
    }

    std::string result;

    result.resize(
        static_cast<std::size_t>(fileSize)
    );

    if (!result.empty())
    {
        const std::size_t read =
            std::fread(
                result.data(),
                sizeof(char),
                result.size(),
                readFile
            );

        if (read != result.size())
        {
            std::fclose(readFile);

            throw std::runtime_error(
                "Failed to read file: " +
                path
            );
        }
    }

    if (std::fclose(readFile) != 0)
    {
        throw std::runtime_error(
            "Failed to close file: " +
            path
        );
    }

    //fast check
    for (unsigned char byte : result)
    {
        if (byte > 127)
        {
            throw std::runtime_error(
                "File is not ASCII: " +
                path
            );
        }
    }

    return RuntimeValue(
        std::move(result)
    );
}

RuntimeValue FileObject::save(
    const std::string& value
)
{
    for (unsigned char byte : value)
    {
        if (byte > 127)
        {
            throw std::runtime_error(
                "Attempted to save non-ASCII content."
            );
        }
    }

    FILE* writeFile =
        std::fopen(
            path.c_str(),
            "w"
        );

    if (writeFile == nullptr)
    {
        throw std::runtime_error(
            "Failed to open file for writing: " +
            path
        );
    }

    if (!value.empty())
    {
        const std::size_t written =
            std::fwrite(
                value.data(),
                sizeof(char),
                value.size(),
                writeFile
            );

        if (written != value.size())
        {
            std::fclose(writeFile);

            throw std::runtime_error(
                "Failed to write file: " +
                path
            );
        }
    }

    if (std::fclose(writeFile) != 0)
    {
        throw std::runtime_error(
            "Failed to close file: " +
            path
        );
    }

    return RuntimeValue(nullptr);
}