#if get_option('enable_tests')

subdir('gemu_sub')

test1_resources = gnome.compile_resources(
  'gdvinspec-resources',
  'gdvinspec.gresources.xml',
  'gui/gdvinspec-app-win.ui',

  c_name: 'gdv',
)

all_sub_test_sources = []

subdir('example2')
subdir('example3')
subdir('example4')
subdir('example5a')
subdir('example6')
subdir('example7')
subdir('example8')
subdir('example9')
subdir('example10')

test1_sources = [
  'gdvinspector.c',
  'gdvinspector-app.c',
  'gdvinspector-app.h',
  'gdvinspector-app-win.c',
  'gdvinspector-app-win.h',
  test1_resources,
  all_sub_test_sources,
]

test1 = executable('test1', test1_sources,
             c_args: gdv_test_cflags,
          link_args: gdv_test_link_args,
       dependencies: libgdv_deps + [libgdv_dep],
          link_with: [ libgdv, libtestgemu ],
       link_depends: libgdv_map,
include_directories: [ root_inc, src_inc ],
)
#test('test1', test1, env: test_env)

