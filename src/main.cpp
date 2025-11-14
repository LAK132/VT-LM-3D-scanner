#include "main.hpp"

#include "leapc_wrapper.hpp"
#include "openvr_wrapper.hpp"

#include <lak/array.hpp>
#include <lak/debug.hpp>
#include <lak/file.hpp>

#include <iostream>

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL
#include <lak/basic_single_window_program.inl>

vr::TrackedDevicePose_t tracked_device_poses[vr::k_unMaxTrackedDeviceCount];
glm::mat4 device_poses[vr::k_unMaxTrackedDeviceCount];
vr::ETrackedDeviceClass device_classes[vr::k_unMaxTrackedDeviceCount];

void imgui_matrix(const char *str, const glm::mat4 &matrix)
{
	ImGui::Text("%s", str);
	auto m = glm::transpose(matrix);
	ImGui::SliderFloat4("", &m[0][0], -1.f, 1.f, "%+.3f");
	ImGui::SliderFloat4("", &m[1][0], -1.f, 1.f, "%+.3f");
	ImGui::SliderFloat4("", &m[2][0], -1.f, 1.f, "%+.3f");
	ImGui::SliderFloat4("", &m[3][0], -1.f, 1.f, "%+.3f");
}

void update_poses()
{
	vr::VRCompositor()->WaitGetPoses(
	  tracked_device_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	for (uint32_t nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount;
	     ++nDevice)
	{
		if (tracked_device_poses[nDevice].bPoseIsValid)
		{
			device_poses[nDevice] = to_z_up(
			  hmd_to_glm(tracked_device_poses[nDevice].mDeviceToAbsoluteTracking));
			if (device_classes[nDevice] == vr::TrackedDeviceClass_Invalid)
				device_classes[nDevice] =
				  vr::VRSystem()->GetTrackedDeviceClass(nDevice);
		}
	}
}

lak::optional<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *>)
{
	return lak::nullopt;
}

lak::optional<leapc::connection> lmconn;
lak::optional<leapc::device> lmdev;

lak::optional<int> LAK_BASIC_SINGLE_WINDOW_PROGRAM(program_init)()
{
	basic_window_target_framerate                = 30;
	basic_window_opengl_settings.major           = 3;
	basic_window_opengl_settings.minor           = 2;
	basic_window_opengl_settings.double_buffered = true;
	basic_window_clear_colour                    = {0.0f, 0.0f, 0.0f, 1.0f};

	basic_imgui_main_window_flags =
	  ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar |
	  ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoSavedSettings |
	  ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

	init_openvr();
	lmconn = leapc::connection::create().UNWRAP();
	lmconn->open().UNWRAP();

	return lak::nullopt;
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	lmdev.reset();
	lmconn.reset();
	vr::VR_Shutdown();

	return EXIT_SUCCESS;
}

void LAK_BASIC_PROGRAM(window_init)(lak::window &window)
{
	window.set_title(L"" APP_NAME);
}

void LAK_BASIC_PROGRAM(window_handle_event)(lak::window *window,
                                            lak::event &event)
{
	switch (event.type)
	{
		case lak::event_type::close_window:
			ASSERT(!!window);
			basic_destroy_window(*window);
			break;

		case lak::event_type::quit_program:
			// Need to rework this, causes a crash.
			// ASSERT(!!basic_single_window_window);
			// basic_destroy_window(*basic_single_window_window);
			break;

		default:
			break;
	}
}

void LAK_BASIC_PROGRAM(window_loop)(lak::window &window,
                                    uint64_t counter_delta)
{
	LAK_UNUSED(window);
	LAK_UNUSED(counter_delta);

	uint64_t distortion_matrix_version = 0U;
	lak::array<lak::array<lak::vec2f_t, LEAP_DISTORTION_MATRIX_N>,
	           LEAP_DISTORTION_MATRIX_N>
	  distortion_matrix;

	/* --- Handle LeapC events --- */
	{
		LEAP_CONNECTION_MESSAGE msg{};
		for (uint32_t timeout = 5;
		     lmconn->poll(timeout)
		       .if_ok([&](LEAP_CONNECTION_MESSAGE m) { msg = m; })
		       .replace(true)
		       .or_else(
		         [](eLeapRS rs) -> leapc::result<bool>
		         {
			         if (rs == eLeapRS_Timeout)
				         return lak::ok_t{false};
			         else
				         return lak::err_t{rs};
		         })
		       .UNWRAP();
		     timeout = 0)
		{
			switch (msg.type)
			{
				case eLeapEventType_Connection:
					DEBUG("eLeapEventType_Connection");
					lmconn
					  ->set_policy_flags(
					    eLeapPolicyFlag_BackgroundFrames | eLeapPolicyFlag_Images |
					      eLeapPolicyFlag_MapPoints,
					    eLeapPolicyFlag_OptimizeHMD | eLeapPolicyFlag_OptimizeScreenTop)
					  .UNWRAP();
					break;
				case eLeapEventType_ConnectionLost:
					DEBUG("eLeapEventType_ConnectionLost");
					break;
				case eLeapEventType_Device:
					DEBUG("eLeapEventType_Device");
					lmdev = leapc::device::create(msg.device_event->device).UNWRAP();
					break;
				case eLeapEventType_DeviceStatusChange:
					DEBUG("eLeapEventType_DeviceStatusChange");
					break;
				case eLeapEventType_Policy:
					DEBUG("eLeapEventType_Policy");
					break;
				case eLeapEventType_DeviceFailure:
					DEBUG("eLeapEventType_DeviceFailure");
					break;
				case eLeapEventType_Tracking:
					// DEBUG_EXPR(msg.tracking_event->nHands);
					break;
				case eLeapEventType_TrackingMode:
					DEBUG("eLeapEventType_TrackingMode");
					break;
				case eLeapEventType_LogEvent:
					DEBUG("eLeapEventType_LogEvent");
					break;
				case eLeapEventType_LogEvents:
					DEBUG("eLeapEventType_LogEvents");
					break;
				case eLeapEventType_ConfigResponse:
					DEBUG("eLeapEventType_ConfigResponse");
					break;
				case eLeapEventType_ConfigChange:
					DEBUG("eLeapEventType_ConfigChange");
					break;
				case eLeapEventType_DroppedFrame:
					DEBUG("eLeapEventType_DroppedFrame");
					break;
				case eLeapEventType_Image:
					DEBUG("eLeapEventType_Image");
					if (msg.image_event->image->matrix_version !=
					    distortion_matrix_version)
					{
						for (size_t i = 0U; i < distortion_matrix.size(); ++i)
							for (size_t j = 0U; j < distortion_matrix[i].size(); ++j)
								distortion_matrix[i][j] = {
								  msg.image_event->image->distortion_matrix->matrix[i][j].x,
								  msg.image_event->image->distortion_matrix->matrix[i][j].y};
						distortion_matrix_version = msg.image_event->image->matrix_version;
					}
					// :TODO: when i started this project i was working under the
					// misunderstanding that LeapC would provide a depth map. alas, it
					// only provides a stereo video feed. you would need to hook that up
					// to OpenCV or something to get a depth feed from it. oops.
					break;
				case eLeapEventType_PointMappingChange:
					DEBUG("eLeapEventType_PointMappingChange");
					break;
				case eLeapEventType_HeadPose:
					DEBUG("eLeapEventType_HeadPose");
					break;
				case eLeapEventType_Eyes:
					DEBUG("eLeapEventType_Eyes");
					break;
				case eLeapEventType_IMU:
					DEBUG("eLeapEventType_IMU");
					break;
				case eLeapEventType_NewDeviceTransform:
					DEBUG("eLeapEventType_NewDeviceTransform");
					break;
				case eLeapEventType_Fiducial:
					DEBUG("eLeapEventType_Fiducial");
					break;
			}
		}
	}

	/* --- Handle SteamVR events --- */

	for (vr::VREvent_t event;
	     vr::VRSystem()->PollNextEvent(&event, sizeof(event));)
	{
		switch (event.eventType)
		{
			case vr::VREvent_TrackedDeviceDeactivated:
			{
				DEBUG("Device ", event.trackedDeviceIndex, " detached.");
			}
			break;
			case vr::VREvent_TrackedDeviceActivated:
			{
				DEBUG("Device ", event.trackedDeviceIndex, " activated.");
			}
			break;
			case vr::VREvent_TrackedDeviceUpdated:
			{
				DEBUG("Device ", event.trackedDeviceIndex, " updated.");
			}
			break;
		}
	}

	update_poses();

	for (size_t i = 0; i < vr::k_unMaxTrackedDeviceCount; ++i)
	{
		switch (device_classes[i])
		{
			case vr::TrackedDeviceClass_Controller:
				ImGui::Separator();
				imgui_matrix("Controller", device_poses[i]);
				break;
			case vr::TrackedDeviceClass_HMD:
				ImGui::Separator();
				imgui_matrix("HMD", device_poses[i]);
				break;
			case vr::TrackedDeviceClass_GenericTracker:
				ImGui::Separator();
				imgui_matrix("Generic Tracker", device_poses[i]);
				break;
			case vr::TrackedDeviceClass_TrackingReference:
				ImGui::Separator();
				imgui_matrix("Tracking Reference", device_poses[i]);
				break;
		}
	}
}

void LAK_BASIC_PROGRAM(window_quit)(lak::window &)
{
	//
}
