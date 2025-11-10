#ifndef LEAPC_WRAPPER_HPP
#define LEAPC_WRAPPER_HPP

#include <LeapC.h>

#include <lak/array.hpp>
#include <lak/com_ptr.hpp>
#include <lak/result.hpp>

#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace leapc
{
	template<typename T = lak::monostate>
	using result = lak::result<T, eLeapRS>;

	inline leapc::result<> as_result(eLeapRS rs)
	{
		if (rs == eLeapRS_Success)
			return lak::ok_t{};
		else
			return lak::err_t{rs};
	}
}

template<>
struct lak::unique_com_ptr_traits<LEAP_CONNECTION>
{
	using handle_type  = LEAP_CONNECTION;
	using exposed_type = LEAP_CONNECTION;

	inline static leapc::result<handle_type> ctor(
	  const LEAP_CONNECTION_CONFIG *config = nullptr)
	{
		LEAP_CONNECTION connection;
		return leapc::as_result(LeapCreateConnection(config, &connection))
		  .replace(connection);
	}

	inline static void dtor(handle_type handle)
	{
		LeapDestroyConnection(handle);
	}
};

template<>
struct lak::unique_com_ptr_traits<_LEAP_DEVICE *>
{
	using handle_type  = _LEAP_DEVICE *;
	using exposed_type = _LEAP_DEVICE *;

	inline static leapc::result<handle_type> ctor(LEAP_DEVICE_REF ref)
	{
		LEAP_DEVICE device;
		return leapc::as_result(LeapOpenDevice(ref, &device)).replace(device);
	}

	inline static void dtor(handle_type handle) { LeapCloseDevice(handle); }
};

namespace leapc
{
	struct connection
	{
		using handle_type = lak::unique_com_ptr<LEAP_CONNECTION>;
		handle_type handle;

		static leapc::result<leapc::connection> create(
		  const LEAP_CONNECTION_CONFIG *config = nullptr);

		leapc::result<> open() const;
		leapc::result<LEAP_CONNECTION_MESSAGE> poll(uint32_t timeout) const;
		leapc::result<LEAP_CONNECTION_INFO> get_info() const;
		leapc::result<> set_policy_flags(uint64_t set, uint64_t unset) const;
		leapc::result<lak::array<LEAP_DEVICE_REF>> get_device_list() const;
	};

	struct device
	{
		using handle_type = lak::unique_com_ptr<LEAP_DEVICE>;
		handle_type handle;

		static leapc::result<leapc::device> create(LEAP_DEVICE_REF ref);

		leapc::result<glm::mat4x4> get_transform();
	};

}

#endif
