project('linux-game-jam-2019-game', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

src = ['linux_game_jam_2019_game.c', 'level.c', 'enemy.c']

if get_option('use_pdcurses')
  #todo : check if this actually worked
  run_command('cp', 'we_like_to_party/pdcurses/meson.build', 'subprojects/PDCurses')
  pdcurses = subproject('PDCurses')
  curses = pdcurses.get_variable('pdcurses')
else
  curses = dependency('ncurses')
endif

exe = executable('linux_game_jam_2019_game', src,
  install : true, dependencies : [curses])

test('basic', exe)
