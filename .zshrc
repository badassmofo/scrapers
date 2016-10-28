#!/usr/bin/env zsh

alias ..="cd .."
alias ...="cd ../.."
alias ....="cd ../../.."
alias .....="cd ../../../.."
alias ~="cd ~"

alias pls="sudo"
alias quit="exit"
alias :q="exit"
alias copy="rsync -hrvP"
alias move="rsync -hrvP --remove-source-files"
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

alias gensokyo-ssh="ssh reimu@192.168.1.76"
alias gensokyo="open http://192.168.1.76:5000"
alias transmission-web="open http://192.168.1.180:9091"
alias sonarr="open http://192.168.1.180:8989"
alias nzbget="open http://192.168.1.180:6789"
alias plex="open http://192.168.1.76:32400/web/index.html"
alias couchpotato="open http://192.168.1.180:5050"
alias btsync="open http://192.168.1.76:8890"
alias headphones="open http://192.168.1.180:8181"

alias vimc="nvim ~/.config/nvim/init.vim"
alias zshc="nvim ~/.zshrc"
alias path="echo -n ${PATH//:/\\n}"

alias zeronet="cd ~/build/ZeroNet; python zeronet.py"
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

function cdf() {
  cd "$(osascript -e 'tell app "Finder" to POSIX path of (insertion location as alias)')";
}

function fs() {
  if du -b /dev/null > /dev/null 2>&1; then
    local arg=-sbh;
  else
    local arg=-sh;
  fi
  if [[ -n "$@" ]]; then
    du $arg -- "$@";
  else
    du $arg .[^.]* ./*;
  fi;
}

function a() {
  if [ $# -eq 0 ]; then
    atom .;
  else
    atom "$@";
  fi;
}

function scrotum() {
  screencapture -wS /tmp/screenshot.png
  convert /tmp/screenshot.png \( +clone -background black -shadow 20x20x20x20+10+10+10+10 \) +swap -background none -layers merge +repage "$(echo "$HOME/Desktop/Screenshot" $(date +"%Y-%m-%d %H.%M.%S") ".png")"
  rm /tmp/screenshot.png
}

PROMPT="$ "
SPROMPT="zsh: correct '%R' to '%r'? [N/y/a/e] "

fortune | cowsay -W $(echo $(tput cols) " - 3" | bc -l) | lolcat -a --speed=100
eval "$(thefuck --alias fuck)"
qlmanage -r 1>/dev/null 2>/dev/null
echo
