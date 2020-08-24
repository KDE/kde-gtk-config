project('gproxy', 'c', 'cpp')

sources = [
	'module.cpp',
	'gtkimcontextwrapper.cpp',
]

deps = [
	dependency('gtk+-3.0'),
	dependency('plasma-key-data'),
	dependency('Qt5', modules: ['Core']),
	dependency('KF5Config', method: 'cmake', modules: ['KF5::ConfigCore']),
	meson.get_compiler('c').find_library('dl', required : true),
]

conf_data = configuration_data()
conf_data.set('module_path', get_option('prefix') / get_option('libdir') / 'gtk-3.0' / '3.0.0' / 'immodules')
configure_file(
	input: 'conf.h.in',
	output: 'conf.h',
	configuration : conf_data,
)

lib = shared_module(
	'kwrapper',
	sources,
	dependencies: deps,
	name_prefix: 'im-',
	cpp_args: '-DQT_NO_SIGNALS_SLOTS_KEYWORDS',
	install: true,
	install_dir: get_option('libdir') / 'gtk-3.0' / '3.0.0' / 'immodules',
)
