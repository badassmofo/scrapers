#!/bin/perl
use warnings;
use strict;

use WWW::Mechanize;
my $mech = WWW::Mechanize->new(autocheck => 1);
$mech->stack_depth(0);
use HTML::TreeBuilder 5 -weak;
use File::Path qw(mkpath rmtree);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);

my $base_url   = "http://www.fakku.net/";
my $def_format = "[%a] %t (%s) [%l]";

my $dl_path    = "/home/rusty/dl/";
my $save_dir   = "/tmp/";

my $last_format = $def_format;
my $last_cat    = "doujinshi";
foreach (@ARGV) {
	if ($_ =~ /^doujinshi$/) { # Download from fakku/doujinshi
		$last_cat = $_;
		print "Setting category to doujinshi!\n\n";
		next;
	}
	elsif ($_ =~ /^manga$/) { # Download from fakku/manga
		$last_cat = $_;
		print "Setting category to manga!\n\n";
		next;
	}
	elsif ($_ =~ /^--f=+?/) { # Get format for zip output
		$_ =~ s/--f=//g;
		if (length $_ == 0) {
			print "ERROR! No format passed! Using default!\n";
			$last_format = $def_format;
		}
		else {
			print "Setting format to: \"$_\"!\n\n";
			$last_format = $_;
		}
		next;
	}

	# Check if format is valid
	if ($last_format !~ /%/) {
		print "ERROR! Format contains no specifiers! Using default!\n";
		$last_format = $def_format;
	}

	print "Downloading Doujin \"$_\"...\nGathering info...";

	# Form URL and GET page
	my $url = $base_url.$last_cat."/".$_;
	$mech->get($url);
	die "ERROR! Failed to fetch \"$url\"!\nSTATUS: $mech->status\n" unless ($mech->success);

	# Parse page into tree
	my $tree = HTML::TreeBuilder->new;
	$tree->parse($mech->content);

	# Get title - First H1 tag
	my ($title) = $tree->look_down(_tag => 'h1')->as_text();
	print "SUCCESS!\nTitle:       \"$title\"\n";

	# Get rest of the information, they're all links so it's easy
	my @links   = $tree->look_down(_tag => 'a');
	my $series  = "unknown";
	my $lang    = "unknown";
	my $trans   = "unknown";
	my $artist  = "unknown";
	for my $link (@links){
		my $href  = $link->attr('href');
		next if !$href;

		# Check to see if the link is right
		$series = $link->as_text if ($href  =~ /^\/series\//);
		$artist = $link->as_text if ($href  =~ /^\/artists\//);
		$trans  = $link->as_text if ($href  =~ /^\/translators\//);

		# Check to see if the title is right
		my $title = $link->attr('title');
		next if (!$title);
		$lang = $link->as_text if ($title =~ / Hentai$/);
	}
	print "Series:      \"$series\"\n";
	print "Language:    \"$lang\"\n";
	print "Translator:  \"$trans\"\n";
	print "Artist:      \"$artist\"\n";
	print "Total pages:  ";

	# Form the zip output name
	my $out = $def_format;
	$out =~ s/%t/$title/g;
	$out =~ s/%a/$artist/g;
	$out =~ s/%s/$series/g;
	$out =~ s/%l/$lang/g;
	$out =~ s/%u/$trans/g;
	$out .= ".zip";

	# Get total number of pages
	# It's in a bold tag, which helps
	my @bolds = $tree->look_down(_tag => 'b');
	my $total_pages = 0;
	for my $bold (@bolds) {
		$bold = $bold->as_text;
		$total_pages = $bold if ($bold =~ /^\d+?$/);
	}
	print $total_pages."\nDownloading pages...\n";

	# Load the first page, theere we can get the rest of the pages
	$url .= "/read";
	$mech->get($url);
	die "ERROR! Failed to fetch \"$url\"!\nSTATUS: $mech->status\n" unless ($mech->success);

	# Page the HTML
	$tree->delete;
	$tree = HTML::TreeBuilder->new;
	$tree->parse($mech->content);

	# Get the Javascript that contains the links we need
	my @scripts = $tree->look_down(_tag => 'script');
	my $page_link = undef;
	for my $script (@scripts) {
		$script = $script->as_HTML;
		if ($script =~ /^<script type="text\/javascript">jQuery/) {
			$page_link = $script;
			last; # We got what we need!
		}
	}
	die "ERROR! Failed to parse Javascript!\n" if (!$page_link);

	# Find all links inside of the Javascript
	my (@link_matches) = ($page_link =~ m/(http:\/\/[^\s]+\.(png|gif|jpeg|jpg))/g);
	my $final_link = undef;
	for my $link (@link_matches) {
		# Get the valid link
		if ($link =~ /^http:\/\/cdn.fakku/) {
			$final_link = $link;
			last; # We got what we need!
		}
	}
	die "ERROR! Failed to parse Javascript!\n" if (!$final_link);

	# Get final images location and ext
	my ($ext) = $final_link =~ /(\.[^.]+)$/;
	$final_link =~ s/\'\+x\+\'$ext//g;

	my $tmp_dir = $save_dir.$_."/";
	mkpath($tmp_dir);

	my $zip = Archive::Zip->new;
	for (my $i = 1; $i <= $total_pages; $i++) {
		printf "Downloading #%03d...", $i;

		my $file_name = sprintf("%03d$ext", $i);
		my $file_url  = $final_link.$file_name;
		my $save_path = $tmp_dir.$file_name;

		# Check if file is already downloaded
		if (-e $save_path) {
			my $zip_member = $zip->addFile($save_path, $file_name);
			print "EXISTS!\n";
		}
		else {
			# Download the page
			die "ERROR! Failed to save \"$file_url\"!\nSTATUS: $mech->status\n" unless $mech->get($file_url, ":content_file" => $save_path);
			print "SUCCESS!\n";
			my $zip_member = $zip->addFile($save_path, $file_name);
		}
	}
	print "Compressing to Zip...";
	die "ERROR! Failed to write file to Zip!\n" unless ($zip->writeToFileNamed($dl_path.$out) == AZ_OK);
	print "SUCCESS!\nDoujin \"$_\" saved to \"$out\"!\n\n";

	# Clean up
	rmtree($tmp_dir, 0, 0);
	$tree->delete;
}
print "Program finished! Exiting!\n";

