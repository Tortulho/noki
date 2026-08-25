#ifndef RUNTIMEOBJECTTYPEINITIALIZER
#define RUNTIMEOBJECTTYPEINITIALIZER

#include "runtimeobjectManager.hpp"

/*
Se houver uma instância viva quando o initializer morrer isto 
representa uma violação do lifetime/ownership que temos de corrigir
não algo que devemos esconder deixando um tipo registado sem owner. (mem leak)
*/

class RuntimeObjectTypeInitializer
{
public:
    RuntimeObjectTypeInitializer(
        RuntimeObjectManager& manager,
        const std::string& name
    );

    ~RuntimeObjectTypeInitializer();

    RuntimeObjectTypeInitializer(
        const RuntimeObjectTypeInitializer&
    ) = delete;

    RuntimeObjectTypeInitializer& operator=(
        const RuntimeObjectTypeInitializer&
    ) = delete;

    RuntimeObjectTypeInitializer(
        RuntimeObjectTypeInitializer&&
    ) = delete;

    RuntimeObjectTypeInitializer& operator=(
        RuntimeObjectTypeInitializer&&
    ) = delete;

    RuntimeObjectTypeID getTypeID() const noexcept;

    RuntimeObjectType* getType() noexcept;

private:
    RuntimeObjectManager& manager;
    RuntimeObjectTypeID typeID;
};

#endif