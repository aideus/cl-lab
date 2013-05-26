#!/usr/bin/perl -w

die "Parameter MUST be N" if ($#ARGV != 0);

$N = $ARGV[0];

$s = "";
for ($i = 0 ; $i < $N ; $i++)
{
    $s = "${s}0";
    print "$s\n";
}

$s = "";
for ($i = 0 ; $i < $N ; $i++)
{
    $s = "${s}1";
    print "$s\n";
}
