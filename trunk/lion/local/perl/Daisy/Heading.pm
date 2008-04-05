# Headings of a Daisy book
# $Id: Heading.pm 1434 2006-04-20 19:12:22Z julienq $

use strict;
use warnings;

package Daisy::Heading;

# Create a new toc item from an XML heading element
sub new
{
  my ($_class, $parent, $node) = @_;

  die "Cannot create a heading for ", $node->localname(), "\n"
    if $node->localname() !~ /^h(\d)/;
  my $level = $1;
  my $id = $node->getAttributeNS("", "id");
  my $class = $node->getAttributeNS("", "class");
  $class = "" if !defined $class;
  my @a = $node->getChildrenByTagNameNS($Daisy::XHTMLNS, "a");
  die "Heading has no anchor child\n" if !@a;
  my $href = $a[0]->getAttributeNS("", "href");
  my $title = Daisy::normalize($a[0]->textContent());

  # Create the object and return it
  my $self = bless {
    parent => $parent,   # parent DTB object
    html_node => $node,  # the HTML node in the NCC file
    level => $level,     # heading level (1 through 6)
    id => $id,           # heading element id
    class => $class,     # heading class
    href => $href,       # URI of the SMIL element for the heading
    title => $title,     # heading title
    text => undef,       # SMIL text (= parsed par element)
    contents => [],      # further text contents of this heading
  }, $_class;

  # Get the SMIL text for this node
  $self->{text} = new Daisy::Text($self);

  return $self;
}

# String representation of the heading (compact representation.)
sub to_string
{
  my $self = shift;
  my $class = $self->{class} ? "$self->{class}; " : "";
  return "-$self->{level}- $self->{title} ($class$self->{href})";
}

# Play the heading, i.e. its text.
sub play
{
  my $self = shift;
  $self->{text}->play($self->{parent});
}
1;
