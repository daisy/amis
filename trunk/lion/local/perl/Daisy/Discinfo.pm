# $Id: Discinfo.pm 1434 2006-04-20 19:12:22Z julienq $

package Daisy::Discinfo;

use strict;
use warnings;

use URI;          # we use a lot of URIs
use LWP::Simple;  # for fetching resources given their URI
use XML::LibXML;  # we want to parse some XML files too actually

use Daisy;

# Read a discinfo.html file given its URI and return a list of books that
# this file points to
sub get_books
{
  my ($parser, $uri) = @_;
  my $contents = get($uri);
  die "Could not get file at URI \"$uri\"\n" if !defined $contents;
  $parser->base_uri($uri);
  my $root = ($parser->parse_string($contents))->documentElement();
  my @books = ();
  for ($root->getElementsByTagNameNS($Daisy::XHTMLNS, "a")) {
    my $href = $_->getAttributeNS("", "href");
    my $ncc_uri = new_abs URI($href, $uri);
    push @books, new Daisy::DTB($parser, $ncc_uri, $Daisy::DTB::TITLE_ONLY);
  }
  return @books;
}

1;
