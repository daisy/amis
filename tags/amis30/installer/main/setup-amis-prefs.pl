# customize the preferences file
# usage: setup-amis-prefs.pl sourcefile entryid newvalue entryid newvalue ... 

use strict;
use XML::LibXML;

my $file = $ARGV[0];
my $parser = XML::LibXML->new();
my $tree = $parser->parse_file($file);
my $root = $tree->getDocumentElement;

foreach my $n (1 .. $#ARGV){
    my $elm = findElementById($root, $ARGV[$n]);
    if ($elm){
        my $value = $ARGV[++$n];
       $elm->setAttribute("value", $value);
        my $temp = $elm->getAttribute("value");
    }
}

print $tree->toString;

sub findElementById{
    my ($root, $id) = @_;
    my @entries = $root->getElementsByTagName('entry');
    foreach my $entry (@entries){
        if ($entry->getAttribute("id") eq $id){
            return $entry;
        }
    }
}
