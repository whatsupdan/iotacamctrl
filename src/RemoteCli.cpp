#include <cstdlib>
#if defined(USE_EXPERIMENTAL_FS)
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif
#include <iomanip>
#include "CRSDK/CameraRemote_SDK.h"
#include "CameraDevice.h"
#include "Text.h"

namespace SDK = SCRSDK;

// Global dll object
// cli::CRLibInterface* cr_lib = nullptr;

int main()
{
    // Change global locale to native locale
    std::locale::global(std::locale(""));

    // Make the stream's locale the same as the current global locale
    cli::tin.imbue(std::locale());
    cli::tout.imbue(std::locale());

    cli::tout << "RemoteSampleApp v1.1.00 running...\n\n";

    CrInt32u version = SDK::GetSDKVersion();
    int major = (version & 0xFF000000) >> 24;
    int minor = (version & 0x00FF0000) >> 16;
    int patch = (version & 0x0000FF00) >> 8;
    // int reserved = (version & 0x000000FF);

    cli::tout << "Remote SDK version: ";
    cli::tout << major << "." << minor << "." << std::setfill(TEXT('0')) << std::setw(2) << patch << "\n";

    // Load the library dynamically
    // cr_lib = cli::load_cr_lib();

    cli::tout << "Initialize Remote SDK...\n";
    cli::tout << "Working directory: " << fs::current_path() << '\n';
    // auto init_success = cr_lib->Init(0);
    auto init_success = SDK::Init();
    if (!init_success) {
        cli::tout << "Failed to initialize Remote SDK. Terminating.\n";
        // cr_lib->Release();
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }
    cli::tout << "Remote SDK successfully initialized.\n\n";

    cli::tout << "Enumerate connected camera devices...\n";
    SDK::ICrEnumCameraObjectInfo* camera_list = nullptr;
    // auto enum_status = cr_lib->EnumCameraObjects(&camera_list, 3);
    auto enum_status = SDK::EnumCameraObjects(&camera_list);
    if (CR_FAILED(enum_status) || camera_list == nullptr) {
        cli::tout << "No cameras detected. Connect a camera and retry.\n";
        // cr_lib->Release();
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }
    auto ncams = camera_list->GetCount();
    cli::tout << "Camera enumeration successful. " << ncams << " detected.\n\n";

    for (CrInt32u i = 0; i < ncams; ++i) {
        auto camera_info = camera_list->GetCameraObjectInfo(i);
        cli::text conn_type(camera_info->GetConnectionTypeName());
        cli::text model(camera_info->GetModel());

        cli::tout << '[' << i + 1 << ']' << model.data() << '\n';
    }

    cli::tout << "Connect to camera with input number...\n";
    cli::tout << "input> ";
    cli::text connectNo;
    std::getline(cli::tin, connectNo);
    cli::tout << '\n';

    cli::tsmatch smatch;
    CrInt32u no = 0;
    while (true) {
        no = 0;
        if (std::regex_search(connectNo, smatch, cli::tregex(TEXT("[0-9]")))) {
            no = stoi(connectNo);
            if (no == 0)
                break; // finish

            if (camera_list->GetCount() < no) {
                cli::tout << "input value over \n";
                cli::tout << "input> "; // Redo
                std::getline(cli::tin, connectNo);
                continue;
            }
            else
                break; // ok
        }
        else
            break; // not number
    }

    if (no == 0) {
        cli::tout << "Invalid Number. Finish App.\n";
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }

    cli::tout << "Connect to selected camera...\n";
    auto* camera_info = camera_list->GetCameraObjectInfo(no - 1);

    cli::tout << "Create camera SDK camera callback object.\n";
    cli::CameraDevice camera(nullptr, camera_info);

    // cli::tout << "ip addr: " << camera.ip_address() << '\n';
    // cli::tout << "mac addr: " << camera.mac_address().data() << '\n';
    cli::tout << "pid: 0x" << std::hex << camera.pid() << std::dec << '\n';

    cli::tout << "Release enumerated camera list.\n";
    camera_list->Release();

    auto connect_status = camera.connect();
    if (!connect_status) {
        cli::tout << "Camera connection failed to initiate. Abort.\n";
        // cr_lib->Release();
        SDK::Release();
        std::exit(EXIT_FAILURE);
    }
    cli::tout << "Camera connection successfully initiated!\n\n";

    // Interactive loop
    for (;;) {
        cli::tout << "What would you like to do? Enter the corresponding number.\n";
        cli::tout
            << "(0) Connect / Disconnect \n"
            << "(1) Shutter Release \n"
            << "(2) Shutter Half Release in AF mode \n"
            << "(3) Shutter Half and Full Release in AF mode \n"
            << "(4) Continuous Shooting \n"
            << "(5) Aperture \n"
            << "(6) ISO \n"
            << "(7) Shutter Speed \n"
            << "(8) Live View \n"
            << "(9) Live View Imege Quality \n"
            << "(a) Position Key Setting \n"
            << "(b) Exposure Program Mode \n"
            << "(c) Still Capture Mode(Drive mode) \n"
            << "(d) Focus Mode \n"
            << "(11) FELock \n"
            << "(12) AWBLock \n"
            << "(13) AF Area Position(x,y) \n"
            << "(14) Selected MediaFormat \n"
            << "(15) Movie Rec Button \n"
            << "(16) White Balance \n"
            << "(17) Custom WB \n"
            << "(18) Zoom Operation \n"
            << "(x) Exit\n";

        cli::tout << "input> ";
        cli::text action;
        std::getline(cli::tin, action);
        cli::tout << '\n';

        if (action == TEXT("x")) { /* Exit app */
            if (camera.connected()) {
                cli::tout << "Initiate disconnect sequence.\n";
                auto disconnect_status = camera.disconnect();
                if (!disconnect_status) {
                    // try again
                    disconnect_status = camera.disconnect();
                }

                if (!disconnect_status)
                    cli::tout << "Disconnect failed to initiate.\n";
                else
                    cli::tout << "Disconnect successfully initiated!\n\n";
            }
            break; // quit application loop
        }
        else if (action == TEXT("0")) { /* disconnect & reconnect */
            if (camera.connected()) {
                camera.disconnect();
            }
            else {
                camera.connect();
            }
        }
        else if (action == TEXT("1")) { /* Take photo */
            camera.capture_image();
        }
        else if (action == TEXT("2")) { /* S1 Shooting */
            camera.s1_shooting();
        }
        else if (action == TEXT("3")) { /* AF Shutter */
            camera.af_shutter();
        }
        else if (action == TEXT("4")) { /* Continuous Shooting */
            camera.continuous_shooting();
        }
        else if (action == TEXT("5")) { /* Aperture. */
            camera.get_aperture();
            camera.set_aperture();
        }
        else if (action == TEXT("6")) { /* ISO */
            camera.get_iso();
            camera.set_iso();
        }
        else if (action == TEXT("7")) { /* Shutter Speed */
            camera.get_shutter_speed();
            camera.set_shutter_speed();
        }
        else if (action == TEXT("8")) { /* Live View */
            camera.get_live_view();
        }
        else if (action == TEXT("9")) { /* Live View Image Quality */
            camera.get_live_view_image_quality();
            camera.set_live_view_image_quality();
        }
        else if (action == TEXT("10")) { /* Live View Image Status */
            camera.get_live_view_status();
            camera.set_live_view_status();
        }
        else if (action == TEXT("a")) { /* Position Key Setting */
            camera.get_position_key_setting();
            camera.set_position_key_setting();
        }
        else if (action == TEXT("b")) { /* Exposure Program Mode */
            camera.get_exposure_program_mode();
            camera.set_exposure_program_mode();
        }
        else if (action == TEXT("c")) { /* Still Capture Mode(Drive mode) */
            camera.get_still_capture_mode();
            camera.set_still_capture_mode();
        }
        else if (action == TEXT("d")) { /* Focus Mode */
            camera.get_focus_mode();
            camera.set_focus_mode();
        }
        else if (action == TEXT("11")) { /* FELock */
            cli::tout << "Flash device required.";
            camera.execute_lock_property((CrInt16u)SDK::CrDevicePropertyCode::CrDeviceProperty_FEL);
        }
        else if (action == TEXT("12")) { /* AWBLock */
            camera.execute_lock_property((CrInt16u)SDK::CrDevicePropertyCode::CrDeviceProperty_AWBL);
        }
        else if (action == TEXT("13")) { /* AF Area Position(x,y) */
            camera.set_af_area_position();
        }
        else if (action == TEXT("14")) { /* Selected MediaFormat */
            camera.get_select_media_format();
            camera.set_select_media_format();
        }
        else if (action == TEXT("15")) { /* Movie Rec Button */
            camera.execute_movie_rec();
        }
        else if (action == TEXT("16")) { /* White Balance */
            camera.get_white_balance();
            camera.set_white_balance();
        }
        else if (action == TEXT("17")) { /* Custom WB */
            camera.get_custom_wb();
            camera.set_custom_wb();
        }
        else if (action == TEXT("18")) { /* Zoom Operation */
            camera.get_zoom_operation();
            camera.set_zoom_operation();
        }

        cli::tout << '\n';
    }

    cli::tout << "Release SDK resources.\n";
    // cr_lib->Release();
    SDK::Release();

    // cli::free_cr_lib(&cr_lib);

    cli::tout << "Exiting application.\n";
    std::exit(EXIT_SUCCESS);
}
