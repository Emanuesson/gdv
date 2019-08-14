'''
gdvaccel3d = library('gdvaccel3d', 'gdvaccel3d.c',
  c_args : gdv_streamplugins_args,
  include_directories : [],
  dependencies : [gstvideo_dep, gst_dep, libm],
  install : true,
  install_dir : plugins_install_dir,
)
pkgconfig.generate(gdvaccel3d, install_dir : plugins_pkgconfig_install_dir)
plugins += [gdvaccel3d]
'''
