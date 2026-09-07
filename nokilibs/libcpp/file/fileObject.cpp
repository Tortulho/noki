#include "fileObject.hpp"

#include <algorithm>
#include <array>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <utility>

#include "runtimeobjectManager.hpp"
#include "runtimeval.hpp"

namespace
{
    const char* getOpenMode(
        FileMode mode
    )
    {
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

    const char* getCloneOpenMode(
        FileMode mode
    )
    {
        switch (mode)
        {
            case FileMode::READ:
                return "r";

            case FileMode::WRITE:
                return "r+";

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
{
}

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

    std::unique_ptr<FileObject> object;

    if (manager == nullptr)
    {
        object =
            std::make_unique<FileObject>(
                nullptr,
                getTypeID(),
                path,
                mode
            );
    }
    else
    {
        object =
            manager->createObject<FileObject>(
                getTypeID(),
                path,
                mode
            );
    }

    object->size =
        size;

    object->position =
        position;

    object->nextLinePosition =
        nextLinePosition;

    if (file == nullptr)
    {
        object->file = nullptr;
        object->isManual = false;

        return object;
    }

    object->file =
        std::fopen(
            path.c_str(),
            getCloneOpenMode(mode)
        );

    if (object->file == nullptr)
    {
        throw std::runtime_error(
            "Failed to clone open file: " +
            path
        );
    }

    if (
        std::fseek(
            object->file,
            static_cast<long>(position),
            SEEK_SET
        ) != 0
    )
    {
        std::fclose(object->file);
        object->file = nullptr;

        throw std::runtime_error(
            "Failed to restore file position while cloning: " +
            path
        );
    }

    object->isManual =
        isManual;

    return object;
}

RuntimeValue FileObject::open()
{
    if (file != nullptr)
    {
        throw std::runtime_error(
            "File is already open: " +
            path
        );
    }

    file =
        std::fopen(
            path.c_str(),
            getOpenMode(mode)
        );

    if (file == nullptr)
    {
        throw std::runtime_error(
            "Failed to open file: " +
            path
        );
    }

    isManual = true;

    if (
        std::fseek(
            file,
            0,
            SEEK_END
        ) != 0
    )
    {
        std::fclose(file);
        file = nullptr;
        isManual = false;

        throw std::runtime_error(
            "Failed to seek file: " +
            path
        );
    }

    const long fileSize =
        std::ftell(file);

    if (fileSize < 0)
    {
        std::fclose(file);
        file = nullptr;
        isManual = false;

        throw std::runtime_error(
            "Failed to determine file size: " +
            path
        );
    }

    size =
        static_cast<std::int64_t>(
            fileSize
        );

    if (
        std::fseek(
            file,
            0,
            SEEK_SET
        ) != 0
    )
    {
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

    const std::string oldPath =
        path;

    const FileMode oldMode =
        mode;

    path = newPath;
    mode = newMode;

    try
    {
        return open();
    }
    catch (...)
    {
        path = oldPath;
        mode = oldMode;

        throw;
    }
}

RuntimeValue FileObject::set(
    const std::string& newPath,
    FileMode newMode
)
{
    if (newPath.empty())
    {
        throw std::runtime_error(
            "File path cannot be empty."
        );
    }

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
    if (file == nullptr)
    {
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

RuntimeValue FileObject::rename(
    const std::string& newPath
)
{
    if (newPath.empty())
    {
        throw std::runtime_error(
            "File path cannot be empty."
        );
    }

    if (
        !std::filesystem::is_regular_file(
            path
        )
    )
    {
        return RuntimeValue(
            RTValue{false}
        );
    }

    if (file != nullptr)
    {
        close();
    }

    std::filesystem::rename(
        path,
        newPath
    );

    path = newPath;

    return RuntimeValue(
        RTValue{true}
    );
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

void FileObject::setPath(
    const std::string& newPath
)
{
    if (newPath.empty())
    {
        throw std::runtime_error(
            "Runtime error: invalid filepath."
        );
    }

    path = newPath;
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

    size =
        static_cast<std::int64_t>(
            fileSize
        );

    position = 0;
    nextLinePosition = 0;

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
        static_cast<std::size_t>(
            fileSize
        )
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

    return RuntimeValue(
        std::move(result)
    );
}

RuntimeValue FileObject::save(
    const std::string& value
)
{
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

RuntimeValue FileObject::write(
    const std::string& value
)
{
    if (file == nullptr)
    {
        throw std::runtime_error(
            "File is not open."
        );
    }

    if (mode == FileMode::READ)
    {
        throw std::runtime_error(
            "File is not writable."
        );
    }

    if (
        std::fwrite(
            value.data(),
            1,
            value.size(),
            file
        ) != value.size()
    )
    {
        throw std::runtime_error(
            "Failed to write to file."
        );
    }

    position +=
        static_cast<std::int64_t>(
            value.size()
        );

    if (position > size)
        size = position;

    return RuntimeValue(nullptr);
}

RuntimeValue FileObject::read(
    std::int64_t requestedSize
)
{
    return read(
        requestedSize,
        position
    );
}

RuntimeValue FileObject::read(
    std::int64_t requestedSize,
    std::int64_t readPosition
)
{
    if (file == nullptr)
    {
        throw std::runtime_error(
            "File is not open: " +
            path
        );
    }

    if (requestedSize < 0)
    {
        throw std::runtime_error(
            "File.read() size cannot be negative."
        );
    }

    if (
        readPosition < 0 ||
        readPosition > size
    )
    {
        throw std::runtime_error(
            "File.read() position out of bounds."
        );
    }

    if (requestedSize == 0)
    {
        return RuntimeValue(
            std::string()
        );
    }

    if (readPosition == size)
    {
        return RuntimeValue(
            std::string()
        );
    }

    if (
        std::fseek(
            file,
            static_cast<long>(readPosition),
            SEEK_SET
        ) != 0
    )
    {
        throw std::runtime_error(
            "Failed to seek file: " +
            path
        );
    }

    const std::int64_t remaining =
        size - readPosition;

    const std::int64_t readSize =
        std::min(
            requestedSize,
            remaining
        );

    std::string result;

    result.resize(
        static_cast<std::size_t>(
            readSize
        )
    );

    const std::size_t readCount =
        std::fread(
            result.data(),
            sizeof(char),
            result.size(),
            file
        );

    if (readCount != result.size())
    {
        result.resize(readCount);

        if (std::ferror(file))
        {
            clearerr(file);

            throw std::runtime_error(
                "Failed to read file: " +
                path
            );
        }
    }

    position =
        readPosition +
        static_cast<std::int64_t>(
            readCount
        );

    return RuntimeValue(
        std::move(result)
    );
}

RuntimeValue FileObject::search(
    const std::string& value
)
{
    return search(
        value,
        position
    );
}

RuntimeValue FileObject::search(
    const std::string& value,
    std::int64_t searchPosition
)
{
    if (file == nullptr)
    {
        throw std::runtime_error(
            "File is not open: " +
            path
        );
    }

    if (
        searchPosition < 0 ||
        searchPosition > size
    )
    {
        throw std::runtime_error(
            "File.search() position out of bounds."
        );
    }

    if (value.empty())
    {
        return RuntimeValue(
            RTValue{searchPosition}
        );
    }

    constexpr std::size_t BLOCK_SIZE =
        8192;

    if (value.size() > BLOCK_SIZE)
    {
        throw std::runtime_error(
            "File.search() search string is too large."
        );
    }

    if (
        std::fseek(
            file,
            static_cast<long>(searchPosition),
            SEEK_SET
        ) != 0
    )
    {
        throw std::runtime_error(
            "Failed to seek file: " +
            path
        );
    }

    std::array<
        char,
        BLOCK_SIZE * 2
    > buffer{};

    const std::size_t overlap =
        value.size() - 1;

    std::size_t carried = 0;

    std::int64_t blockPosition =
        searchPosition;

    while (true)
    {
        const std::size_t readCount =
            std::fread(
                buffer.data() + carried,
                sizeof(char),
                BLOCK_SIZE,
                file
            );

        const std::size_t available =
            carried + readCount;

        if (available == 0)
        {
            if (std::ferror(file))
            {
                clearerr(file);

                throw std::runtime_error(
                    "Failed to read file while searching: " +
                    path
                );
            }

            position =
                blockPosition;

            return RuntimeValue(
                RTValue{static_cast<std::int64_t>(-1)}
            );
        }

        if (available >= value.size())
        {
            auto it =
                std::search(
                    buffer.begin(),
                    buffer.begin() + available,
                    value.begin(),
                    value.end()
                );

            if (
                it !=
                buffer.begin() + available
            )
            {
                const std::size_t offset =
                    static_cast<std::size_t>(
                        it - buffer.begin()
                    );

                const std::int64_t result =
                    blockPosition -
                    static_cast<std::int64_t>(
                        carried
                    ) +
                    static_cast<std::int64_t>(
                        offset
                    );

                position =
                    result +
                    static_cast<std::int64_t>(
                        value.size()
                    );

                return RuntimeValue(
                    RTValue{result}
                );
            }
        }

        blockPosition +=
            static_cast<std::int64_t>(
                readCount
            );

        if (available <= overlap)
        {
            std::memmove(
                buffer.data(),
                buffer.data() + available,
                available
            );

            carried =
                available;
        }
        else
        {
            std::memmove(
                buffer.data(),
                buffer.data() +
                    (available - overlap),
                overlap
            );

            carried =
                overlap;
        }

        if (readCount < BLOCK_SIZE)
        {
            if (std::ferror(file))
            {
                clearerr(file);

                throw std::runtime_error(
                    "Failed to read file while searching: " +
                    path
                );
            }

            position =
                blockPosition;

            return RuntimeValue(
                RTValue{static_cast<std::int64_t>(-1)}
            );
        }
    }
}