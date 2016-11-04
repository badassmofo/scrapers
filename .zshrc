#!/usr/bin/env zsh

alias please='sudo $(fc -nl -1)'
function try() {
  eval $1 $(fc -nl -1 | cut -d " " -f2-)
}
alias pls="sudo"
alias quit="exit"
alias :q="exit"
alias mk="mkdir"
alias sym="ln -rs"
alias ls="ls --color=auto --group-directories-first"
alias la="ls -A"
alias grep="grep --color=auto"
alias g="git"

alias dl="cd ~/Downloads"
alias pics="cd ~/Pictures"
alias db="cd ~/Dropbox/Shared"
alias dev="cd ~/Dropbox/dev"

alias nas="ssh reimu@192.168.1.76"
alias nas-web="open http://192.168.1.76:5000"
alias transmission-web="open http://192.168.1.76:9091"
alias sonarr="open http://192.168.1.76:8989"
alias nzbget="open http://192.168.1.76:6789"
alias plex="open http://192.168.1.76:32400/web/index.html"
alias couchpotato="open http://192.168.1.76:5050"
alias btsync="open http://192.168.1.76:8890"
alias headphones="open http://192.168.1.76:8181"

alias vimc="nvim ~/.config/nvim/init.vim"
alias zshc="nvim ~/.zshrc"
alias path='echo -e "${PATH//:/\\n}"'

alias zeronet="cd ~/.build/ZeroNet; python zeronet.py"
alias jsc="/System/Library/Frameworks/JavaScriptCore.framework/Versions/A/Resources/jsc"
alias vim="nvim"
alias ranger="vifm"
alias py="python"
alias py3="python3"
alias tree="tree --dirsfirst -l -x -C -q"

alias urlencode='python -c "import sys, urllib as ul; print ul.quote_plus(sys.argv[1]);"'
alias remoteip='wget -qO- "http://dynupdate.no-ip.com/ip.php"'
alias battery="pmset -g ps | grep -oP '(\d+)%'"
alias space_pc="df /dev/disk1 | grep -oP '(\d+)%'"
alias space_gb="printf '%.3fgb\n' $(echo $(df /dev/disk1 | grep -m2 -oP '(\d+)' | awk '{i++}i==5') ' / 1048576' | bc -l)"
alias space="printf '%s (%s)\n' $(space_gb) $(space_pc)"

alias show="defaults write com.apple.finder AppleShowAllFiles -bool true && killall Finder"
alias hide="defaults write com.apple.finder AppleShowAllFiles -bool false && killall Finder"

function mkd() {
  mkdir -p "$@" && cd "$_";
}

function size() {
  if [[ -n "$@" ]]; then
    du -sbh -- "$@"
  else
    du -sbh .[^.]* ./*
  fi
}

function edit() {
  if [ $# -eq 0 ]; then
    atom .
  else
    atom "$@"
  fi
}

function alert() {
  full="$@"
  cmd=$(echo $full | cut -d " " -f1)
  args=$(echo $full | cut -d " " -f2-)
  if eval $full; then
    terminal-notifier -title "'$cmd' has finished!" -message "With args: '$args'" -sound "ja jan"
  else
    terminal-notifier -title "'$cmd' has failed!" -message "With args: '$args'" -sound "Sosumi"
  fi
}

function scrotum() {
  screencapture -wS /tmp/screenshot.png
  convert /tmp/screenshot.png \( +clone -background black -shadow 20x20x20x20+10+10+10+10 \) +swap -background none -layers merge +repage "$(echo "$HOME/Desktop/Screenshot" $(date +"%Y-%m-%d %H.%M.%S") ".png")"
  rm /tmp/screenshot.png
}

function github() {
  if git rev-parse --git-dir > /dev/null 2>&1; then
    cmd=$(git config --get remote.origin.url)
    if [[ "$cmd" =~ "git@github.com:.*\/.*\.git" ]]; then
      open $(echo $cmd | sed 's/:/\//g' | sed 's/git@/http:\/\//g')
    else
      open $cmd
    fi
  else
    open "http://www.github.com/takeiteasy/"
  fi
}

PROMPT="$ "
SPROMPT="zsh: correct '%R' to '%r'? [N/y/a/e] "

fortune | cowsay -W $(echo $(tput cols) " - 5" | bc -l) | lolcat -a --speed=100
eval "$(thefuck --alias fuck)"
qlmanage -r 1>/dev/null 2>/dev/null
echo
