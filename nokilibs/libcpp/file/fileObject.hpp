#ifndef NOKI_FILE_OBJECT
#define NOKI_FILE_OBJECT

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>

#include "runtimeobject.hpp"

enum class FileMode
{
    READ,
    WRITE,
    READ_WRITE
};

class FileObject : public RuntimeObject
{
public:

    FileObject(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID,
        std::string path,
        FileMode mode
    );

    ~FileObject() override;

    FileObject(const FileObject&) = delete;
    FileObject& operator=(const FileObject&) = delete;

    FileObject(FileObject&&) = delete;
    FileObject& operator=(FileObject&&) = delete;

    std::unique_ptr<RuntimeObject> clone() const override;

    RuntimeValue open();

    RuntimeValue open(
        const std::string& path,
        FileMode mode
    );

    RuntimeValue close();

    const std::string& getPath() const noexcept;

    FileMode getMode() const noexcept;

    bool IsManual() const noexcept;

    std::int64_t getSize() const noexcept;

    std::int64_t getPosition() const noexcept;

    std::int64_t getNextLinePosition() const noexcept;

    RuntimeValue load();

    RuntimeValue save(const std::string& value);

private:

    FILE* file = nullptr;

    std::int64_t size = 0;
    std::int64_t position = 0;
    std::int64_t nextLinePosition = 0;

    std::string path;

    bool isManual = false;
    FileMode mode;

};

#endif