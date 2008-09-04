# customize the preferences file
# usage: setup-amis-prefs.pl sourcefile entryid newvalue

use strict;
use XML::LibXML;

my $file = $ARGV[0];
my $parser = XML::LibXML->new();
my $tree = $parser->parse_file($file);
my $root = $tree->getDocumentElement;
my @entries = $root->getElementsByTagName('entry');

foreach my $entry (@entries){
  my $id = $entry->getAttribute('id');
  if ($id eq $ARGV[1]){
    $entry->setAttribute("value", $ARGV[2]);
  }
}

print $tree->toString;
