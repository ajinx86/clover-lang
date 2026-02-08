project(
  'clover',
  ['c'],
  version: '0.1',
  license: 'LGPL-3.0',
  meson_version: '>=1.7.0',
  default_options: {'c_std': 'c17'},
)

add_project_arguments('-D_POSIX_C_SOURCE=200809L', language: 'c')

cc = meson.get_compiler('c')
cc_name = cc.get_id()
cc_version = cc.version()

host_system = build_machine.system()

clc_version = meson.project_version()
clc_buildinfo = '@0@ @1@ (@2@)'.format(cc_name, cc_version, host_system)

add_project_arguments(
  f'-DCL_VERSION="@clc_version@"',
  f'-DCL_BUILDINFO="@clc_buildinfo@"',
  language: 'c'
)

if get_option('buildtype') == 'debug'
  add_project_arguments('-DDEBUG=1', language: 'c')
endif

# required system libraries
m_lib = cc.find_library('m', required: true)

# clover compiler lib
subdir('modules/libcloverc')

# clover compiler
subdir('modules/cloverc')
