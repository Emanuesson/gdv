gst_version_major = '1'
gst_version_minor = '0'

gst_req = '>= @0@.@1@.0'.format(gst_version_major, gst_version_minor)

gst_dep = dependency('gstreamer-1.0', version : gst_req,
fallback : ['gstreamer', 'gst_dep'])
gstvideo_dep = dependency('gstreamer-video-1.0', version : gst_req,
fallback : ['gst-plugins-base', 'video_dep'])

gdv_streamplugins_args = ['-DHAVE_CONFIG_H']

#plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')
plugins_install_dir = get_option('libdir')
plugins_pkgconfig_install_dir = join_paths(plugins_install_dir, 'pkgconfig')

configinc = include_directories('.')

libm = cc.find_library('m', required : false)

pkgconfig = import('pkgconfig')

plugins = []

subdir('sensors')

gdvstreaminput = library('gdvstreaminput', 'gdvstreaminput.c',
  c_args : gdv_streamplugins_args,
  include_directories : [],
  dependencies : [libgdv_dep, gst_dep, libm],
  install : true,
  install_dir : plugins_install_dir,
)
pkgconfig.generate(gdvstreaminput, install_dir : plugins_pkgconfig_install_dir)
plugins += [gdvstreaminput]

