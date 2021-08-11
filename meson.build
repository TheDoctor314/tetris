project(
  'tetris',
  'cpp',
  default_options : [
    'werror=true',
    'warning_level=3',
    'cpp_std=c++17',
    ],
  )

sfml_dep = dependency('sfml-graphics')

subdir('src')
executable('tetris', tetris_sources, dependencies : sfml_dep)
