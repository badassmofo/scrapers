#!/usr/bin/env perl

$args .= "\"$_\" " foreach @ARGV;
$ed2k  = `ed2k $args | anidb --dry`;
@m     = $ed2k =~ /(~ Renaming: )?(.*) => (.*)/ig;
@res   = ();
for ($i = 1; $i < scalar(@m); $i += 3) {
  $dir = `dirname "$m[$i+1]"`;
  chop $dir;
  push @res, "mkdir -p \"$dir\"; mv \"@m[$i]\" \"@m[$i+1]\"";
}
$cmd   = join '; ', @res;
print "'$cmd'";
