set -xg fish_greeting
set -xg PATH $HOME/.bin /usr/local/opt/coreutils/libexec/gnubin $PATH
set -xg BROWSER 'open -a Firefox'
set -xg EDITOR nvim
set -xg PERL5LIB $HOME/.perl5/lib/perl5 $PERL5LIB

alias reload "stfu source ~/.config/fish/config.fish"
alias pls sudo
alias quit exit
alias :q exit
alias cp "cp -v -i"
alias copy "rsync -hrvP"
alias mv "mv -v -i"
alias mkdir "mkdir -p -v"
alias mkd mkdir
alias mkcd mkcdir
alias sym "ln -rs"
alias ls "ls --color=auto --group-directories-first"
alias le "ls -lahF"
alias la "ls -ahF"
alias l. "ls -dhF .*"
alias grep "grep --color=auto"
alias egrep "egrep --color=auto"
alias fgrep "fgrep --color=auto"

alias finder-restart "kill -1 (ps aucx | grep Finder | awk '{print $2}')"
alias lastcmd "history | head -1"
alias zeronet "cd ~/.build/ZeroNet-master; python zeronet.py"
alias rangerw "echo ranger | win"
alias tmux "tmux -2"
alias xmpv "xargs mpv"
alias cclone "clone (pbpaste)"
alias cmpv "mpv (pbpaste)"
alias cmpvnv "mpv (pbpaste) --no-video"
alias cwget "wget (pbpaste)"
alias eecho "highlight -l -O ansi"
alias sprunge "curl -F 'sprunge=<-' http://sprunge.us"
alias timef "date +\"%r\" | toilet -f mono9 -F gay"
alias audio-dac "set_audio_out 'Speaker-Schiit USB Audio Device'"
alias audio-spk "set_audio_out 'Headphones'"
alias fishc "nvim ~/.config/fish/config.fish"
alias vimc "nvim ~/.config/nvim/init.vim"
alias dailydose 'mpv "https://www.youtube.com/watch?v=nU-cJ42BqC0" --fs --loop'
alias xq "xmllint --xpath"
alias tree "tree --dirsfirst -l -x -C -q"
alias vim nvim
alias newest "ls -ltr | tail -n 1 | cut -d ' ' -f11"
alias open-new "open (newest)"
alias ytdl youtube-dl
alias yt youtube-mpv
alias ytnv "yt nv"
alias battery "pmset -g ps | grep -oP '(\d+)%'"
alias bat battery
alias spacep "df /dev/disk1 | grep -oP '(\d+)%'"
alias space "printf \"%.3fgb\n\" (echo (df /dev/disk1 | grep -m2 -oP '(\d+)' | awk '{i++}i==5')\" / 1048576\" | bc -l)"
alias spaceleft "printf \"%s (%s)\n\" (space) (spacep)"
alias remoteip 'wget -qO- "http://dynupdate.no-ip.com/ip.php"'
alias py python
alias py3 python3

alias gensokyo-ssh "ssh root@192.168.1.76"
alias gensokyo "open http://192.168.1.76:5000"
alias transmission-web "open http://192.168.1.180:9091"
alias sonarr "open http://192.168.1.180:8989"
alias nzbget "open http://192.168.1.180:6789"
alias plex "open http://192.168.1.76:32400/web/index.html"
alias couchpotato "open http://192.168.1.180:5050"
alias btsync "open http://192.168.1.76:8890"
alias headphones "open http://192.168.1.180:8181"

alias tile "osascript ~/.bin/termtile/tile.scpt "
alias big "osascript ~/.bin/termtile/resize.scpt  "
alias cen "osascript ~/.bin/termtile/center.scpt  "
alias max "osascript ~/.bin/termtile/maximize.scpt  "
alias sn "osascript ~/.bin/termtile/changeScreen.scpt next "
alias fs "osascript ~/.bin/termtile/fullscreen.scpt  "

function please --description "Run last command as root"
  eval sudo $history[1]
end

function rr --description "Run last commands args with different program"
  eval $argv (echo $history[1] | cut -d ' ' -f2-)
end

function stfu --description "Mute programs"
  eval $argv 1>/dev/null 2>/dev/null
end

function bind_bang
  switch (commandline -t)
  case "!"
    commandline -t $history[1]; commandline -f repaint
  case "*"
    commandline -i !
  end
end

function bind_dollar
  switch (commandline -t)
  case "!"
    commandline -t ""
    commandline -f history-token-search-backward
  case "*"
    commandline -i '$'
  end
end

function fish_user_key_bindings
  bind ! bind_bang
  bind '$' bind_dollar
end

function mpvd
  mpv --input-unix-socket=/tmp/mpvsocket --really-quiet $argv &
end

function scrotum --description "Add a fancy drop shadow to screenshot"
  screencapture -wS /tmp/screenshot.png
  convert /tmp/screenshot.png \( +clone -background black -shadow 20x20x20x20+10+10+10+10 \) +swap -background none -layers merge +repage "$HOME/Dropbox/Screenshots/Screenshot "(date +"%Y-%m-%d %H.%M.%S")".png"
  rm /tmp/screenshot.png
end

function archive-dropbox --description "Archive shared folder"
  set f_dir {$HOME}/Dropbox/Shared
  set t_dir {$HOME}/Dropbox\ Archive/(date +"%Y/%m-%d")
  mkdir $t_dir
  mv $f_dir/* $t_dir/
end

function mkcdir --description "Make directory, then enter it"
  mkd $argv; cd $argv[-1]
end

function git_branch_name
  echo (git symbolic-ref HEAD ^/dev/null | sed -e 's|^refs/heads/||')
end

function git_is_dirty
  echo (git status -s --ignore-submodules=dirty ^/dev/null)
end

function g
  if test -n (echo $argv)
    eval "git $argv"
    return 0
  end
  while true
    while read -l line
      if test (string sub -s 1 -l 1 $line) = ' '
        eval (string sub -s 2 $line)
      else if test $line = "exit"
        return 0
      else
        eval "git $line"
      end
    end
  end
end

function fish_title --description "get rekt in reverse"
  if [ (git_branch_name) ]
    if [ (git_is_dirty) ]
      echo '✍🏿'
    else
      echo '👌🏿'
    end
    printf "%s@" (git_branch_name)
  end
  printf "%s" (pwd | sed -e "s/\/Users\/$USER/~/")
end

function fish_prompt
	if [ $TMUX ]
		printf '%s⚡︎ ' (set_color -o yellow)
	else
		printf ' '
	end
end

function clone
  git clone --recursive $argv
  cd (echo $argv | awk -F/ '{print $NF}' | sed -e 's/.git$//')
end

function mkbin --description "Copy a file to bin and chmod"
  for i in $argv
      set x (echo $i | cut -d '.' -f1)
      if [ $x ]
          set y {$HOME}/.bin/(basename $x)
          cp $i $y
          chmod +x $y
      else
          printf "\"%s\" doesn't exist!\n" $i
      end
  end
end

function jsugly --description "Uglify .js file"
  for i in $argv
    uglifyjs $i -c -m -o (echo $i | cut -d '.' -f1).min.js
  end
end

function mv. --description "Move files to pwd"
  mv $argv .
end

function wrap
  echo $argv | sed 's/^/"/g' | sed 's/$/"/g'
end

function wrapp
  while read -l line
    wrap $line
  end
end

function to_args
  echo $argv | wrapp | tr '\n' ' '
end

function to_argsp
  while read -l line
    echo $line | wrapp | tr '\n' ' '
  end
end

function win
  while read -l line
    window $line
  end
end

function truncate
  if test -z $argv[1]
    set to (tput cols)
  else
    set to $argv[1]
  end
  while read -l line
    echo (expr substr $line 1 $to)
  end
end

function 0x0
  if i in $argv
    curl --progress-bar -F "file=@$i" "https://0x0.st'"
  end
end

function edit
  for arg in $argv
    if test ! -e $arg
      set ext {$HOME}/.templates/(string match -r '\..*$' $arg)
      if test -e $ext
        cp $ext $arg
      else
        touch $arg
      end
    end
  end
  eval $EDITOR $argv
end

fortune | cowsay -W 76
rvm default
source {$HOME}/.iterm2_shell_integration.fish

eval (thefuck --alias | tr '\n' ';')
eval (dircolors -b ~/.LS_COLORS | grep -v export | sed 's/LS_COLORS=/set -x LS_COLORS /')
qlmanage -r 1>/dev/null 2>/dev/null
clear
