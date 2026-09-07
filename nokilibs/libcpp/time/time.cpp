#include "time.hpp"
#include "timeObject.hpp"

#include "runtime.hpp"
#include "runtimeobjectTypeInitializer.hpp"
#include "libinitializer.hpp"

#include <chrono>
#include <ctime>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


namespace
{

    constexpr std::int64_t NS_PER_SEC  = 1'000'000'000LL;
    constexpr std::int64_t NS_PER_MS   = 1'000'000LL;
    constexpr std::int64_t NS_PER_MIN  = 60LL * NS_PER_SEC;
    constexpr std::int64_t NS_PER_HOUR = 60LL * NS_PER_MIN;
    constexpr std::int64_t NS_PER_DAY  = 24LL * NS_PER_HOUR;


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

        if (
            localtime_s(
                &result,
                &timestamp
            ) != 0
        )
        {
            throw std::runtime_error(
                "Failed to convert time."
            );
        }

#else

        if (
            localtime_r(
                &timestamp,
                &result
            ) == nullptr
        )
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


    void checkRange(
        std::int64_t value,
        std::int64_t min,
        std::int64_t max,
        const char* name
    )
    {
        if (
            value < min ||
            value > max
        )
        {
            throw std::runtime_error(
                std::string(name) +
                " is out of range."
            );
        }
    }


    TimeObject* getTimeObject(
        RuntimeValue* value
    )
    {
        if (value == nullptr)
            return nullptr;

        if (!value->isObject())
            return nullptr;

        return dynamic_cast<TimeObject*>(
            value->getObject()
        );
    }


    void setCalendar(
        TimeObject& time,
        std::int64_t year,
        std::int64_t month,
        std::int64_t day,
        std::int64_t hour,
        std::int64_t min,
        std::int64_t sec,
        std::int64_t ms,
        std::int64_t ns
    )
    {
        checkRange(
            month,
            1,
            12,
            "month"
        );

        checkRange(
            day,
            1,
            31,
            "day"
        );

        checkRange(
            hour,
            0,
            23,
            "hour"
        );

        checkRange(
            min,
            0,
            59,
            "min"
        );

        checkRange(
            sec,
            0,
            59,
            "sec"
        );

        checkRange(
            ms,
            0,
            999,
            "ms"
        );

        checkRange(
            ns,
            0,
            999999,
            "ns"
        );

        std::tm tmValue{};

        tmValue.tm_year =
            static_cast<int>(
                year - 1900
            );

        tmValue.tm_mon =
            static_cast<int>(
                month - 1
            );

        tmValue.tm_mday =
            static_cast<int>(day);

        tmValue.tm_hour =
            static_cast<int>(hour);

        tmValue.tm_min =
            static_cast<int>(min);

        tmValue.tm_sec =
            static_cast<int>(sec);

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to construct time."
            );
        }

        time.setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            ms * NS_PER_MS +
            ns
        );
    }

}


class TimeLibraryInitializer final
    : public LibraryInitializer,
      public RuntimeObjectTypeInitializer
{

public:

    explicit TimeLibraryInitializer(
        RuntimeObjectManager& manager
    )
        : RuntimeObjectTypeInitializer(
            manager,
            "time"
        )
    {
        RuntimeObjectType* type =
            getType();

        if (type == nullptr)
        {
            throw std::runtime_error(
                "Failed to initialize time type."
            );
        }


        type->registerMethod(
            "getyear",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getyear
                )
            }
        );

        type->registerMethod(
            "getmonth",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getmonth
                )
            }
        );

        type->registerMethod(
            "getday",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getday
                )
            }
        );

        type->registerMethod(
            "gethour",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::gethour
                )
            }
        );

        type->registerMethod(
            "getmin",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getmin
                )
            }
        );

        type->registerMethod(
            "getsec",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getsec
                )
            }
        );

        type->registerMethod(
            "getms",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getms
                )
            }
        );

        type->registerMethod(
            "getns",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::getns
                )
            }
        );


        type->registerMethod(
            "set",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    static_cast<
                        RuntimeValue (*)(
                            std::vector<RuntimeValue*>&
                        )
                    >(nokitime::set)
                )
            }
        );


        type->registerMethod(
            "setin_ns",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_ns
                )
            }
        );

        type->registerMethod(
            "setin_ms",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_ms
                )
            }
        );

        type->registerMethod(
            "setin_sec",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_sec
                )
            }
        );

        type->registerMethod(
            "setin_min",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_min
                )
            }
        );

        type->registerMethod(
            "setin_hour",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_hour
                )
            }
        );

        type->registerMethod(
            "setin_day",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_day
                )
            }
        );

        type->registerMethod(
            "setin_month",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::setin_month
                )
            }
        );


        type->registerMethod(
            "addin_ns",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_ns
                )
            }
        );

        type->registerMethod(
            "addin_ms",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_ms
                )
            }
        );

        type->registerMethod(
            "addin_sec",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_sec
                )
            }
        );

        type->registerMethod(
            "addin_min",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_min
                )
            }
        );

        type->registerMethod(
            "addin_hour",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_hour
                )
            }
        );

        type->registerMethod(
            "addin_day",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_day
                )
            }
        );

        type->registerMethod(
            "addin_month",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_month
                )
            }
        );

        type->registerMethod(
            "addin_year",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::addin_year
                )
            }
        );


        type->registerMethod(
            "formated",
            BuiltinEntry{
                static_cast<BuiltinMutFunc>(
                    nokitime::formated
                )
            }
        );
    }
};


namespace nokitime
{

    std::unique_ptr<LibraryInitializer>
    createInitializer(
        RuntimeObjectManager& manager
    )
    {
        return std::make_unique<
            TimeLibraryInitializer
        >(
            manager
        );
    }


    RuntimeValue now(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.now() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        RuntimeObjectType* type =
            runtime.objects.findType(
                "time"
            );

        if (type == nullptr)
        {
            throw std::runtime_error(
                "time type is not initialized."
            );
        }

        auto object =
            runtime.objects.createObject<
                TimeObject
            >(
                type->getID(),
                value
            );

        return RuntimeValue::fromObject(
            std::move(object)
        );
    }


    RuntimeValue getrealtimeyear(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.getrealtimeyear() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        const std::tm tmValue =
            getLocalTime(value);

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_year + 1900
            )
        );
    }


    RuntimeValue getrealtimemonth(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.getrealtimemonth() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        const std::tm tmValue =
            getLocalTime(value);

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_mon + 1
            )
        );
    }


    RuntimeValue getrealtimeday(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.getrealtimeday() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        const std::tm tmValue =
            getLocalTime(value);

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_mday
            )
        );
    }


    RuntimeValue getrealtimemin(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.getrealtimemin() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        const std::tm tmValue =
            getLocalTime(value);

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_min
            )
        );
    }


    RuntimeValue getrealtimesec(
        const std::vector<RuntimeValue>& args
    )
    {
        if (!args.empty())
        {
            throw std::runtime_error(
                "time.getrealtimesec() expects no arguments."
            );
        }

        const auto current =
            std::chrono::system_clock::now();

        const auto duration =
            current.time_since_epoch();

        const std::int64_t value =
            std::chrono::duration_cast<
                std::chrono::nanoseconds
            >(
                duration
            ).count();

        const std::tm tmValue =
            getLocalTime(value);

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_sec
            )
        );
    }


    RuntimeValue getyear(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getyear() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getyear() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_year + 1900
            )
        );
    }


    RuntimeValue getmonth(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getmonth() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getmonth() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_mon + 1
            )
        );
    }


    RuntimeValue getday(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getday() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getday() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_mday
            )
        );
    }


    RuntimeValue gethour(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.gethour() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.gethour() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_hour
            )
        );
    }


    RuntimeValue getmin(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getmin() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getmin() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_min
            )
        );
    }


    RuntimeValue getsec(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getsec() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getsec() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        return RuntimeValue(
            static_cast<std::int64_t>(
                tmValue.tm_sec
            )
        );
    }


    RuntimeValue getms(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getms() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getms() receiver is not a time object."
            );
        }

        return RuntimeValue(
            getSubsecond(
                time->getValue()
            ) / NS_PER_MS
        );
    }


    RuntimeValue getns(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.getns() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.getns() receiver is not a time object."
            );
        }

        return RuntimeValue(
            getSubsecond(
                time->getValue()
            ) % NS_PER_MS
        );
    }


    /*
     * t.set(...)
     */
    RuntimeValue set(
        std::vector<RuntimeValue*>& args
    )
    {
        if (
            args.size() != 2 &&
            args.size() != 9
        )
        {
            throw std::runtime_error(
                "time.set() expects 1 or 8 arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.set() receiver is not a time object."
            );
        }


        /*
         * t.set(time)
         */
        if (args.size() == 2)
        {
            RuntimeValue* value =
                args[1];

            if (value == nullptr)
            {
                throw std::runtime_error(
                    "time.set() argument cannot be null."
                );
            }


            if (value->isObject())
            {
                TimeObject* source =
                    dynamic_cast<TimeObject*>(
                        value->getObject()
                    );

                if (source == nullptr)
                {
                    throw std::runtime_error(
                        "time.set() argument must be a time object."
                    );
                }

                time->setValue(
                    source->getValue()
                );

                return RuntimeValue(nullptr);
            }


            /*
             * t.set(int64)
             */
            if (
                value->is<std::int64_t>()
            )
            {
                time->setValue(
                    value->get<std::int64_t>()
                );

                return RuntimeValue(nullptr);
            }


            /*
             * t.set(vector)
             */
            if (value->isVector())
            {
                RuntimeValue_Vector* vector =
                    value->getVector();

                if (
                    vector == nullptr ||
                    vector->size() != 8
                )
                {
                    throw std::runtime_error(
                        "time.set() vector must contain 8 values."
                    );
                }

                for (std::size_t i = 0; i < 8; ++i)
                {
                    if (
                        !vector->at(i).is<std::int64_t>()
                    )
                    {
                        throw std::runtime_error(
                            "time.set() vector values must be integers."
                        );
                    }
                }

                setCalendar(
                    *time,
                    vector->at(0).get<std::int64_t>(),
                    vector->at(1).get<std::int64_t>(),
                    vector->at(2).get<std::int64_t>(),
                    vector->at(3).get<std::int64_t>(),
                    vector->at(4).get<std::int64_t>(),
                    vector->at(5).get<std::int64_t>(),
                    vector->at(6).get<std::int64_t>(),
                    vector->at(7).get<std::int64_t>()
                );

                return RuntimeValue(nullptr);
            }

            throw std::runtime_error(
                "time.set() argument must be a time object, integer or vector."
            );
        }


        /*
         * t.set(
         *     year,
         *     month,
         *     day,
         *     hour,
         *     min,
         *     sec,
         *     ms,
         *     ns
         * )
         */
        for (std::size_t i = 1; i < 9; ++i)
        {
            if (args[i] == nullptr)
            {
                throw std::runtime_error(
                    "time.set() arguments cannot be null."
                );
            }

            if (
                !args[i]->is<std::int64_t>()
            )
            {
                throw std::runtime_error(
                    "time.set() arguments must be integers."
                );
            }
        }

        setCalendar(
            *time,
            args[1]->get<std::int64_t>(),
            args[2]->get<std::int64_t>(),
            args[3]->get<std::int64_t>(),
            args[4]->get<std::int64_t>(),
            args[5]->get<std::int64_t>(),
            args[6]->get<std::int64_t>(),
            args[7]->get<std::int64_t>(),
            args[8]->get<std::int64_t>()
        );

        return RuntimeValue(nullptr);
    }


    /*
     * time.set(...)
     */
    RuntimeValue set(
        const std::vector<RuntimeValue>& args
    )
    {
        if (
            args.size() != 1 &&
            args.size() != 8
        )
        {
            throw std::runtime_error(
                "time.set() expects 1 or 8 arguments."
            );
        }

        RuntimeObjectType* type =
            runtime.objects.findType(
                "time"
            );

        if (type == nullptr)
        {
            throw std::runtime_error(
                "time type is not initialized."
            );
        }


        /*
         * time.set(time)
         */
        if (args.size() == 1)
        {
            if (args[0].isObject())
            {
                const TimeObject* source =
                    dynamic_cast<const TimeObject*>(
                        args[0].getObject()
                );

                if (source == nullptr)
                {
                    throw std::runtime_error(
                        "time.set() argument must be a time object, integer or vector."
                    );
                }

                auto object =
                    runtime.objects.createObject<
                        TimeObject
                    >(
                        type->getID(),
                        source->getValue()
                    );

                return RuntimeValue::fromObject(
                    std::move(object)
                );
            }


            /*
             * time.set(int64)
             */
            if (
                args[0].is<std::int64_t>()
            )
            {
                auto object =
                    runtime.objects.createObject<
                        TimeObject
                    >(
                        type->getID(),
                        args[0].get<std::int64_t>()
                    );

                return RuntimeValue::fromObject(
                    std::move(object)
                );
            }


            /*
             * time.set(vector)
             */
            if (args[0].isVector())
            {
                const RuntimeValue_Vector* vector =
                    args[0].getVector();

                if (
                    vector == nullptr ||
                    vector->size() != 8
                )
                {
                    throw std::runtime_error(
                        "time.set() vector must contain 8 values."
                    );
                }

                for (std::size_t i = 0; i < 8; ++i)
                {
                    if (
                        !vector->at(i).is<std::int64_t>()
                    )
                    {
                        throw std::runtime_error(
                            "time.set() vector values must be integers."
                        );
                    }
                }

                auto object =
                    runtime.objects.createObject<
                        TimeObject
                    >(
                        type->getID(),
                        0
                    );

                setCalendar(
                    *object,
                    vector->at(0).get<std::int64_t>(),
                    vector->at(1).get<std::int64_t>(),
                    vector->at(2).get<std::int64_t>(),
                    vector->at(3).get<std::int64_t>(),
                    vector->at(4).get<std::int64_t>(),
                    vector->at(5).get<std::int64_t>(),
                    vector->at(6).get<std::int64_t>(),
                    vector->at(7).get<std::int64_t>()
                );

                return RuntimeValue::fromObject(
                    std::move(object)
                );
            }

            throw std::runtime_error(
                "time.set() argument must be a time object, integer or vector."
            );
        }


        /*
         * time.set(
         *     year,
         *     month,
         *     day,
         *     hour,
         *     min,
         *     sec,
         *     ms,
         *     ns
         * )
         */
        for (const RuntimeValue& value : args)
        {
            if (
                !value.is<std::int64_t>()
            )
            {
                throw std::runtime_error(
                    "time.set() arguments must be integers."
                );
            }
        }

        auto object =
            runtime.objects.createObject<
                TimeObject
            >(
                type->getID(),
                0
            );

        setCalendar(
            *object,
            args[0].get<std::int64_t>(),
            args[1].get<std::int64_t>(),
            args[2].get<std::int64_t>(),
            args[3].get<std::int64_t>(),
            args[4].get<std::int64_t>(),
            args[5].get<std::int64_t>(),
            args[6].get<std::int64_t>(),
            args[7].get<std::int64_t>()
        );

        return RuntimeValue::fromObject(
            std::move(object)
        );
    }


    RuntimeValue setin_ns(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_ns() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_ns() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_ns() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_ns() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            0,
            999999999,
            "ns"
        );

        time->setValue(
            time->getValue() -
            getSubsecond(time->getValue()) +
            value
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_ms(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_ms() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_ms() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_ms() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_ms() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            0,
            999,
            "ms"
        );

        const std::int64_t subsecond =
            getSubsecond(
                time->getValue()
            );

        time->setValue(
            time->getValue() -
            subsecond +
            value * NS_PER_MS +
            (subsecond % NS_PER_MS)
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_sec(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_sec() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_sec() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_sec() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_sec() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            0,
            60,
            "sec"
        );

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_sec =
            static_cast<int>(value);

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to set seconds."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_min(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_min() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_min() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_min() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_min() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            0,
            59,
            "min"
        );

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_min =
            static_cast<int>(value);

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to set minutes."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_hour(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_hour() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_hour() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_hour() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_hour() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            0,
            23,
            "hour"
        );

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_hour =
            static_cast<int>(value);

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to set hour."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_day(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_day() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_day() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_day() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_day() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            1,
            31,
            "day"
        );

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_mday =
            static_cast<int>(value);

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to set day."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue setin_month(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.setin_month() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.setin_month() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.setin_month() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.setin_month() expects an integer."
            );
        }

        const std::int64_t value =
            args[1]->get<std::int64_t>();

        checkRange(
            value,
            1,
            12,
            "month"
        );

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_mon =
            static_cast<int>(
                value - 1
            );

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to set month."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_ns(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_ns() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_ns() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_ns() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_ns() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>()
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_ms(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_ms() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_ms() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_ms() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_ms() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>() *
                NS_PER_MS
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_sec(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_sec() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_sec() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_sec() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_sec() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>() *
                NS_PER_SEC
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_min(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_min() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_min() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_min() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_min() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>() *
                NS_PER_MIN
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_hour(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_hour() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_hour() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_hour() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_hour() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>() *
                NS_PER_HOUR
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_day(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_day() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_day() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_day() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_day() expects an integer."
            );
        }

        time->setValue(
            time->getValue() +
            args[1]->get<std::int64_t>() *
                NS_PER_DAY
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_month(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_month() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_month() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_month() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_month() expects an integer."
            );
        }

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_mon +=
            static_cast<int>(
                args[1]->get<std::int64_t>()
            );

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to add months."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }


    RuntimeValue addin_year(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 2)
        {
            throw std::runtime_error(
                "time.addin_year() expects 1 argument."
            );
        }

        if (
            args[0] == nullptr ||
            args[1] == nullptr
        )
        {
            throw std::runtime_error(
                "time.addin_year() received a null argument."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.addin_year() receiver is not a time object."
            );
        }

        if (
            !args[1]->is<std::int64_t>()
        )
        {
            throw std::runtime_error(
                "time.addin_year() expects an integer."
            );
        }

        std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        tmValue.tm_year +=
            static_cast<int>(
                args[1]->get<std::int64_t>()
            );

        tmValue.tm_isdst = -1;

        const std::time_t timestamp =
            std::mktime(
                &tmValue
            );

        if (
            timestamp ==
            static_cast<std::time_t>(-1)
        )
        {
            throw std::runtime_error(
                "Failed to add years."
            );
        }

        time->setValue(
            static_cast<std::int64_t>(
                timestamp
            ) * NS_PER_SEC +
            getSubsecond(
                time->getValue()
            )
        );

        return RuntimeValue(nullptr);
    }

    RuntimeValue formated(
        std::vector<RuntimeValue*>& args
    )
    {
        if (args.size() != 1)
        {
            throw std::runtime_error(
                "time.formated() expects no arguments."
            );
        }

        TimeObject* time =
            getTimeObject(args[0]);

        if (time == nullptr)
        {
            throw std::runtime_error(
                "time.formated() receiver is not a time object."
            );
        }

        const std::tm tmValue =
            getLocalTime(
                time->getValue()
            );

        const std::int64_t subsecond =
            getSubsecond(
                time->getValue()
            );

        auto vector =
            std::make_unique<RuntimeValue_Vector>();

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_year + 1900
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_mon + 1
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_mday
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_hour
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_min
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                static_cast<std::int64_t>(
                    tmValue.tm_sec
                )
            )
        );

        vector->pushback(
            RuntimeValue(
                subsecond / NS_PER_MS
            )
        );

        vector->pushback(
            RuntimeValue(
                subsecond % NS_PER_MS
            )
        );

        return RuntimeValue(
            std::move(vector)
        );
    }

}


Library createTimeLibrary()
{
    return Library
    {
        "time",
        {
            {
                "now",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::now
                    )
                }
            },
            {
                "set",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        static_cast<
                            RuntimeValue (*)(
                                const std::vector<RuntimeValue>&
                            )
                        >(nokitime::set)
                    )
                }
            },
            {
                "getrealtimeyear",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::getrealtimeyear
                    )
                }
            },
            {
                "getrealtimemonth",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::getrealtimemonth
                    )
                }
            },
            {
                "getrealtimeday",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::getrealtimeday
                    )
                }
            },
            {
                "getrealtimemin",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::getrealtimemin
                    )
                }
            },
            {
                "getrealtimesec",
                BuiltinEntry{
                    static_cast<BuiltinFunc>(
                        nokitime::getrealtimesec
                    )
                }
            }
        },
        {},
        nokitime::createInitializer
    };
}
