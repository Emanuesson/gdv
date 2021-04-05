
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
  #'-fPIE',
  '-DTEST_DATA_DIR="@0@/data"'.format(meson.current_source_dir()),
#  '-Db_coverage=true',
  '-O0',
  '-g',
]

gdv_test_link_args = [
  '-fPIC',
]

#subdir('unittest_axis_test')

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

lib_test_gdv_srcs = [
  'tgdv-axis.c',
  'tgdv-axis.h',
  'tgdv-tic.c',
  'tgdv-tic.h',
  'tgdv-linearaxis.c',
  'tgdv-linearaxis.h',
  'tgdv-layer.c',
  'tgdv-layer.h',
  'tgdv-specialdrumdisplay.c',
  'tgdv-specialdrumdisplay.h',
  'tgdv-shared-functions.c',
  'tgdv-shared-functions.h',
]

libtestgdv = shared_library(
  'tgdv',
  lib_test_gdv_srcs,
  include_directories: [root_inc, src_inc],
  c_args: gdv_test_cflags,
  dependencies: [libgdv_dep],
  install: false,
)

libtestgdv_dep = declare_dependency(
  link_with: libtestgdv,
  include_directories: include_directories('.'),
)

gdv_test_deps = [
  libgdv_dep,
  libtestgdv_dep
]

test('tgdv-linearaxis',
  executable('tgdv-linearaxis-test', 'tgdv-linearaxis-test.c',
    include_directories: [root_inc, src_inc],
    c_args: gdv_test_cflags,
    dependencies: [
      gdv_test_deps,
    ],
  ),
  env: gdv_test_env,
)

test('tgdv-tic',
  executable('tgdv-tic-test', 'tgdv-tic-test.c',
    include_directories: [root_inc, src_inc],
    c_args: gdv_test_cflags,
    dependencies: [
      gdv_test_deps,
    ],
  ),
  env: gdv_test_env,
)

test('tgdv-specialdrumdisplay',
  executable('tgdv-specialdrumdisplay-test', 'tgdv-specialdrumdisplay-test.c',
    include_directories: [root_inc, src_inc],
    c_args: gdv_test_cflags,
    dependencies: [
      gdv_test_deps,
    ],
  ),
  env: gdv_test_env,
)

test('tgdv-twodlayer',
  executable('tgdv-twodlayer-test', 'tgdv-twodlayer-test.c',
    include_directories: [root_inc, src_inc],
    c_args: gdv_test_cflags,
    dependencies: [
      gdv_test_deps,
    ],
  ),
  env: gdv_test_env,
)

subdir('test-content')

