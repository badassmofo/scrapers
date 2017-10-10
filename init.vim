set shell=/bin/bash
set nocompatible
filetype off

call plug#begin()
Plug 'Shougo/deoplete.nvim', { 'do': ':UpdateRemotePlugins' }
Plug 'tweekmonster/deoplete-clang2'
Plug 'junegunn/rainbow_parentheses.vim'
Plug 'itchyny/lightline.vim'
Plug 'airblade/vim-gitgutter', {'branch': 'nvim'}
Plug 'easymotion/vim-easymotion'
Plug 'tpope/vim-fugitive'
Plug 'tpope/vim-endwise'
Plug 'tpope/vim-surround'
Plug 'haya14busa/incsearch.vim'
Plug 'haya14busa/incsearch-easymotion.vim'
Plug 'vim-scripts/a.vim'
Plug 'ctrlpvim/ctrlp.vim'
Plug 'terryma/vim-multiple-cursors'
Plug 'raimondi/delimitmate'
Plug 'simnalamburt/vim-mundo'
Plug 'godlygeek/tabular'
Plug 'scrooloose/nerdcommenter'
Plug 'scrooloose/nerdtree'
Plug 'lfilho/cosco.vim'
Plug 'takeiteasy/vim-monochrome'
call plug#end()

syntax enable
filetype on
filetype indent on
filetype plugin on
"autocmd! bufwritepost init.vim source %

set t_Co=256
set background=dark
color monochrome

let mapleader=","

set wrap
set magic
set textwidth=0
set wrapmargin=0
set number
set showcmd
set cursorline
set wildmenu
set wildmode=list:full
set wildignore=*.swp,*.bak,*.pyc,*.class
set lazyredraw
set showmatch
set incsearch
set hlsearch
nnoremap <leader><space> :nohlsearch<CR>
set foldenable
set foldlevelstart=99
set foldmethod=marker
set foldopen=block,hor,insert,jump,mark,percent,quickfix,search,tag,undo
set foldcolumn=2
set tabstop=2
set softtabstop=2
set shiftwidth=2
set shiftround
set smarttab
set backspace=indent,eol,start
set autoindent
set copyindent
set ignorecase
set smartcase
set virtualedit=
set nolist
set showbreak=↪\
set listchars=tab:→\ ,eol:↲,nbsp:␣,trail:•,extends:⟩,precedes:⟨
set pastetoggle=<F2>
set mouse=a
set fileformats="unix,dos,mac"
set formatoptions+=1
set nrformats=
set clipboard=unnamed
set autoread
set history=1000
set undolevels=1000
set undofile
set undodir=~/.config/nvim/.undo,~/tmp,/tmp
set nobackup
set noswapfile
set directory=~/.config/nvim/.tmp,~/tmp,/tmp
set viminfo='20,\"80

function! NumberToggle()
	if(&relativenumber == 1)
		set norelativenumber
		set number
	else
		set relativenumber
	endif
endfunc
nnoremap <F5> :call NumberToggle()<cr>
nnoremap ; :
nnoremap <leader>; ;
nnoremap j gj
nnoremap k gk
nnoremap B ^
nnoremap E $
nnoremap $ <nop>
nnoremap ^ <nop>
nnoremap gV `[v`]
inoremap jk <esc>
nnoremap <leader>q :q<CR>
nnoremap <leader>s :w<CR>
nnoremap <Space> za
vnoremap <Space> za
noremap <F3> :set list!<CR>

let g:deoplete#enable_at_startup=1
function g:Multiple_cursors_before()
	let g:deoplete#disable_auto_complete = 1
endfunction
function g:Multiple_cursors_after()
	let g:deoplete#disable_auto_complete = 0
endfunction

let g:deoplete#sources#clang#executable = '/usr/bin/clang'

let s:base03  = [ '#242424', 235 ]
let s:base023 = [ '#353535 ', 236 ]
let s:base02  = [ '#444444 ', 238 ]
let s:base01  = [ '#585858', 240 ]
let s:base00  = [ '#666666', 242  ]
let s:base0   = [ '#808080', 244 ]
let s:base1   = [ '#969696', 247 ]
let s:base2   = [ '#a8a8a8', 248 ]
let s:base3   = [ '#d0d0d0', 252 ]
let s:yellow  = [ '#cae682', 180 ]
let s:orange  = [ '#e5786d', 203 ]
let s:red     = [ '#e5786d', 203 ]
let s:magenta = [ '#e5786d', 203 ]
let s:blue    = [ '#e5786d', 203 ]
let s:cyan    = s:blue
let s:green   = [ '#e5786d', 203 ]
let s:p       = {'normal': {}, 'inactive': {}, 'insert': {}, 'replace': {}, 'visual': {}, 'tabline': {}}

let s:p.normal.left     = [ [ s:base3, s:blue ], [ s:base3, s:base01 ] ]
let s:p.normal.right    = [ [ s:base02, s:base0 ], [ s:base1, s:base01 ] ]
let s:p.inactive.right  = [ [ s:base023, s:base01 ], [ s:base00, s:base02 ] ]
let s:p.inactive.left   = [ [ s:base1, s:base02 ], [ s:base00, s:base023 ] ]
let s:p.insert.left     = [ [ s:base3, s:green ], [ s:base3, s:base01 ] ]
let s:p.replace.left    = [ [ s:base3, s:red ], [ s:base3, s:base01 ] ]
let s:p.visual.left     = [ [ s:base3, s:magenta ], [ s:base3, s:base01 ] ]
let s:p.normal.middle   = [ [ s:base2, s:base02 ] ]
let s:p.inactive.middle = [ [ s:base1, s:base023 ] ]
let s:p.tabline.left    = [ [ s:base3, s:base00 ] ]
let s:p.tabline.tabsel  = [ [ s:base3, s:base03 ] ]
let s:p.tabline.middle  = [ [ s:base2, s:base02 ] ]
let s:p.tabline.right   = [ [ s:base2, s:base00 ] ]
let s:p.normal.error    = [ [ s:base03, s:red ] ]
let s:p.normal.warning  = [ [ s:base023, s:yellow ] ]

let g:lightline#colorscheme#monochromell#palette = lightline#colorscheme#flatten(s:p)

let g:rainbow#max_level = 16
let g:rainbow#pairs = [['(', ')'], ['[', ']'], ['<', '>'], ['{', '}']]
let g:rainbow#colors = {
\	'dark': [
\		['255', ''],
\		['243', '']
\	],
\	'light': [
\		['255', ''],
\		['243', '']
\	]
\ }
autocmd VimEnter * RainbowParentheses

set laststatus=2
let g:lightline = {
\	'colorscheme': 'monochromell',
\ 	'mode_map': {
\		'n':			' N ',
\		'i':			' I ',
\		'R':			' R ',
\		'v':			' V ',
\		'V':			'V·L',
\		"\<C-v>": 'V·B',
\		'c':			' C ',
\		's':			' S ',
\		'S':			'S·L',
\		"\<C-s>": 'S·B',
\		't':			' T ',
\	},
\	'active': {
\		'left': [['mode', 'paste'], ['fugitive', 'filename']]
\	},
\	'component_function': {
\		'fugitive': 'LightLineFugitive',
\		'filename': 'LightLineFilename'
\	},
\	'separator': { 'left': '', 'right': '' },
\	'subseparator': { 'left': '|', 'right': '|' },
\	}

function! LightLineModified()
	return &ft =~ 'help\|vimfiler' ? '' : &modified ? '+' : &modifiable ? '' : '-'
endfunction

function! LightLineReadonly()
	return &ft !~? 'help\|vimfiler' && &readonly ? '⭤' : ''
endfunction

function! LightLineFilename()
	return ('' != LightLineReadonly() ? LightLineReadonly() . ' ' : '') .
\				 (&ft == 'vimfiler' ? vimfiler#get_status_string() :
\					&ft == 'unite' ? unite#get_status_string() :
\					&ft == 'vimshell' ? vimshell#get_status_string() :
\					'' != expand('%:t') ? expand('%:t') : '[No Name]') .
\				 ('' != LightLineModified() ? ' ' . LightLineModified() : '')
endfunction

function! LightLineFugitive()
  if &ft !~? 'vimfiler' && exists('*fugitive#head')
    let branch = fugitive#head()
    return branch !=# '' ? ' ⭠ '.branch : ''
  endif
	return ''
endfunction

map \ <Plug>(easymotion-prefix)

let g:incsearch#auto_nohlsearch = 1
map /  <Plug>(incsearch-forward)
map ?  <Plug>(incsearch-backward)
map g/ <Plug>(incsearch-stay)
map n  <Plug>(incsearch-nohl-n)
map N  <Plug>(incsearch-nohl-N)
map *  <Plug>(incsearch-nohl-*)
map #  <Plug>(incsearch-nohl-#)
map g* <Plug>(incsearch-nohl-g*)
map g# <Plug>(incsearch-nohl-g#)
map z/ <Plug>(incsearch-easymotion-/)
map z? <Plug>(incsearch-easymotion-?)
map zg/ <Plug>(incsearch-easymotion-stay)

set wildignore+=*/tmp/*,*.so,*.swp,*.zip
let g:ctrlp_custom_ignore = '\v[\/]\.(git|hg|svn)$'
let g:ctrlp_user_command = ['.git', 'cd %s && git ls-files -co --exclude-standard']

nnoremap <F4> :MundoToggle<CR>
let g:mundo_width=30
let g:mundo_preview_height=10
let g:mundo_right=0
let g:mundo_prefer_python3=1

map <F1> :NERDTreeToggle<CR>
let g:NERDSpaceDelims=1
let g:NERDCompactSexyComs=1
let g:NERDCommentEmptyLines=1
let g:NERDTrimTrailingWhitespace=1

" let g:auto_comma_or_semicolon = 1
autocmd FileType javascript,css,cpp,c nmap <silent> <Leader>; <Plug>(cosco-commaOrSemiColon)
autocmd FileType javascript,css,cpp,c imap <silent> <Leader>; <c-o><Plug>(cosco-commaOrSemiColon)
