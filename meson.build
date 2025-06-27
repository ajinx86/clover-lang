project(
  'clover',
  ['cpp'],
  version: '0.1',
  license: 'GPL-3.0',
  meson_version: '>=1.0.0',
  default_options: ['cpp_std=c++17'],
)

subdir('src')
subdir('include')

clover_deps = []

cc = meson.get_compiler('cpp')
cfg = configuration_data()
cfg.set('version', meson.project_version())
cfg.set('buildinfo', '@0@ @1@'.format(cc.get_id(), cc.version()))
cfg.set('hostinfo', build_machine.system())
configure_file(configuration: cfg, input: 'version.h.in', output: 'version.h')

executable(
  'clover',
  sources: clover_sources,
  include_directories: clover_includes,
  dependencies: clover_deps,
)
