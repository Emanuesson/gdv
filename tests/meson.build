
gdv_test_env = [
#  'GI_TYPELIB_PATH="@0@/src/libide:@0@/src/gstyle:$(GI_TYPELIB_PATH)"'.format(meson.build_root()),
#  'GB_IN_TREE_PLUGINS=1',
  'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
  'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  'G_DEBUG=gc-friendly',
  'GSETTINGS_BACKEND=memory',
#  'GSETTINGS_SCHEMA_DIR=@0@/data/gsettings:$(GSETTINGS_SCHEMA_DIR)'.format(meson.build_root()),
  'PYTHONDONTWRITEBYTECODE=yes',
  'MALLOC_CHECK_=2',
#  'MALLOC_PERTURB_=$((${RANDOM:-256} % 256))',
]

gdv_test_cflags = [
  '-fPIE',
  '-DTEST_DATA_DIR="@0@/data"'.format(meson.current_source_dir()),
]

gdv_test_link_args = [
  '-fPIC',
]

#subdir('unittest_axis_test')

subdir('gemu_sub')
subdir('test1basics')
subdir('test3axes')
subdir('test4legend')
subdir('test5overlay')
subdir('test6gengui')
subdir('test7retic')
subdir('test8timeaxis')
subdir('test9timebar')


gdv_test_deps = [
  libgdv_dep,
]

#gtk_tests = [
#  # testname, optional extra sources
#  ['rendernode'],
#]

#foreach t: gtk_tests
#  test_name = t.get(0)
#  test_srcs = ['@0@.c'.format(test_name), t.get(1, [])]
#  executable(test_name, test_srcs,
#             include_directories: [confinc, gdkinc],
#             c_args: test_args,
#             dependencies: [libgtk_dep, libm])
#endforeach


gdv_linearaxis = executable('test-gdv-linearaxis',
  'test-gdv-linearaxis.c',
  include_directories: [root_inc, src_inc],
  c_args: gdv_test_cflags,
  dependencies: [
    gdv_test_deps,
  ],
)

test('gdv-linearaxis', gdv_linearaxis,
  env: gdv_test_env,
)
