# $Id: Daisy.pm 2322 2008-02-12 18:05:14Z julienq $

package Daisy;

use strict;
use warnings;

use LWP::Simple;      # for fetching outside documents
use XML::LibXML;      # we are in control of the global parser

use Daisy::Audio;     # audio elements
use Daisy::Discinfo;  # a discinfo file pointing to DAISY books
use Daisy::DTB;       # a book object, read from an NCC file
use Daisy::Heading;   # a heading from the NCC
use Daisy::Text;      # a text node from a DTB
use Daisy::Toc;       # for table of contents and headings

our $CATALOG = "./catalog/catalog.xml";         # catalog file location
our $XHTMLNS = "http://www.w3.org/1999/xhtml";  # used for parsing XHTML

# Get a non-validating parser using the aforementioned catalog
sub get_parser
{
  my $catalog = shift;
  my $parser = new XML::LibXML();
  $parser->validation(0);
  $parser->load_catalog($catalog) if defined $catalog;
  return $parser;
}

# Whitespace normalization of a string
sub normalize
{
  local $_ = shift;
  s/\s+/ /g;
  s/^ //;
  s/ $//;
  return $_;
}

1;
