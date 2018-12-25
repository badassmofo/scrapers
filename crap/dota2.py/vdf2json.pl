#!/usr/bin/env perl -X
# modified + fixed from: http://jwjdev.com/blog/dota-2-items_game-txt-json-for-developers/

my $vdf = "{\n";
while (<>) {
  $vdf .= "\t$_";
}

$vdf =~ s/\s\/\/.*//g;
$vdf =~ s/^(?:[\t ]*(?:\r?\n|\r))+//gm;

$vdf =~ s/"([^"]*)"(\s*){/"${1}": {/g;
$vdf =~ s/"([^"]*)"\s*"([^"]*)"/"${1}": "${2}",/g;
$vdf =~ s/,(\s*[}\]])/${1}/g;
$vdf =~ s/([}\]])(\s*)("[^"]*":\s*)?([{\[])/${1},${2}${3}${4}/g;
$vdf =~ s/}(\s*"[^"]*":)/},${1}/g;

print "$vdf\n}";
