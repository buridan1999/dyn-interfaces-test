// driver-interface-test.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <memory>
#include <cassert>
#include <windows.h>

#include "..\driver-connector\driver-connector.h"



int main()
{
    std::cout << "Hello World!\n";

    LibCtx libCtx;

    if (!DriverConnectorLibInit(&libCtx, sizeof(LibCtx))) {

        std::cerr << "DriverConnectorLibInit() failed" << std::endl;
        return 1;
    }

    //
    // =======================================================
    //

    {
        using namespace dc_v2;

        if (!dc_version_check(DriverConnectorVersion())) {

            std::cerr << "dc_version_check() failed" << std::endl;
            return 2;
        }

        assert(dc_version_check(dc_v1::DriverConnectorVersion()) == false);

        auto eventObject = GetEvent(); // TODO: Get\Create some interface for reading events

        //
        // 1. Geting event interface
        //
        
        // Example 1
        IEventProcessCreate0* eventProcessCreate_t1 = nullptr;
        if (dr_error_t error = eventObject->GetInterface<IEventProcessCreate0>(&eventProcessCreate_t1)) {

            std::cerr << "GetInterface<IEventProcessCreate0>() failed, error: " << error << std::endl;
            return 3;
        }

        // Example 2
        auto eventProcessCreate_t2 = eventObject->GetInterface<IEventProcessCreate0>(); // throws

        // Example 3
        auto eventProcessCreate_t3 = eventObject->GetInterface<IEventProcessCreate0>(std::nothrow); // nothrow

        // Example 4
        dr_error_t error = 1;
        auto eventProcessCreate_t4 = eventObject->GetInterface<IEventProcessCreate0>(&error); // nothrow



        //
        // 2. using interface
        //
        
        // test {
        std::cout << "event[ChildProcessImageName]: " << eventProcessCreate_t1->GetChildProcessImageName() << std::endl;
        std::cout << "event[ChildProcessImageName]: " << eventProcessCreate_t2->GetChildProcessImageName() << std::endl;
        std::cout << "event[ChildProcessImageName]: " << eventProcessCreate_t3->GetChildProcessImageName() << std::endl;
        std::cout << "event[ChildProcessImageName]: " << eventProcessCreate_t4->GetChildProcessImageName() << std::endl;
        // }

        // test another interface version
        auto eventProcessCreate1 = eventObject->GetInterface<IEventProcessCreate1>();
        std::cout << "event[ChildProcessImageName]: " << eventProcessCreate1->GetChildProcessImageName() << std::endl;
        std::cout << "event[ChildProcessImageNameEx]: " << eventProcessCreate1->GetChildProcessImageNameEx() << std::endl;
    }
    return 0;
}
