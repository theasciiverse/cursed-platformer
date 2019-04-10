project('linux-game-jam-2019-game', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

curses = dependency('ncurses')

exe = executable('linux_game_jam_2019_game', 'linux_game_jam_2019_game.c',
  install : true, dependencies : [curses])

test('basic', exe)
