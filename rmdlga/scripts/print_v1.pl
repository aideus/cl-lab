#!/usr/bin/perl -w

die "Parameter MUST be N" if ($#ARGV != 0);
  
$N = $ARGV[0];
$k = int(log($N) / log(2.0));
for ($i = 0 ; $i < $N ; $i++)
{
    $s = sprintf "%b", $i;
    $s = sprintf("%0${k}d", $s);
    $s1 = substr($s, 0,1) . "$s";
    print "$s1\n";
}
