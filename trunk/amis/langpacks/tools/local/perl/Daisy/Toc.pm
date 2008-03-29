# Table of contents of a Daisy book--a flat list of headings.
# $Id: Toc.pm 1621 2006-08-31 17:26:39Z julienq $

use strict;
use warnings;

package Daisy::Toc;

# Read the toc of a book and return a list of headings.
# TODO at the moment only h1..6 elements are read; need to read spans and
# divs as well.
sub new
{
  my ($class, $dtb) = @_;
  # Create a new heading for all h children of the body
  my @body = $dtb->{ncc}->getElementsByTagNameNS($Daisy::XHTMLNS, "body");
  die "Toc::new: NCC file has no body element.\n" if !@body;
  warn "Toc::new: NCC file has several body elements.\n" if @body > 1;
  my @toc =
    map { new Daisy::Heading($dtb, $_) }
    grep { $_->nodeName() =~ /^h\d$/ }
    $body[0]->childNodes();
  return bless \@toc, $class;
}

# Dump a toc to STDOUT.
sub dump
{
  my $self = shift;
  for (@$self) {
    print "  " x ($_->{level} - 1);
    print $_->to_string();
    print "\n";
  }
}

# Get only the title of a book (when reading discinfo files.) The first
# heading must be h1 and have a class="title" attribute. Return only a
# heading, not a full TOC.
sub get_title
{
  my $dtb = shift;
  my $h1 = ($dtb->{ncc}->getElementsByTagNameNS($Daisy::XHTMLNS, "h1"))[0];
  die if $h1->getAttributeNS("", "class") ne "title";
  return new Daisy::Heading($dtb, $h1);
}

1;
