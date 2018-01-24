#!/usr/bin/env perl -w
use warnings;
use strict;
use LWP::Simple;

my @total = (get("http://bunkai-kei.com/release/") =~ /\[BK-K (\d+)\]/g);
for (my $i = 1; $i < $total[0]; $i++) {
	for my $link (grep(/<a.*href=.*>/, split(/\n/, get("http://bunkai-kei.com/release/bk-k_".sprintf("%03d", $i))))) {
		($link) =~ s/^.*?<a.*href="([\s\S]+?)".*>.*/$1/;
		fork or exec("wget \"$link\"") if $link =~ /^http:\/\/download.bunkai/;
	}
}