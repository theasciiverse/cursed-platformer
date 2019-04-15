project('linux-game-jam-2019-game', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

src = ['src/main.c', 'src/level.c', 'src/map.c', 'src/enemy.c', 'src/entity.c', 'src/helper.c']

sdl2 = dependency('sdl2')

if get_option('use_pdcurses')
  #todo : check if this actually worked
  run_command('cp', 'we_like_to_party/pdcurses/meson.build', 'subprojects/PDCurses')
  pdcurses = subproject('PDCurses')
  curses = pdcurses.get_variable('pdcurses')
  add_project_arguments('-DUSE_PDCURSES', language : 'c')
else
  curses = dependency('ncurses')
endif

exe = executable('linux_game_jam_2019_game', src, link_args: '-lm',
  install : true, dependencies : [sdl2, curses])

test('basic', exe)
