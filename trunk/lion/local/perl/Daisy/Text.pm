# A Text element, that is a node consisting of some text and associated
# audio.
# $Id: Text.pm 2322 2008-02-12 18:05:14Z julienq $

use strict;
use warnings;
use URI;

package Daisy::Text;

# Create a new Daisy text element from a NCC node (heading) or from a SMIL
# node (regular text element.) In the second case, the URI of the element as
# well as a reference to the containing book are required.
sub new
{
  my ($class, $node, $uri, $dtb, $heading) = @_;
  return defined $uri ?
    $class->_new($node, $uri, $dtb, $heading) : $class->new_heading($node);
}

# Create a new Daisy text node from a SMIL text node
sub _new
{
  my ($class, $node, $uri, $dtb, $heading) = @_;

  # Get the SMIL text reference
  my $par = $node->parentNode();
  my $textref = (grep { $_->nodeName() eq "text" }
    $par->getChildrenByTagNameNS("", "text"))[0];
  my $fragment = $node->getAttributeNS("", "id");
  $fragment = $par->getAttributeNS("", "id") if !defined $fragment;
  $uri->fragment($fragment);

  # Get the actual HTML text node
  my $src = $textref->getAttributeNS("", "src");
  my $html = $dtb->get_document($src);
  my $hfragment = new URI($src)->fragment();
  my $text = ($html->findnodes("//*[\@id='$hfragment']"))[0];

  # Get the audio items (actually, just src attributes)
  my @audio =
    map { new Daisy::Audio($_) }
    $par->getElementsByTagNameNS("", "audio");

  # Create the node
  return bless {
    uri => $uri,          # full URI of the element
    heading => $heading,  # heading that this element belongs to
    par => $par,          # par element in the SMIL file
    text => $text,        # text content in the HTML file
    audio => \@audio,     # list of audio items
  }, $class;
}

# Create a new Daisy text node from a NCC heading
sub new_heading
{
  my ($class, $heading) = @_;
  my ($smil, $uri) = $heading->{parent}->get_document($heading->{href});

  # Get the SMIL text reference
  my $fragment = new URI($heading->{href})->fragment();
  $uri->fragment($fragment);
  my $par = ($smil->findnodes("//*[\@id='$fragment']"))[0];
  $par = $par->parentNode() if $par->nodeName() eq "text";
  my $textref = (grep { $_->nodeName() eq "text" }
    $par->getChildrenByTagNameNS("", "text"))[0];

  # Get the actual HTML text node
  my $src = $textref->getAttributeNS("", "src");
  my $html = $heading->{parent}->get_document($src);
  my $hfragment = new URI($src)->fragment();
  my $text = ($html->findnodes("//*[\@id='$hfragment']"))[0];

  # Get the audio items (actually, just src attributes)
  my @audio =
    map { new Daisy::Audio($_) }
    $par->getElementsByTagNameNS("", "audio");

  # Create the node
  return bless {
    uri => $uri,       # full URI of the element
    par => $par,       # par element in the SMIL file
    text => $text,     # text content in the HTML file
    audio => \@audio,  # list of audio items
  }, $class;
}

# Get the unformatted text content
sub raw_text { Daisy::normalize($_[0]->{text}->textContent()) }

# Get the audio items as a list
sub audio { @{$_[0]->{audio}} }

# Get only audio files, regardless of begin/end times
sub audio_files
{
  my $self = shift;
  my @files = ();
  my %files = ();
  for (@{$self->{audio}}) {
    push @files, $_->{src} if !$files{$_->{src}}++;
  }
  return @files;
}

# Play a text element and call the book's callback when done.
sub play
{
  my ($self, $dtb) = @_;
  print $self->raw_text(), "\n";
  $dtb->{play_callback}->();
}

1;
