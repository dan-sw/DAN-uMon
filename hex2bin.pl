#!/usr/bin/perl
open(F,$ARGV[0]) || &my_die("Can't open file $ARGV[0], exit");
open(FF, ">$ARGV[0].bin") || &my_die("Can't open file $ARGV[0].bin, exit");
while($Line = <F>){
        @tmp = split("",$Line);
        for($i=0;$i<$#tmp;$i=$i+2){
                $hex = "0x".$tmp[$i].$tmp[$i+1];
                $bin = pack("C",hex $hex);
                print FF $bin;
        }
}

close(F);
close(FF);

sub my_die {
        my ($str) = @_;
        die "\n\033[31mERROR: $str\033[0m\n";
}
