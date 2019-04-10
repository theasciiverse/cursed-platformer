project('linux-game-jam-2019-game', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

if get_option('use_pdcurses')
  curses_compile_flags = run_command('xcurses-config', '--cflags').stdout().strip().split()
  curses_link_flags = run_command('xcurses-config', '--libs').stdout().strip().split()
  curses = declare_dependency(link_args : curses_link_flags, compile_args : curses_compile_flags)
else
  curses = dependency('ncurses')
endif

exe = executable('linux_game_jam_2019_game', 'linux_game_jam_2019_game.c',
  install : true, dependencies : [curses])

test('basic', exe)
