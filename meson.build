project(
  'clover',
  ['c'],
  version: '0.1',
  license: 'LGPL-3.0',
  meson_version: '>=1.7.0',
  default_options: {'c_std':'c17'}
)

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

cc = meson.get_compiler('c')
cc_name = cc.get_id()
cc_version = cc.version()

host_system = build_machine.system()

clc_version = meson.project_version()
clc_buildinfo = '@0@ @1@ (@2@)'.format(cc_name, cc_version, host_system)

cfg = configuration_data()
cfg.set('CL_BUFSIZE', 1024)
cfg.set_quoted('CL_VERSION', clc_version)
cfg.set_quoted('CL_BUILDINFO', clc_buildinfo)
configure_file(configuration: cfg, output: 'config.h')


# clc compiler
subdir('clc')
