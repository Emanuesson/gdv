introsp_test_env = environment()

if introspection_available
  message('Hello World------------------------!')
else
  message('Bye World------------------------!')
endif
#libgigsl_so_build_dir

#introsp_test_env.set('GI_TYPELIB_PATH', libgigsl_so_build_dir)
#introsp_test_env.set('LD_LIBRARY_PATH', libgigsl_so_build_dir)

introspec_tests = [
  ['test-basic'],
]

foreach t: introspec_tests
  test_name = t.get(0)
  test_srcs = ['@0@.py'.format(test_name), t.get(1, [])]
  test('tggsl-'+test_name, python,
    workdir: meson.current_source_dir(),
#    depends: [ libgdv_gir ],
    env: introsp_test_env,
    args: test_srcs,
  )
endforeach


