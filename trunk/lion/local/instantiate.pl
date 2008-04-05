#!/usr/bin/perl

# Instantiate XPath expressions in a template file
# We use this script for filling in the templated Microsoft RC file with values from amisAccessibleUi.xml
# $Id: instantiate.pl 2325 2008-02-14 21:36:00Z julienq $

use strict;
use warnings;

use Getopt::Long;
use XML::LibXML;
use XML::LibXML::XPathContext;
use open ":utf8";
binmode STDERR, ":utf8";
binmode STDOUT, ":utf8";

my $HELP = 0;
my $DEBUG = 0;
my $OUTFILE = "-";
my $EXTHELP = 0;
my $AMISNS = "http://amisproject.org";
GetOptions("o|output=s" => \$OUTFILE, "d|debug!" => \$DEBUG, help => \$HELP,
  "n|namespace" => \$AMINS, "x|extension-functions" => \$EXTHELP);
usage() if $HELP;
extension_functions() if $EXTHELP;

usage("Error: expected 2 arguments but got " . @ARGV . ".")
  if @ARGV != 2;
my ($template_file, $xmlfile) = @ARGV;

# Parse the XML file and get a context for xpath matching
my $parser = new XML::LibXML();
my $xmldoc = $parser->parse_file($xmlfile);
my $context = new XML::LibXML::XPathContext($xmldoc->documentElement());

# Get the normalized text content of a node
sub normalized_text
{
  my $node = shift;
  local $_ = $node->textContent();
  s/\s+/ /g;
  s/^ //;
  s/ $//;
  return $_;
}

### Extension function: string caption(nodelist, boolean?)
### The caption functions produces a caption from an action item or a
### container. The caption is made up by getting the accelerator of the action,
### its text and its mnemonic. The mnemonic is inserted at the first possible
### spot. The optional boolean parameter is used to omit the accelerator when
### set to false (it is true by default.)
my $caption = sub
{
  my ($nodelist, $append_accelerator) = @_;
  $append_accelerator = 1 if !defined $append_accelerator;
  my $action = $nodelist->get_node(0);
  my $textelem =
    $action->find("(caption/text)|text|(caption/promptItem)|promptItem")->
      get_node(0);
  local $_;
  if ($textelem->nodeName() eq "promptItem") {
    my $refid = $textelem->getAttributeNS($AMISNS, "refid");
    $_ = normalized_text($action->find("//promptItem[\@id='$refid']/text")->
      get_node(0));
  } else {
    $_ = normalized_text($textelem);
  }
  # Get the mnemonic
  my @mnemonics = $textelem->find("../../mnemonic/text")->get_nodelist();
  if (@mnemonics) {
    my $mnemonic = normalized_text($mnemonics[0]);
    $_ .= " (&$mnemonic)" if !s/$mnemonic/&$&/i;
  }
  # Get the accelerator (if needed)
  if ($append_accelerator) {
    my @accels = $textelem->find("../../accelerator/text")->get_nodelist();
    if (@accels) {
      my $accelerator = normalized_text($accels[0]);
      $_ .= "\\t$accelerator" if $accelerator;
    }
  }
  return $_;
};

# Register all the XPath functions that can be used in the template file
$context->registerFunction(caption => $caption);

# Read in the template file and replace replace replace
open TEMPLATE, $template_file
  or die "Could not open template file $template_file: $!\n";
open OUT, ">$OUTFILE"
  or die "Could not open $OUTFILE for output: $!\n";
while (<TEMPLATE>) {
  while (/\$<([^>]+)>\$/g) {
    my $xpath = $1;
    print STDERR ">>> $xpath\n" if $DEBUG;
    my $value = eval { $context->findvalue($xpath) };
    if ($@) {
      warn
        "Could not evaluate XPath expression ($xpath) at $template_file:$.\n";
    } else {
      s/\$<\Q$xpath\E>\$/$value/;
      print STDERR "=== ", $context->findvalue($xpath), "\n\n" if $DEBUG;
    }
  }
  print OUT;
}
close OUT;
close TEMPLATE;

# Prints the usage information. If a message argument is given, output it
# first then quit with an error code of 1. When no message is given, exit
# normally.
sub usage
{
  my $message = shift;
  if (defined $message) {
    chomp $message;
    print "$message\n";
  }
  print <<USAGE;

Usage: $0 [OPTIONS] template lang.xml

Read in a template file and a localization XML file to create the localized
file.

Options are:
  -d, --debug                print some debugging output to stderr
  -h, --help                 show this help message
  -n, --namespace            elements namespace
  -o, --output               output to a file instead of stdout
  -x, --extension-functions  show the list of custom extension functions

USAGE
  exit defined $message;
}

# Outputs the documentation for the extention functions. This documentation
# should be insterted in this script with comments block starting with ###
sub extension_functions
{
  my $self = $0;
  my @doc = ();
  open SELF, $self or die "Cannot open self source file $self: $!\n";
  local $/ = "\n\n";
  while (<SELF>) {
    my $doc = join "", map { s/^### ?//; "$_\n" } grep { /^###/ } split /\n/;
    push @doc, $doc if $doc;
  }
  close SELF;
  print join("\n", @doc);
  exit 0;
}
