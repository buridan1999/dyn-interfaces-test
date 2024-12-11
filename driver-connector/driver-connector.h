// Приведенный ниже блок ifdef — это стандартный метод создания макросов, упрощающий процедуру
// экспорта из библиотек DLL. Все файлы данной DLL скомпилированы с использованием символа DRIVERCONNECTOR_EXPORTS
// Символ, определенный в командной строке. Этот символ не должен быть определен в каком-либо проекте,
// использующем данную DLL. Благодаря этому любой другой проект, исходные файлы которого включают данный файл, видит
// функции DRIVERCONNECTOR_API как импортированные из DLL, тогда как данная DLL видит символы,
// определяемые данным макросом, как экспортированные.
#ifdef DRIVERCONNECTOR_EXPORTS
#define DRIVERCONNECTOR_API __declspec(dllexport)
#else
#define DRIVERCONNECTOR_API __declspec(dllimport)
#endif

#define C_PUBLIC extern "C"

#include <string_view>
#include <memory>
#include <cassert>
#include <stdexcept>

using dr_error_t = int;

class IObject0
{
public:
	virtual void Release() = 0;

	template<typename Interface>
	dr_error_t GetInterface(_Out_ Interface** ppi)
	{
		assert(ppi);
		if (!ppi) {

			return 1;
		}

		auto pi = dynamic_cast<Interface*>(this);
		if (!pi) {

			return 2;
		}

		*ppi = pi;
		return 0;
	}

	template<typename Interface>
	Interface* GetInterface(_Out_ dr_error_t* perror)
	{
		assert(perror);
		if (!perror) {

			return nullptr;
		}

		*perror = 0;

		Interface* pi = nullptr;
		if (GetInterface(&pi)) {

			*perror = 1;
			return nullptr;
		}

		// assert(pi);
		return pi;
	}

	template<typename Interface>
	Interface* GetInterface()
	{
		dr_error_t error;
		auto interface_ptr = GetInterface<Interface>(&error);
		if (error) {

			// TODO: implement exception with dr_error_t inside
			throw std::runtime_error("no_such_interface");
		}

		return interface_ptr;
	}

	template<typename Interface>
	Interface* GetInterface(const std::nothrow_t)
	{
		dr_error_t error;
		return GetInterface<Interface>(&error);
	}

private:
	// 7 entry reserved in virtual table [ 1 + 7 -> 8 ]
	inline virtual void Reserved0() {};
	inline virtual void Reserved1() {};
	inline virtual void Reserved2() {};
	inline virtual void Reserved3() {};
	inline virtual void Reserved4() {};
	inline virtual void Reserved5() {};
	inline virtual void Reserved6() {};
};

namespace dc_v1
{
	constexpr std::string_view inline DriverConnectorVersion() { return "dc_v1"; };

	class IEventProcessCreate0
	{
	public:
		virtual std::string_view GetChildProcessImageName() = 0;
	};

	class IEventProcessCreate1 : public IEventProcessCreate0
	{
	public:
		virtual std::string_view GetChildProcessImageNameEx() = 0;
	};

};

namespace dc_v2
{
	constexpr std::string_view inline DriverConnectorVersion() { return "dc_v2"; };

	class IEventProcessCreate0
	{
	public:
		virtual std::string_view GetChildProcessImageName() = 0; // some another inerface api compare to dc_v1
	};

	class IEventProcessCreate1 : public IEventProcessCreate0
	{
	public:
		virtual std::string_view GetChildProcessImageNameEx() = 0;  // some another inerface api compare to dc_v1
	};

};

using Cdriverconnector_GetEvent_Fn = IObject0 * (*)();
C_PUBLIC DRIVERCONNECTOR_API IObject0* Cdriverconnector_GetEvent();

using dc_version_check_Fn = bool(*)(std::string_view dc_version);
C_PUBLIC DRIVERCONNECTOR_API bool _dc_version_check(std::string_view dc_version);

using dc_version_get_Fn = std::string_view(*)();
C_PUBLIC DRIVERCONNECTOR_API std::string_view _dc_version_get();

/// client data

static HMODULE driverConnectorLib = NULL;

struct LibCtx
{
	Cdriverconnector_GetEvent_Fn Cdriverconnector_GetEvent;
	dc_version_check_Fn dc_version_check;
} ctx;

inline std::shared_ptr<IObject0> GetEvent()
{
	assert(driverConnectorLib);

	auto eventPtr = ctx.Cdriverconnector_GetEvent();

	return std::shared_ptr<IObject0>(
		eventPtr,
		[](IObject0* _this) { _this->Release(); }
	);
}

inline bool dc_version_check(std::string_view dc_version)
{
	return ctx.dc_version_check(dc_version);
}

inline bool DriverConnectorLibInit(LibCtx* ctxptr, size_t ctxSize)
{
	if (sizeof(LibCtx) != ctxSize) {

		SetLastError(ERROR_INVALID_VARIANT);
		return false;
	}

	// TODO: driverconnector.dll
	std::string driverConnectorPath = "driverconnector.dll";

	driverConnectorLib = LoadLibraryA(driverConnectorPath.c_str());
	if (driverConnectorLib == NULL) {

		return false;
	}

	ctx.Cdriverconnector_GetEvent = reinterpret_cast<Cdriverconnector_GetEvent_Fn>(GetProcAddress(driverConnectorLib, "Cdriverconnector_GetEvent"));
	if (!ctx.Cdriverconnector_GetEvent) {

		return false;
	}

	ctx.dc_version_check = reinterpret_cast<dc_version_check_Fn>(GetProcAddress(driverConnectorLib, "_dc_version_check"));
	if (!ctx.dc_version_check) {

		return false;
	}

	*ctxptr = ctx;
	return true;
}