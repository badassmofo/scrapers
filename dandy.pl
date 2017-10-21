#!/usr/bin/env perl -X

use feature qw(switch);
use open ':std', ':encoding(UTF-8)';

use LWP::Simple;
use HTML::TreeBuilder 5 -weak;

sub get_dandy {
    my $id = sprintf('%03d', $_[0]);
    my $tree = HTML::TreeBuilder->new;
    $tree->parse(get "http://www.choi-waru.com/dandy-$id" or die "ERROR! Failed to fetch URL!");

    my $desc = $tree->look_down(
        _tag => 'div',
        'id' => 'titleBox'
    )->look_down(
        _tag => 'p'
    )->as_text();
    print("\ndescription:\n\t$desc\n\n");

    my $desc = $tree->look_down(
        _tag => 'div',
        'id' => 'titleData'
    );
    my @dt = $desc->look_down(_tag => 'dt');
    my @dd = $desc->look_down(_tag => 'dd');

    print("info:\n");
    for (my $i = 0; i < scalar(@dd) - 1; $i++) {
        unless (defined(@dt[$i])) {
            print("\n");
            system("mpv http://151.mediaimage.jp/dandy_$id.mp4");
            $tree->delete();
            return;
        }
        print("\t".@dt[$i]->as_text().": ".@dd[$i]->as_text()."\n");
    }
}

sub newest_dandy {
    my $tree = HTML::TreeBuilder->new;
    $tree->parse(get "http://www.choi-waru.com/?s=dandy" or die "ERROR! Failed to fetch URL!");

    $new = $tree->look_down(
        _tag => 'div',
        'id' => 'resultList'
    )->look_down(
        _tag => 'a'
    );
    $new->attr('href') =~ /^http:\/\/www\.choi-waru\.com\/dandy-(\d+)\/$/;
    $tree->delete();

    return $1;
}

sub random_dandy {
    return int(rand(newest_dandy())) + 1;
}

foreach (@ARGV) {
    given (lc($_)) {
        when('newest')  { get_dandy(newest_dandy()) }
        when('random')  { get_dandy(random_dandy()) }
        when(/^(\d+)$/) { get_dandy($_)       }
        default         { die "ERROR! Invalid argument \"$_\"!" }
    }
}
