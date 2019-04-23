project('pdcurses', 'c', version : '3.6', default_options : ['warning_level=1'])

sdl2 = dependency('sdl2')
sdl2_ttf = dependency('SDL2_ttf')

add_project_arguments('-DPDC_WIDE', language : 'c')

lib = static_library('pdcurses', 
'pdcurses/addch.c',
'pdcurses/addchstr.c',
'pdcurses/addstr.c',
'pdcurses/attr.c',
'pdcurses/beep.c',
'pdcurses/bkgd.c',
'pdcurses/border.c',
'pdcurses/clear.c',
'pdcurses/color.c',
'pdcurses/debug.c',
'pdcurses/delch.c',
'pdcurses/deleteln.c',
#'pdcurses/deprec.c',
'pdcurses/getch.c',
'pdcurses/getstr.c',
'pdcurses/getyx.c',
'pdcurses/inch.c',
'pdcurses/inchstr.c',
'pdcurses/initscr.c',
'pdcurses/inopts.c',
'pdcurses/insch.c',
'pdcurses/insstr.c',
'pdcurses/instr.c',
'pdcurses/kernel.c',
'pdcurses/keyname.c',
'pdcurses/mouse.c',
'pdcurses/move.c',
'pdcurses/outopts.c',
'pdcurses/overlay.c',
'pdcurses/pad.c',
'pdcurses/panel.c',
'pdcurses/printw.c',
'pdcurses/refresh.c',
'pdcurses/scanw.c',
'pdcurses/scr_dump.c',
'pdcurses/scroll.c',
'pdcurses/slk.c',
'pdcurses/termattr.c',
#'pdcurses/terminfo.c',
'pdcurses/touch.c',
'pdcurses/util.c',
'pdcurses/window.c',
'sdl2/pdcclip.c',
'sdl2/pdcdisp.c',
'sdl2/pdcgetsc.c',
'sdl2/pdckbd.c',
'sdl2/pdcscrn.c',
'sdl2/pdcsetsc.c',
'sdl2/pdcutil.c',
'sdl2/sdltest.c',

dependencies : [sdl2, sdl2_ttf])

pdc_includes = include_directories('..')

pdcurses = declare_dependency(link_with : lib,
  include_directories : pdc_includes)
