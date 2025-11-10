openvr_wrapper = static_library(
	'openvr_wrapper',
	[
		'openvr_wrapper.cpp',
	],
	override_options: lak_subprj.get_variable('override_options_w3'),
	dependencies: [
		lak_dep,
		openvr_dep,
	],
)

leapc_wrapper = static_library(
	'leapc_wrapper',
	[
		'leapc_wrapper.cpp',
	],
	override_options: lak_subprj.get_variable('override_options_w0'),
	dependencies: [
		lak_dep,
		leapc_dep,
	],
)

scanner_files = files(
	'main.cpp',
)
