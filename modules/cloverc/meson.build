cloverc_src = [
  'main.c'
]

cloverc_inc = include_directories('.')

executable('cloverc',
  sources: cloverc_src,
  include_directories: [cloverc_inc, libcloverc_inc],
  link_with: [libcloverc_lib],
  c_args: ['-DCL_PRGNAME="cloverc"'],
  install: true
)
