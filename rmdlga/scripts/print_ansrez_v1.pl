#!/usr/bin/perl -w

die "Parameter MUST be N" if ($#ARGV != 0);
  
$N = $ARGV[0];
$s = "";
for ($i = 0 ; $i < $N ; $i++)
{
    $s = "${s}0";
    $s2 = sprintf "%b", $i;
    print "$s 0$s2\n";
}

$s = "";
for ($i = 0 ; $i < $N ; $i++)
{
    $s = "${s}1";
    $s2 = sprintf "%b", $i;
    print "$s 1$s2\n";
}
