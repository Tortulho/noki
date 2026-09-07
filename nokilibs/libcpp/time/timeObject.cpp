#include "timeObject.hpp"

#include "runtimeobjectManager.hpp"

#include <ctime>
#include <stdexcept>

namespace
{
    constexpr std::int64_t NS_PER_MS =
        1'000'000;

    constexpr std::int64_t NS_PER_SEC =
        1'000'000'000;

    std::tm getLocalTime(
        std::int64_t value
    )
    {
        const std::time_t timestamp =
            static_cast<std::time_t>(
                value / NS_PER_SEC
            );

        std::tm result{};

#if defined(_WIN32)

        if (localtime_s(
                &result,
                &timestamp
            ) != 0)
        {
            throw std::runtime_error(
                "Failed to convert time."
            );
        }

#else

        if (localtime_r(
                &timestamp,
                &result
            ) == nullptr)
        {
            throw std::runtime_error(
                "Failed to convert time."
            );
        }

#endif

        return result;
    }

    std::int64_t getSubsecond(
        std::int64_t value
    )
    {
        std::int64_t result =
            value % NS_PER_SEC;

        if (result < 0)
            result += NS_PER_SEC;

        return result;
    }
}

TimeObject::TimeObject(
    RuntimeObjectManager* manager,
    RuntimeObjectTypeID typeID,
    std::int64_t value
)
    : RuntimeObject(
        manager,
        typeID
    ),
      value(value)
{
}

std::unique_ptr<RuntimeObject>
TimeObject::clone() const
{
    RuntimeObjectManager* manager =
        getObjectManager();

    if (manager == nullptr)
    {
        return std::make_unique<TimeObject>(
            nullptr,
            getTypeID(),
            value
        );
    }

    return manager->createObject<TimeObject>(
        getTypeID(),
        value
    );
}

std::int64_t TimeObject::getValue() const noexcept
{
    return value;
}

void TimeObject::setValue(
    std::int64_t newValue
) noexcept
{
    value = newValue;
}

std::int64_t TimeObject::getyear() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_year + 1900
    );
}

std::int64_t TimeObject::getmonth() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_mon + 1
    );
}

std::int64_t TimeObject::getday() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_mday
    );
}

std::int64_t TimeObject::gethour() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_hour
    );
}

std::int64_t TimeObject::getmin() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_min
    );
}

std::int64_t TimeObject::getsec() const
{
    return static_cast<std::int64_t>(
        getLocalTime(value).tm_sec
    );
}

std::int64_t TimeObject::getms() const
{
    return getSubsecond(value) /
           NS_PER_MS;
}

std::int64_t TimeObject::getns() const
{
    return getSubsecond(value) %
           NS_PER_MS;
}