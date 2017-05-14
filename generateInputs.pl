#!/usr/bin/perl -w

use strict;
use warnings;

# (1) quit unless we have the correct number of command-line args
my $num_args = $#ARGV + 1;
if ($num_args != 2) {
    print "\nUsage: generateInputs.pl time(Min) workingDir\n";
    exit;
}

my $time = $ARGV[0];
my $dir = $ARGV[1];

chomp $time;
chomp $dir;

print "SIM. Time: ".$time." Min \n";
print "Working Dir: ".$dir."\n";

main($time,$dir); #Start program




#Subroutines
sub main
{
    #Constant that models input from user
my $time=scalar($_[0]);
my $ppal_dir =scalar($_[1]);
my @time_sim=(60)x$time;

#Loop over several hours    
for (my $i=0; $i<$time; $i++) { 
    #Reads intensity file
    my $filename = 'IntSpectralIndexFlux.txt';
    my $seed=int(rand(60*$time));
    open(my $fh, '<:encoding(UTF-8)',$filename)
        or die "Could not open file '$filename' $!";
        
    #Loop over lines in intensity file
    while (my $row = <$fh>) {
        chomp $row;
        my @line = split /\t/, $row;
        if ( $line[1] ne "Z") { #Skip 1st line 
        my $Z = $line[1];
        my $A = $line[2];
        my $alpha = $line[4];
        my $totalFlux = int($line[5]*$time_sim[$i]);
        my $e_min = $line[6];
        my $cka_ID = 0;
        if ($Z == 1) { $cka_ID = 14;} #Calculates CORSIKA ID
        else { $cka_ID = $A*100+$Z} 

        #call to subrutine that creates the input file
        createInput($Z,$totalFlux,$cka_ID,$alpha,$e_min,$ppal_dir,$seed,$i);
        }
    }
    close $fh;
}

}


sub createInput {
 #Reads iinputs file
    my $filename1 = 'inputs';
    open(my $fh1, '<:encoding(UTF-8)',$filename1)
	or die "Could not open file '$filename1' $!";

 #Opens file to be written
    my $n = scalar($_[0]);
    my $flux = scalar($_[1]);
    my $cka_ID= scalar($_[2]);
    my $alpha = scalar($_[3]);
    my $e_min = scalar($_[4]);
    my $ppal_dir = scalar($_[5]);
    my $seed=scalar($_[6]);
    my $fnumber=scalar($_[7])+1;
    my $bool_seed=0;
    my $w_filename = 'inputs_Z'.$n.'_'.$fnumber;
    
    open(my $fhw,'>',$w_filename) or die "Could not open file '$w_filename' $!";
 #Loop over lines in inputs file
    while (my $row1 = <$fh1>) {
	my @actual_line = split /\t/, $row1;	   
	
	#Writes total flux to archive
	if($actual_line[0] eq "NSHOW") {
        my $row_change = "NSHOW\t".$flux."\n";
        print $fhw $row_change;
	}
	#Primary particle
	elsif($actual_line[0] eq "PRMPAR") {
        my $row_change = "PRMPAR\t".$cka_ID."\n";
        print $fhw $row_change;
	}	
        #Energy slope
	elsif($actual_line[0] eq "ESLOPE") {
        my $row_change = "ESLOPE\t".$alpha."\n";
        print $fhw $row_change;
	}
        #Energy Range
	elsif($actual_line[0] eq "ERANGE") {
        my $row_change = "ERANGE\t".$e_min."\t"."1.E6\n";
        print $fhw $row_change;
	}    
    #Change SEED
    elsif($actual_line[0] eq "SEED" && !$bool_seed) {
        my $row_change = "SEED\t".$seed."\t"."0"."\t"."0\n";
        $bool_seed=1;
        print $fhw $row_change;       
    }
    
	#WRite Directory
	elsif($actual_line[0] eq "DIRECT") {
        my $row_change = "DIRECT\t".$ppal_dir."/".$cka_ID.$flux.$fnumber."\n";
        print $fhw $row_change;
	}
	#Write other lines
	else {
	    print $fhw $row1;
	}
    }
    close $fhw;
    close $fh1;
}
