#if get_option('enable_tests')

test1_resources = gnome.compile_resources(
  'gdvinspec-resources',
  'gdvinspec.gresources.xml',
  'gui/gdvinspec-app-win.ui',

  c_name: 'gdv',
)

test1_sources = [
  'gdvinspector.c',
  'gdvinspector-app.c',
  'gdvinspector-app.h',
  'gdvinspector-app-win.c',
  'gdvinspector-app-win.h',
  test1_resources,
]

test1 = executable('test1', test1_sources,
             c_args: gdv_test_cflags,
          link_args: gdv_test_link_args,
       dependencies: libgdv_deps + [libgdv_dep],
          link_with: libgdv,
       link_depends: libgdv_map,
include_directories: [ root_inc, src_inc ],
)
#test('test1', test1, env: test_env)

