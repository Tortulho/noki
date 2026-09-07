#ifndef NOKI_TIME_OBJECT
#define NOKI_TIME_OBJECT

#include <cstdint>
#include <memory>

#include "runtimeobject.hpp"

class TimeObject : public RuntimeObject
{
public:

    TimeObject(
        RuntimeObjectManager* manager,
        RuntimeObjectTypeID typeID,
        std::int64_t value
    );

    ~TimeObject() override = default;

    TimeObject(const TimeObject&) = delete;
    TimeObject& operator=(const TimeObject&) = delete;

    TimeObject(TimeObject&&) = delete;
    TimeObject& operator=(TimeObject&&) = delete;

    std::unique_ptr<RuntimeObject> clone() const override;

    std::int64_t getValue() const noexcept;
    void setValue(
        std::int64_t value
    ) noexcept;

    std::int64_t getyear() const;
    std::int64_t getmonth() const;
    std::int64_t getday() const;
    std::int64_t gethour() const;
    std::int64_t getmin() const;
    std::int64_t getsec() const;
    std::int64_t getms() const;
    std::int64_t getns() const;

private:

    std::int64_t value;
};

#endif