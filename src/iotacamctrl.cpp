#include <cstdlib>
#include <iostream>
#include <string>
#include <pybind11/pybind11.h>
#include "CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "ICrCameraObjectInfo.h"

namespace SDK = SCRSDK;
namespace py = pybind11;

std::tuple<int, int, int, int> GetSDKVersion()
{
	CrInt32u version = SDK::GetSDKVersion();
	int major = (version & 0xFF000000) >> 24;
	int minor = (version & 0x00FF0000) >> 16;
	int patch = (version & 0x0000FF00) >> 8;
	int service = (version & 0x000000FF);
	return std::make_tuple(major, minor, patch, service);
}


void SDKInit()
{
	// std::cout << "Initialize Remote SDK...\n";
	auto init_success = SDK::Init();
	if (!init_success) {
		SDK::Release();
		 std::exit(EXIT_FAILURE);
		 std::cout << "Failed to initialize Remote SDK. Terminating.";
//		throw std::exception("Failed to initialize Remote SDK. Terminating.");
	}
	 std::cout << "Remote SDK successfully initialized.";
}


void SDKRelease()
{
	SDK::Release();
	 std::cout << "SDK resources released.";
}



PYBIND11_MODULE(iotacamctrl, m) {
    m.doc() = "iotacamctrl - a Sony camera api"; // Optional module docstring
    m.def("SDK_GetSDKVersion", &GetSDKVersion, "Get Remote SDK version");
    m.def("SDK_Init", &SDKInit, "Initialize Remote SDK");
    m.def("SDK_Release", &SDKRelease, "Release Remote SDK");

	

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}