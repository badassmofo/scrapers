#!/usr/bin/env bash

alias please='sudo $(fc -nl -1)'
function try() {
  eval $1 $(fc -nl -1 | cut -d " " -f2-)
}
alias pls="sudo"
alias quit="exit"
alias :q="exit"
alias mkd="mkdir"
alias sym="ln -rs"
alias ls="ls --color=auto --group-directories-first"
alias la="ls -A"
alias grep="grep --color=auto"

alias nas="ssh reimu@192.168.1.76"
alias nas-web="open http://192.168.1.76:5000"
function nas-anime-ren-ssh() {
	ssh reimu@192.168.1.76 bash -c "`perl nas-anime-ren.pl '$@' | sed 's/\/Volumes\//\/volume1\//g'`"
}
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
alias git="hub"
alias ranger="vifm"
alias py="python"
alias py3="python3"
alias tree="tree --dirsfirst -l -x -C -q"
function xcode() {
	for x in "$@"; do
		if [[ ! -a $x ]]; then
			touch $x
		fi
	done
	open -a /Applications/Xcode.app $@
}

alias urlencode='python -c "import sys, urllib as ul; print ul.quote_plus(sys.argv[1]);"'
alias remoteip='wget -qO- "http://dynupdate.no-ip.com/ip.php"'
alias battery="pmset -g ps | grep -oP '(\d+)%'"
alias space_pc="df /dev/disk1 | grep -oP '(\d+)%'"
alias space_gb="printf '%.3fgb\n' $(echo $(df /dev/disk1 | grep -m2 -oP '(\d+)' | awk '{i++}i==5') ' / 1048576' | bc -l)"
alias space="printf '%s (%s)\n' $(space_gb) $(space_pc)"

alias show="defaults write com.apple.finder AppleShowAllFiles -bool true && killall Finder"
alias hide="defaults write com.apple.finder AppleShowAllFiles -bool false && killall Finder"

function mkcd() {
  mkdir -p "$@" && cd "$_";
}

function size() {
  if [[ -n "$@" ]]; then
    du -sbh -- "$@"
  else
    du -sbh .[^.]* ./*
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

function clone-all() {
  echo $(wget -qO- "https://api.github.com/users/takeiteasy/repos") | jq -r ".[] | .ssh_url" | while read -r line; do
    repo=$(basename $(echo $line) | rev | cut -d '.' -f2- | rev)
    if [[ ! -d $repo ]]; then
      git clone $line
    fi
  done
}

function bak() {
  for x in "$@"; do
    if [[ "$x" =~ "^.*\..*\.bak$" ]]; then
      if [[ -a "$x" ]]; then
        mv -v "$x" "${x:0:-4}"
      else
        echo "File doesn't exist: \"$x\""
      fi
    else
      mv -v "$x" "$x.bak"
    fi
  done
}

function currency() {
	if [[ -n $3 ]]; then
		args="&symbols=$(printf "%s," "${@:3}" | sed 's/,$//' | tr '[:lower:]' '[:upper:]')"
	fi
	wget -qO- "http://api.fixer.io/latest?base=$2$args" | jq -r ".rates | keys[] as \$k | \"\(\$k)=\(.[\$k] * $1)\""
}

function stock() {
	usage="needs to arguments to work...\n\tusage: ./stocks.bash lookup [company name]\n\t       ./stocks.bash quote  [company symbol]"

	if [[ -z $2 ]]; then
		echo -e $usage
		return -1
	fi

	case "$1" in
		"quote")
			json=$(wget -qO- "http://dev.markitondemand.com/Api/v2/Quote/json?symbol=$2")
			sym=$(echo $json | jq -r '.Symbol')
			price=$(echo $json | jq -r '.LastPrice')
			cpc=$(printf "%.2f" "$(echo $json | jq -r '.ChangePercent')")
			if [[ $(echo "$cpc<0" | bc) -eq 1 ]]; then
				cpcc="\e[101m↓"
			else
				cpcc="\e[102m↑"
			fi
			echo -e "$cpcc $cpc%\e[0m $sym@$price" ;;
		"lookup")
			wget -qO- "http://dev.markitondemand.com/Api/v2/Lookup/json?input=$2" | jq -r '.[] | "\(.Symbol): \(.Name)"' ;;
		*)
			echo -e $usage ;;
		esac
}

function audio() {
	input=${1-'(choose from list {"Internal Speakers", "Bose Mini II SoundLink", "PLEX-PC"} with title "Sound Picker" default items {"Internal Speakers"}) as text'}

	if [[ "$input" = "$1" ]]; then
		input="\"$input\""
	fi

	osascript -e "
		set asrc to $input
		tell application \"System Preferences\"
			reveal anchor \"output\" of pane id \"com.apple.preference.sound\"
			activate

			tell application \"System Events\"
				tell process \"System Preferences\"
					select (row 1 of table 1 of scroll area 1 of tab group 1 of window \"Sound\" whose value of text field 1 is asrc)
				end tell
			end tell

			quit
		end tell"
}

export PS1="❯ "

fortune | cowsay -W $(echo $(tput cols) " - 5" | bc -l) | lolcat -a --speed=500
eval "$(thefuck --alias fuck)"
qlmanage -r 1>/dev/null 2>/dev/null
echo
