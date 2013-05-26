#!/usr/bin/perl -w

die "Parameter MUST be N len dub" if ($#ARGV != 2);

$N   = $ARGV[0];
$len = $ARGV[1];
$dub = $ARGV[2];


sub make_one()
{
    $rez = "";
    for ($i = 0 ; $i < $len ; $i++)
      {
	  $a = 1;
	  $a = 0 if (int(rand(2)));	  
	  for ($j = 0 ; $j < $dub ; $j++)
	    {
		$rez = "${rez}$a";
	    }
      }
    return $rez;
}

while (keys %all != $N)
{
    $s = make_one();
    $all{$s} = 1;
}

print join("\n",keys %all) . "\n";
