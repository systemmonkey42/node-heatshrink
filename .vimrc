set aw
"
let g:ycm_show_diagnostics_ui = 0

let g:ale_pattern_options = {}

let s:clang_opts = []

" Select compilation language standard (-std=), eg, c++11, c99, and source file type (-x) etc, c or c++
let s:clang_opts += [ '-x', 'c' ]

let s:clang_opts += [ '-m64' ]

set cpo-=C
let s:clang_opts += [
            \ '-Wall',
            \ '-Werror',
            \ '-Weverything',
            \ '-Wno-missing-prototypes',
            \ '-fdiagnostics-show-option',
            \ '-Wno-reserved-id-macro',
            \ '-Wno-format-pedantic',
            \ '-Wno-disabled-macro-expansion',
            \ '-Wno-covered-switch-default',
            \ '-Wno-switch-enum',
            \ '-Wno-padded',
            \ '-Wmost',
            \ '-Wextra',
            \ '-Wenum-compare',
            \ '-Wconversion',
            \ '-Wc++98-compat',
            \ '-Wno-long-long',
            \ '-Wno-variadic-macros',
            \ '-Wno-unknown-pragmas',
            \ '-Wno-unused-macros',
            \ '-fexceptions',
            \ ]

let s:user_include_dirs = [
            \ '.',
            \ 'src',
            \ 'src/lib' ]

let s:system_include_dirs = [
            \ '/usr/include',
            \ '/usr/include/json-c',
            \ '/usr/include/x86_64-linux-gnu',
            \ '/usr/local/include',
            \ '/home/freestyle/.nvm/versions/node/v10.10.0/include/node' ]

for s:dir in s:user_include_dirs
    let s:clang_opts += [ '-I', fnamemodify(s:dir,':p') ]
endfor

for s:dir in s:system_include_dirs
    let s:clang_opts += [ '-isystem', fnamemodify(s:dir,':p') ]
endfor

unlet s:dir

let g:ale_pattern_options = {
            \ '\.h$' : {
            \      'ale_c_clang_options': join(s:clang_opts),
            \      },
            \ '\.c$' : {
            \      'ale_c_clang_options': join(s:clang_opts),
            \      }
            \ }

let g:ale_linters = {
            \ 'c': [ 'clang' ],
            \ 'cpp': [ 'clang' ]
            \ }

" Toggle ALE to re-read configuration options.
ALEDisable | ALEEnable

" Map movement through errors without wrapping.
augroup ALEBuffer
autocmd BufEnter *.c,*.h nmap <silent> <buffer> <C-k> <Plug>(ale_previous_wrap)
autocmd BufEnter *.c,*.h nmap <silent> <buffer> <C-j> <Plug>(ale_next_wrap)
augroup END

" C Syntax tweaks
" Ale uses the filetype, and vim defaults .h files to 'cpp' unless you
" explicity set the filetype in the modeline, or override the default here.
let g:c_syntax_for_h = 1
" Other syntax highlighting tweaks
let g:c_space_errors = 1
let g:c_ansi_typedefs = 1
let g:c_comment_strings = 1

unlet s:clang_opts

let g:ale_fix_on_save=1

nmap <Leader>? <Plug>(ale_detail)
