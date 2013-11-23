#!/usr/bin/env perl -w
use warnings;
use strict;

use LWP::UserAgent;
use HTML::TreeBuilder 5 -weak;
use File::Path qw(mkpath rmtree);
use Archive::Zip qw(:ERROR_CODES :CONSTANTS);
use DateTime;

my $def_format = "[%a] %t (%s) [%l]";
my $dl_path    = "$ENV{'HOME'}/Downloads/";
my $save_dir   = "/tmp/";

my $last_format = $def_format;
my $last_cat    = "doujinshi";
foreach (@ARGV) {
    if ($_ =~ /^doujinshi$/) { # Download from fakku/doujinshi
        $last_cat = $_;
        print "Setting category to doujinshi!\n\n";
        next;
    } elsif ($_ =~ /^manga$/) { # Download from fakku/manga
        $last_cat = $_;
        print "Setting category to manga!\n\n";
        next;
    } elsif ($_ =~ /^--f=+?/) { # Get format for zip output
        $_ =~ s/--f=//g;
        if (length $_ == 0) {
            print "ERROR! No format passed! Using default!\n";
            $last_format = $def_format;
        } else {
            print "Setting format to: \"$_\"!\n\n";
            $last_format = $_;
        }
        next;
    } elsif ($_ =~ /^(manga|doujinshi)\/(.*)$/) {
        $last_cat = $1;
        $_ = $2;
    }

    # Check if format is valid
    if ($last_format !~ /%/) {
        print "ERROR! Format contains no specifiers! Using default!\n";
        $last_format = $def_format;
    }

    print "Downloading Doujin \"$_\"...\nGathering info...";

    # Cloudflare blocks access without a user-agent
    my $ua = LWP::UserAgent->new(timeout => 10, agent => 'Mozilla/5.0 (Windows NT x.y; WOW64; rv:10.0) Gecko/20100101 Firefox/10.0');

    # Form URL and GET page
    my $url = "http://www.fakku.net/".$last_cat."/".$_;
    my $response = $ua->get($url);
    die "ERROR! Failed to fetch \"$url\"!" unless $response->code == '200';
    my $content = $response->content;

    # Parse page into tree
    my $tree = HTML::TreeBuilder->new;
    $tree->parse($content);

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
    my $dt = undef;
    for my $bold (@bolds) {
        $bold = $bold->as_text;
        $total_pages = $bold if ($bold =~ /^\d+?$/);
        if ($bold =~ /^(\w+) (\d+), (\d+)$/) {
            my %mon2num = qw(January 1  February 2  March 3  April 4  May 5  June 6 July 7  August 8  September 9  October 10 November 11 December 12);
            $dt = DateTime->new(
                year  => $3,
                month => $mon2num{$1},
                day   => $2);
        }
    }
    print $total_pages."\nDownloading pages...\n";

    my @imgs = $tree->look_down(_tag => 'img');
    my $final_link = '';
    for my $img (@imgs) {
        $img = $img->attr('src');
        $final_link = $img if $img =~ /^http:\/\/t\.fakku\.net\/images\/manga\/(d|m)\/(.*)\/thumbs\/001\.thumb\.jpg$/;
    }
    $final_link =~ s/thumb\.//g;
    $final_link =~ s/thumbs/images/g;
    $final_link = substr $final_link, 0, -7;

    my $tmp_dir = $save_dir.$_."/";
    mkpath($tmp_dir);

    my $zip = Archive::Zip->new;
    for (my $i = 1; $i <= $total_pages; $i++) {
        printf "Downloading #%03d...", $i;

        my $file_name = sprintf("%03d.jpg", $i);
        my $file_url  = $final_link.$file_name;
        my $save_path = $tmp_dir.$file_name;

        # Check if file is already downloaded
        if (-e $save_path) {
            print "EXISTS!\n";
        } else {
            # Download the page
            my $response = $ua->get($file_url);
            die "ERROR! Failed to get \"$file_url\"!" unless $response->code == '200';
            my $content = $response->content;
            open FH, ">$save_path" or die "ERROR! Failed to save file \"$save_path\"!";
            print FH $content;
            close FH;
            print "SUCCESS!\n";
        }
        $zip->addFile($save_path, $file_name);
    }
    print "Compressing to Zip...";
    $out =~ tr/\\\///ds;
    die "ERROR! Failed to write file to Zip!\n" unless ($zip->writeToFileNamed($dl_path.$out) == AZ_OK);
    print "SUCCESS!\nDoujin \"$_\" saved to \"$out\"!\n\n";

    # Clean up
    rmtree($tmp_dir, 0, 0);
    $tree->delete;
}
print "Program finished! Exiting!\n";

