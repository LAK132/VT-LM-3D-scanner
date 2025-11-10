project(
	'VT-LM-3D-scanner',
	['c', 'cpp'],
	default_options: [
		'b_vscrt=static_from_buildtype',
	],
)

lak_subprj = subproject('lak')

c_cpp_args = lak_subprj.get_variable('c_cpp_args')
cpp_args = lak_subprj.get_variable('cpp_args')
c_args = lak_subprj.get_variable('c_args')
version = lak_subprj.get_variable('version')
lak_dep = lak_subprj.get_variable('lak_dep')

c_cpp_args += [
	'-DGLM_FORCE_SIZE_T_LENGTH',
]

cxx = meson.get_compiler('cpp')

if cxx.get_id() == 'msvc'
	c_cpp_args += [
		'-bigobj',
	]
endif

add_project_arguments(c_cpp_args + cpp_args, language: ['cpp'])
add_project_arguments(c_cpp_args + c_args, language: ['c'])

if host_machine.system() == 'windows'
  git_header = custom_target(
    'scanner_generate_git_file',
    output: 'scanner_git.hpp',
    command: ['generate_git_file.bat', '@OUTPUT@'] ,
    build_by_default: true,
  )
else
  git_header = custom_target(
    'scanner_generate_git_file',
    output: 'scanner_git.hpp',
    command: ['./generate_git_file.sh', '@OUTPUT@'] ,
    build_by_default: true,
  )
endif

subdir('include')
subdir('src')

fs = import('fs')
fs.copyfile(leapc_lib/leapc_dll, leapc_dll)

executable(
	'scanner',
	[
		scanner_files,
		git_header,
	],
	override_options: lak_subprj.get_variable('override_options_w3'),
	link_with: [
		openvr_wrapper,
		leapc_wrapper,
	],
	dependencies: [
		lak_dep,
		openvr_dep,
		leapc_dep,
	],
)
