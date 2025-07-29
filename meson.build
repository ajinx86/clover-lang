project(
  'clover',
  ['cpp'],
  version: '0.1',
  license: 'LGPL-3.0',
  meson_version: '>=1.0.0',
  default_options: ['cpp_std=c++17'],
)

clover_sources = files([
  'src/main.cpp',
  'src/compiler.cpp',
  'src/lexer.cpp',
  'src/parser.cpp'
])

clover_includes = include_directories('include')

clover_deps = []

cxx = meson.get_compiler('cpp')
clover_version = meson.project_version()
clover_buildinfo = '@0@ @1@ (@2@)'.format(cxx.get_id(), cxx.version(), build_machine.system())

add_project_arguments(
  f'-DCLOVER_VERSION="@clover_version@"',
  f'-DCLOVER_BUILDINFO="@clover_buildinfo@"',
  language: 'cpp'
)

executable(
  'clover',
  sources: clover_sources,
  include_directories: clover_includes,
  dependencies: clover_deps,
)
