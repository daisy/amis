#!/usr/local/bin/perl

#create resource.h.ini from resource.h

open(INFILE, "<./src/AmisGuiMFC2/resource.h");
@lines = <INFILE>;

open(OUTFILE, ">./settings/resource.h.ini");
print OUTFILE ";\n[resources]\n";

$re_comments = '^//.*$'; 
$re_define = '^#define ';
$re_spaces = '[ ]+';
$re_ifdef = '^#ifdef.*$';
$re_ifndef = '^#ifndef.*$';
$re_endif = '^#endif.*$';

foreach $line (@lines)
{
  $x = $line;
  $x =~ s/$re_comments//g;
  $x =~ s/$re_define//g;
  $x =~ s/$re_spaces/=/g;
  $x =~ s/$re_ifdef//g;
  $x =~ s/$re_ifndef//g;
  $x =~ s/$re_endif//g;
  print ">>$x\n";
  
  print OUTFILE "$x"; 
}
close(INFILE);
close(OUTFILE);

print "done\n";
