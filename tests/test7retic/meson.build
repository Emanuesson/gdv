#if get_option('enable_tests')

executable('test7', 'test7.c',
             c_args: gdv_test_cflags,
          link_args: gdv_test_link_args,
       dependencies: libgdv_deps + [libgdv_dep],
          link_with: [libgdv, libtestgemu],
       link_depends: libgdv_map,
include_directories: [ root_inc, src_inc ],
)

