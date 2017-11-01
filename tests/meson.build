
test_env = [
  'G_TEST_SRCDIR=@0@'.format(meson.current_source_dir()),
  'G_TEST_BUILDDIR=@0@'.format(meson.current_build_dir()),
  'G_DEBUG=gc-friendly',
  'GSETTINGS_BACKEND=memory',
  'PYTHONDONTWRITEBYTECODE=yes',
  'MALLOC_CHECK_=2',
]

test_cflags = [
  '-fPIE',
  '-DTEST_DATA_DIR="@0@/data"'.format(meson.current_source_dir()),
]

test_link_args = [
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

