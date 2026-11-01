project('linux-game-jam-2019-game', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

src = ['src/main.c', 'src/level.c', 'src/map.c', 'src/enemy.c', 'src/entity.c',
       'src/helper.c', 'src/sound.c', 'src/av_input.c']

sdl2 = dependency('sdl2')
sdl2_mixer = dependency('SDL2_mixer')

if get_option('use_pdcurses')
  #todo : check if this actually worked
  run_command('cp', 'we_like_to_party/pdcurses/meson.build', 'subprojects/PDCurses')
  pdcurses = subproject('PDCurses')
  pdcurses = pdcurses.get_variable('pdcurses')
#  add_project_arguments('-DUSE_PDCURSES', language : 'c')
endif

if get_option('use_ncurses')
  ncurses = dependency('ncurses')
endif

if get_option('use_ncurses')
exe = executable('platformer_game_ncurses', src, link_args: '-lm',
  install : true, dependencies : [sdl2, ncurses, sdl2_mixer])
test('basic', exe)
endif

if get_option('use_ncurses')
exe_hardcore = executable('platformer_game_ncurses_hardcore', src, link_args: '-lm',
  install : true, dependencies : [ncurses],
  c_args : '-DNOSDL',
)
test('basic', exe_hardcore)
endif



if get_option('use_pdcurses')
  exe_pdcurses = executable('platformer_game_pdcurses', src, link_args: '-lm',
  c_args : '-DUSE_PDCURSES',
  install : true, dependencies : [sdl2, pdcurses, sdl2_mixer])
  test('basic', exe_pdcurses)
endif

