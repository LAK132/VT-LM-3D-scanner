#include "leapc_wrapper.hpp"

#include <lak/array.hpp>

#include <assert.h>
#include <stdlib.h>

leapc::result<leapc::connection> leapc::connection::create(
  const _LEAP_CONNECTION_CONFIG *config)
{
	leapc::connection result;
	RES_TRY(result.handle.emplace(config));
	return lak::move_ok(result);
}

leapc::result<> leapc::connection::open() const
{
	return leapc::as_result(LeapOpenConnection(handle));
}

leapc::result<LEAP_CONNECTION_MESSAGE> leapc::connection::poll(
  uint32_t timeout) const
{
	LEAP_CONNECTION_MESSAGE evt{.size = sizeof(LEAP_CONNECTION_MESSAGE)};
	return leapc::as_result(LeapPollConnection(handle, timeout, &evt))
	  .replace(evt);
}

leapc::result<LEAP_CONNECTION_INFO> leapc::connection::get_info() const
{
	LEAP_CONNECTION_INFO info{.size = sizeof(LEAP_CONNECTION_INFO)};
	return leapc::as_result(LeapGetConnectionInfo(handle, &info)).replace(info);
}

leapc::result<> leapc::connection::set_policy_flags(uint64_t set,
                                                    uint64_t unset) const
{
	return leapc::as_result(LeapSetPolicyFlags(handle, set, unset));
}

leapc::result<lak::array<_LEAP_DEVICE_REF>>
leapc::connection::get_device_list() const
{
	uint32_t device_count = 0U;
	RES_TRY(leapc::as_result(LeapGetDeviceList(handle, NULL, &device_count)));
	if (device_count == 0) return lak::err_t{eLeapRS_CannotOpenDevice};

	lak::array<LEAP_DEVICE_REF> device_refs;
	device_refs.resize(device_count);
	RES_TRY(leapc::as_result(
	  LeapGetDeviceList(handle, device_refs.data(), &device_count)));

	return lak::move_ok(device_refs);
}

leapc::result<leapc::device> leapc::device::create(LEAP_DEVICE_REF ref)
{
	leapc::device result;
	RES_TRY(result.handle.emplace(ref));
	return lak::move_ok(result);
}

leapc::result<glm::mat4x4> leapc::device::get_transform()
{
	glm::mat4x4 mat;
	return leapc::as_result(LeapGetDeviceTransform(handle, &mat[0][0]))
	  .replace(mat);
}
