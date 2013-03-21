#!/usr/bin/perl
use warnings;
use strict;

use WWW::Mechanize;
my $mech = WWW::Mechanize->new(autocheck => 1);
$mech->stack_depth(0);
use HTML::TreeBuilder 5 -weak;
use File::Path qw(mkpath rmtree);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);

my $base_cats = "http://thedoujin.com/index.php/categories/";
my $base_page = "http://thedoujin.com/index.php/pages/";

my $dl_path   = "/home/rusty/dl/";
my $save_dir  = "/tmp/";

foreach (@ARGV) {
	# Check if argument is a valid number. Can't begin with 0
	if ($_ !~ /^\d+?$/ || $_ =~ /^[0]{1}/) {
		print "ERROR! \"$_\" is not a valid ID! Skipping\n";
		next;
	}
	print "Downloading Doujin \"$_\"...\nGathering info...";

	# Create link and fetch page
	my $url = $base_cats.$_;
	$mech->get($url);
	die "ERROR! Failed to fetch \"$url\"!\nSTATUS: $mech->status\n" unless ($mech->success);

	# Parse HTML to tree
	my $tree = HTML::TreeBuilder->new;
	$tree->parse($mech->content);

	# Get Title for archive name, if it's balnk, try description field
	# If that's blank or contains a newline, use timestamp as title
	my ($title) = $tree->look_down(id => 'Categories_title')->attr("value");
	($title)    = $tree->look_down(id => 'Categories_description')->as_text() if ($title eq "");
	($title)    = time if ($title eq "" || $title =~ /\n.*\z/);
	print "SUCCESS!\nTitle: \"$title\"\nTotal pages: ";
	$title      = $dl_path.$title.".zip"; # Add extension
	die "ERROR! This Doujin is already downloaded!\n" if (-e $title);

	# Get all links of the page, to get the total pages and images
	my @links = $tree->find(_tag => 'a');
	my $total_pages = 0;
	my $total_imgs  = 0;
	for my $link (@links) {
		$link = $link->attr("href");
		if ($link =~ /^\/index.php\/categories\/$_\?/) { # Pages
			$link =~ s/^\/index.php\/categories\/$_\?Pages_page=//g;
			$total_pages = $link if ($link > $total_pages);
		}
		elsif ($link =~ /^\/index.php\/pages\/$_\?/) { # Images
			$link =~ s/^\/index.php\/pages\/$_\?Pages_page=//g;
			$total_imgs = $link if ($link > $total_imgs);
		}
	}

	# Check if there is more than 1 page of (doujin) pages
	# Get the real total number of pages
	if ($total_pages != 0) {
		$url = $base_cats."/".$_."?Pages_page=".$total_pages;
		$mech->get($url);
		die "ERROR! Failed to fetch \"$url\"!\nSTATUS: $mech->status\n" unless ($mech->success);

		$tree->delete;
		$tree = HTML::TreeBuilder->new;
		$tree->parse($mech->content);

		@links = $tree->find(_tag => 'a');
		for my $link (@links) {
			$link = $link->attr("href");
			if ($link =~ /^\/index.php\/pages\/$_\?/) { # Images
				$link =~ s/^\/index.php\/pages\/$_\?Pages_page=//g;
				$total_imgs = $link if ($link > $total_imgs);
			}
		}
	}
	print $total_imgs."\nDownloading pages...\n";

	# Get format for file names
	my $zero_pad     = int((log($total_imgs) / log(10)) + 1);
	my $print_format = "%0${zero_pad}d";

	# Make directory for saved images
	my $tmp_dir = $save_dir.$_."/";
	mkpath($tmp_dir);

	# Finally, load each page and save the image
	# I wish I knew a faster and easy way to do it, but there isn't
	# After the file is downloaded, add it to Zip archive
	my $zip = Archive::Zip->new;
	for (my $i = 1; $i <= $total_imgs; $i++) {
		printf "Downloading #".$print_format."...", $i;
		$url = $base_page.$_."?Pages_page=".$i;
		$mech->get($url);

		$tree->delete;
		$tree = HTML::TreeBuilder->new;
		$tree->parse($mech->content);

		# Search for the right image
		my @imgs = $tree->find(_tag => 'img');
		for my $img (@imgs) {
			$img       = $img->attr("src");
			my ($ext)  = $img =~ /(\.[^.]+)$/;
			$ext       = substr $ext, 0, 4;
			my $zip_n  = sprintf($print_format.$ext, $i);
			my $save_n = $tmp_dir.$zip_n;

			# Check if file is already downloaded
			if (-e $save_n) {
				print "EXISTS!\n";
				my $zip_member = $zip->addFile($save_n, $zip_n);
			}
			elsif ($img =~ /^http:\/\/thedoujin.com\/images\//) {
				# Download the page
				die "ERROR! Failed to save \"$img\"!\nSTATUS: $mech->status\n" unless $mech->get($img, ":content_file" => $save_n);
				print "SUCCESS!\n";
				my $zip_member = $zip->addFile($save_n, $zip_n);
			}
		}
	}
	print "Compressing to Zip...";
	die "ERROR! Failed to write file to Zip!\n" unless ($zip->writeToFileNamed($title) == AZ_OK);
	print "SUCCESS!\nDoujin \"$_\" saved to \"$title\"!\n\n";

	# Clean up
	rmtree($tmp_dir, 0, 0);
	$tree->delete;
}
print "Program finished! Exiting!\n";

