#!/usr/bin/env bash

xcode-select --install

/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"

mkdir ~/.bin ~/.build ~/git ~/.sfx

cd ~/git

git clone --recursive git@github.com:takeiteasy/dotfiles.git
git clone git@github.com:takeiteasy/utils.git

cd dotfiles

convert \( -size `system_profiler SPDisplaysDataType | grep -Eohm1 "[0-9]{4} x [0-9]{4}" | tr -d [:space:]` xc:'rgb(35, 35, 35)' \) \( "$1" -fuzz 10% -transparent white -colorspace gray +level-colors white -trim \) -compose atop -gravity southwest -geometry +300 -composite "$HOME/Pictures/bg_`openssl rand -hex 6`.png"

unzip fonts.zip -d ~/Library/Fonts/
unzip sfx.zip -d ~/.sfx
