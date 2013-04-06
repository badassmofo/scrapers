#!/usr/bin/perl
use warnings;
use strict;
use LWP::Simple;

for (my $i = 1; $i < 37; $i++) {
	for my $link (grep(/<a.*href=.*>/, split(/\n/, get("http://bunkai-kei.com/release/bk-k_".sprintf("%03d", $i))))) {
		($link) =~ s/^.*?<a.*href="([\s\S]+?)".*>.*/$1/;
		fork or exec("wget \"$link\"") if $link =~ /^http:\/\/download.bunkai/;
	}
}

