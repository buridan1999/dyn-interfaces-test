// driver-connector.cpp : Определяет экспортируемые функции для DLL.
//

#include "pch.h"
#include "framework.h"
#include "driver-connector.h"

#include <iostream>

using namespace dc_v2;

class EventProcessCreate : 
    virtual public IObject0,
    virtual public IEventProcessCreate1
{
public:
    virtual void Release() override
    {
        std::cout << "EventProcessCreate::Release()" << std::endl;
        
        delete this;
    }

    virtual std::string_view GetChildProcessImageName() override
    {
        return std::string_view("%ChildProcessImageName%");
    }

    virtual std::string_view GetChildProcessImageNameEx() override
    {
        return std::string_view("%ChildProcessImageNameEx%");
    }
};

// Пример экспортированной функции.
DRIVERCONNECTOR_API int fndriverconnector(void)
{
    return 0;
}

// Этот класс экспортирован из библиотеки DLL
class DRIVERCONNECTOR_API Cdriverconnector {
public:
    Cdriverconnector(void);
    // TODO: добавьте сюда свои методы.

    IObject0* GetEvent();
};

// Конструктор для экспортированного класса.
Cdriverconnector::Cdriverconnector()
{
    return;
}

IObject0* Cdriverconnector::GetEvent()
{
    auto event = new EventProcessCreate();
    return event;
}

DRIVERCONNECTOR_API IObject0* Cdriverconnector_GetEvent()
{
    return Cdriverconnector().GetEvent();
}

DRIVERCONNECTOR_API bool _dc_version_check(std::string_view dc_version)
{
    return DriverConnectorVersion() == dc_version;
}

DRIVERCONNECTOR_API std::string_view _dc_version_get()
{
    return DriverConnectorVersion();
}