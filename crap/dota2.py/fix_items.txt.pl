#!/usr/bin/env perl -X

sub pretty {
  my $ret =  $_[0];
     $ret =~ tr/_/ /;
     $ret =~ s/([\w']+)/\u\L$1/g;
     $ret =~ s/Of/of/g;
  return $ret;
}

my %id_book       = ();
my %recipies_book = ();
my @recipies      = ();
while (<>) {
  my ($id, $alias, $key) = $_ =~ /(\d+):(.*)-(.*)/g;
  $key =~ /^item_(?<name>.*)/g;

  if ($alias eq "null") {
    if ($+{name} =~ /^recipe_(?<r_name>.*)/g) {
      $id_book{$+{r_name}} = $id;
      push(@recipies, $+{r_name});
    } else {
      my $x = pretty($+{name});
      $recipies_book{$+{name}} = $x;
      print "$id:$x\n";
    }
  } else {
    my $x = pretty(substr($alias, rindex($alias, ';') + 1));
    $recipies_book{$+{name}} = $x;
    print "$id:$x\n";
  }
}

for my $recipie (@recipies) {
  print $id_book{$recipie}.":".$recipies_book{$recipie}." Recipie\n";
}
