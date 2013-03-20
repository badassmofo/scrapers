#!/bin/perl
use warnings;
use strict;

use WWW::Mechanize;
my $mech = WWW::Mechanize->new(autocheck => 1);
$mech->stack_depth(0);
use HTML::TreeBuilder 5 -weak;

my $base_url   = "http://www.fakku.net/";
my $def_format = "[%a] %t (%s) [%l]";

my $last_format = $def_format;
my $last_cat    = "doujinshi";
foreach (@ARGV) {
	if ($_ =~ /^doujinshi$/) {
		$last_cat = $_;
		print "Setting category to doujinshi!\n";
		next;
	}
	elsif ($_ =~ /^manga$/) {
		$last_cat = $_;
		print "Setting category to manga!\n";
		next;
	}
	elsif ($_ =~ /^--f=+?/) {
		$_ =~ s/--f=//g;
		if (length $_ == 0) {
			print "ERROR! No format passed! Using default!\n";
			$last_format = $def_format;
		}
		else {
			print "Setting format to: \"$_\"!\n";
			$last_format = $_;
		}
		next;
	}

	my $url = $base_url.$last_cat."/".$_;
	$mech->get($url);
	die "ERROR! Failed to fetch \"$url\"!\n" unless ($mech->success);

	my $tree = HTML::TreeBuilder->new;
	$tree->parse($mech->content);

	my ($title) = $tree->look_down('_tag', 'h1')->as_text();
	print $title."\n";
}

