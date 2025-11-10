cmake = import('cmake')

if get_option('leapc_inc_path') != ''
	leapc_inc = include_directories(get_option('leapc_inc_path'))
elif host_machine.system() == 'windows'
	leapc_inc = include_directories('C:'/'Program Files'/'Ultraleap'/'LeapSDK'/'include')
elif host_machine.system() == 'darwin'
	leapc_inc = include_directories('/Applications/Ultraleap Hand Tracking Service.app/Contents/LeapSDK/include')
elif host_machine.system() == 'linux'
	leapc_inc = include_directories('/usr/include')
else
	error('Unknown OS, please specify LeapC header include path')
endif

if get_option('leapc_lib_path') != ''
	leapc_lib = get_option('leapc_lib_path')
elif host_machine.system() == 'windows'
	leapc_lib = 'C:'/'Program Files'/'Ultraleap'/'LeapSDK'/'lib'/'x64'
elif host_machine.system() == 'darwin'
	leapc_lib = '/Applications/Ultraleap Hand Tracking Service.app/Contents/LeapSDK/lib'
elif host_machine.system() == 'linux'
	leapc_lib = '/usr/lib/ultraleap-hand-tracking-service'
else
	error('Unknown OS, please specify LeapC library path')
endif

if get_option('leapc_shared_lib') != ''
	leapc_dll = get_option('leapc_shared_lib')
elif host_machine.system() == 'windows'
	leapc_dll = 'LeapC.dll'
elif host_machine.system() == 'darwin'
	leapc_dll = 'LeapC.dylib'
elif host_machine.system() == 'linux'
	leapc_dll = 'LeapC.so'
else
	error('Unknown OS, please specify LeapC shared library name')
endif

cc = meson.get_compiler('c')
leapc_dep = declare_dependency(
	dependencies: cc.find_library('LeapC', dirs: leapc_lib),
	include_directories: leapc_inc,
)

openvr_opts = cmake.subproject_options()

openvr_opts.add_cmake_defines({
	'BUILD_SHARED': 'OFF',
})
openvr_opts.set_install(false)
if get_option('b_vscrt') == 'mt'
	openvr_opts.append_compile_args('cpp', '/MT')
elif get_option('b_vscrt') == 'mtd'
	openvr_opts.append_compile_args('cpp', '/MTd')
elif get_option('b_vscrt') == 'static_from_buildtype'
	if get_option('buildtype') == 'debug'
		openvr_opts.append_compile_args('cpp', '/MTd')
	else
		openvr_opts.append_compile_args('cpp', '/MT')
	endif
elif get_option('b_vscrt') == 'md'
	openvr_opts.append_compile_args('cpp', '/MD')
elif get_option('b_vscrt') == 'mdd'
	openvr_opts.append_compile_args('cpp', '/MDd')
elif get_option('b_vscrt') == 'from_buildtype'
	if get_option('buildtype') == 'debug'
		openvr_opts.append_compile_args('cpp', '/MDd')
	else
		openvr_opts.append_compile_args('cpp', '/MD')
	endif
elif get_option('b_vscrt') != 'none'
	error('unsupported vscrt link mode')
endif


openvr_subprj = cmake.subproject('openvr', options: openvr_opts)

if host_machine.system() == 'windows'
	openvr_dep = openvr_subprj.dependency('openvr_api64').as_link_whole()
else
	openvr_dep = openvr_subprj.dependency('openvr_api').as_link_whole()
endif
