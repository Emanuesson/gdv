project('libgdv', 'c',
          version: '3.26.0',
          license: 'GPLv3+',
    meson_version: '>= 0.40.1',
  default_options: [ 'warning_level=1', 'buildtype=debugoptimized', 'c_std=gnu11' ],
)

version_arr = meson.project_version().split('.')
gdv_version_major = version_arr[0].to_int()
gdv_version_minor = version_arr[1].to_int()
gdv_version_micro = version_arr[2].to_int()

apiversion = '1.0'
soversion = 0

if gdv_version_minor.is_odd()
  gdv_interface_age = 0
else
  gdv_interface_age = gdv_version_micro
endif

# maintaining compatibility with the previous libtool versioning
# current = minor * 100 + micro - interface
# revision = interface
current = gdv_version_minor * 100 + gdv_version_micro - gdv_interface_age
revision = gdv_interface_age
libversion = '@0@.@1@.@2@'.format(soversion, current, revision)

config_h = configuration_data()
config_h.set_quoted('GETTEXT_PACKAGE', 'libgdv')
config_h.set_quoted('LOCALEDIR', join_paths(get_option('prefix'), get_option('localedir')))

configure_file(
  output: 'config.h',
  configuration: config_h,
)

add_project_arguments([
  '-DHAVE_CONFIG_H',
  '-DGDV_COMPILATION',
  '-I' + meson.build_root(),
], language: 'c')

root_inc = include_directories('.')
src_inc = include_directories('gdv')

cc = meson.get_compiler('c')

global_c_args = []
test_c_args = [
  '-Wcast-align',
  '-Wdeclaration-after-statement',
  ['-Werror=format-security', '-Werror=format=2'],
  '-Wformat-nonliteral',
  '-Wformat-security',
  '-Wmissing-include-dirs',
  '-Wnested-externs',
  '-Wno-missing-field-initializers',
  '-Wno-sign-compare',
  '-Wno-strict-aliasing',
  '-Wno-uninitialized',
  '-Wno-unused-parameter',
  '-Wpointer-arith',
  '-Wredundant-decls',
  '-Wshadow',
  '-Wswitch-default',
  '-Wswitch-enum',
  '-Wundef',
]
if get_option('buildtype') != 'plain'
  test_c_args += '-fstack-protector-strong'
endif
if get_option('enable_profiling')
  test_c_args += '-pg'
endif

foreach arg: test_c_args
  if cc.has_multi_arguments(arg)
    global_c_args += arg
  endif
endforeach
add_project_arguments(
  global_c_args,
  language: 'c'
)

# Check if we can use version scripts for ABI exports
ld_supports_version_script = cc.links('''
  int main (void) { return 0; }
''', args: '-Wl,--version-script,' + join_paths(meson.source_root(), 'gdv/gdv.map'))
message('Linker supports --version-script: @0@'.format(ld_supports_version_script))

# Setup various paths that subdirectory meson.build files need
package_subdir = get_option('package_subdir') # When used as subproject
libdir = join_paths(get_option('libdir'), package_subdir)
girdir = join_paths(get_option('datadir'), package_subdir, 'gir-1.0')
typelibdir = join_paths(get_option('libdir'), package_subdir, 'girepository-1.0')
if package_subdir != ''
  vapidir = join_paths(get_option('datadir'), package_subdir, 'vapi')
else
  vapidir = join_paths(get_option('datadir'), 'vala', 'vapi')
endif

gnome = import('gnome')

subdir('gdv')
subdir('data_viewer')
subdir('tests')

if get_option('enable_gtk_doc')
  subdir('doc')
endif