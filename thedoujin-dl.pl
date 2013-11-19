#!/usr/bin/env perl -w
use warnings;
use strict;

use LWP::Simple;
use HTML::TreeBuilder 5 -weak;
use File::Path qw(mkpath rmtree);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);

my $base_cats = "http://thedoujin.com/index.php/categories/";
my $base_page = "http://thedoujin.com/index.php/pages/";

# Set this to whatever path you want to download to
# Note: Windows doesn't have the HOME enviroment variable set
my $dl_path   = "$ENV{'HOME'}/Downloads/";
# Windows also doesn't have the /tmp/ directory.
# Instead there is C:\Windows\Temp\
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
    my $content = get $url or die "ERROR! Failed to fetch \"$url\"!\n";

    # Parse HTML to tree
    my $tree = HTML::TreeBuilder->new;
    $tree->parse($content);

    # Get Title for archive name, if it's balnk, try description field
    # If that's blank or contains a newline, use timestamp as title
    my ($title) = $tree->look_down(id => 'Categories_title')->attr("value");
    ($title)    = $tree->look_down(id => 'Categories_description')->as_text() if ($title eq "");
    ($title)    = time if ($title eq "" || $title =~ /\n.*\z/);
    print "SUCCESS!\nTitle: \"$title\"\nTotal pages: ";
    if ($title =~ /.(zip|rar)$/i) {
        $title = $dl_path.(substr($title, 0, -4).".cbz"); # Add extension
    } else {
        $title = $dl_path.$title.".cbz"; # Add extension
    }
    die "ERROR! This Doujin is already downloaded!\n" if (-e $title);

    my $total_pages = 0;
    my $total_imgs  = 0;
    for my $link ($tree->find(_tag => 'a')) {
        $link = $link->attr('href');

        if ($link =~ /^\/index.php\/categories\/$_\?/) { # Pages
            $link =~ s/^\/index.php\/categories\/$_\?Pages_page=//g;
            $total_pages = $link if ($link > $total_pages);
        } elsif ($link =~ /^\/index.php\/pages\/$_\?/) { # Images
            $link =~ s/^\/index.php\/pages\/$_\?Pages_page=//g;
            $total_imgs = $link if ($link > $total_imgs);
        }
    }

    # Loop though all the files on the front page and get their links
    my @files = ();
    for my $img ($tree->find(_tag => 'img')) {
        push (@files, "http://thedoujin.com/images/$1/$2/$3.$4") if ($img->attr('src') =~ /^http:\/\/thedoujin.com\/thumbnails\/([0-9a-z]{2})\/([0-9a-z]{2})\/thumbnail_([0-9a-z]{32}).(jpg|png|gif)$/);
    }

    # Loop through all the pages on the category page to find the file names and locations
    # I don't how what the file names are sourced from, if they're random or just a hash.
    # It looks like a SHA-1 hash, but after guessing a few times, I gave up and used this
    # method instead. It's the fastest choice, without knowing what the file names represent.
    for (my $i = 2; $i <= $total_pages; $i++) {
        $url = $base_cats."/".$_."?Pages_page=".$i;
        $content = get $url or die "ERROR! Failed to fetch \"$url\"!\n";

        $tree->delete;
        $tree = HTML::TreeBuilder->new;
        $tree->parse($content);

        if ($i == $total_pages) {
            for my $link ($tree->find(_tag => 'a')) {
                $link = $link->attr("href");

                if ($link =~ /^\/index.php\/pages\/$_\?/) { # Images
                    $link =~ s/^\/index.php\/pages\/$_\?Pages_page=//g;
                    $total_imgs = $link if ($link > $total_imgs);
                }
            }
        }

        for my $img ($tree->find(_tag => 'img')) {
            push (@files, "http://thedoujin.com/images/$1/$2/$3.$4") if ($img->attr('src') =~ /^http:\/\/thedoujin.com\/thumbnails\/([0-9a-z]{2})\/([0-9a-z]{2})\/thumbnail_([0-9a-z]{32}).(jpg|png|gif)$/);
        }
    }

    # Get format for file names
    my $zero_pad     = int((log($total_imgs) / log(10)) + 1);
    my $print_format = "%0${zero_pad}d";

    # Make directory for saved images
    my $tmp_dir = $save_dir.$_."/";
    mkpath($tmp_dir);

    # Finally, loop through and download all the images directly
    # After the file is downloaded, add it to Zip archive
    print $total_imgs."\nDownloading pages...\n";
    my $zip = Archive::Zip->new;
    for (my $i = 1; $i <= $total_imgs; $i++) {
        my $file_name = sprintf("$print_format", $i);
        print "Downloading #$file_name...";
        $file_name .= sprintf("%s", ($files[$i - 1] =~ /(\.[^.]+)$/));
        my $save_path = $tmp_dir.$file_name;

        if (-e $save_path) {
            print "EXISTS!\n";
        } else {
            my $file_url = $files[$i - 1];
            $content = get $file_url or die "ERROR! Failed to fetch \"$file_url\"!\n";
            open  FH, ">$save_path" or die "ERROR! Failed to save \"$save_path\"!\n";
            print FH $content;
            close FH;
            print "SUCCESS!\n";
        }
        $zip->addFile($save_path, $file_name);
    }
    print "Compressing to Zip...";
    die "ERROR! Failed to write file to Zip!\n" unless ($zip->writeToFileNamed($title) == AZ_OK);
    print "SUCCESS!\nDoujin \"$_\" saved to \"$title\"!\n\n";

    # Clean up
    rmtree($tmp_dir, 0, 0);
    $tree->delete;
}
print "Program finished! Exiting!\n";

