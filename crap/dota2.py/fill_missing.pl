#!/usr/bin/env perl -X

my $last = -1;
while (<>) {
  my ($id, $alias) = $_ =~ /^(\d+):(.*)$/g;
  my $loop_v = ($id - $last);
  if ($loop_v > 1) {
    for (my $i = 1; $i < $loop_v; $i++) {
      my $new = $last + $i;
      print "$new:null\n";
    }
  }
  print "$id:$alias\n";
  $last = $id;
}
