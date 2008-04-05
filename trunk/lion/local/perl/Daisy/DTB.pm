# The Daisy 2.02 Digital Talking Book object.
# $Id: DTB.pm 1621 2006-08-31 17:26:39Z julienq $

package Daisy::DTB;

use strict;
use warnings;
use URI;
use Daisy::Text;

our $TITLE_ONLY = 1;  # when we only want the title
our $FULL_BOOK = 0;   # when we need to read the whole thing

# Create a new DTB from the URI of its NCC file (considered as the base URI
# of the book.) Use the optional "title only" argument to not read the full
# TOC (useful for discinfo.)
sub new
{
  my ($class, $parser, $uri, $title_only) = @_;
  $title_only = 0 if !defined $title_only;

  # Create the object for the book
  my $self = bless {
    parser => $parser,  # keep a parser around to parse referenced files
    base_uri => $uri,   # URI of the NCC file
    uri_map => {},      # map of files already parsed (DOM documents)
    ncc => undef,       # the ncc DOM document
    title => undef,     # the title heading
    toc => undef,       # the table of contents
    text => [],         # list of all text elements in order
  }, $class;

  # Parse the NCC document
  eval { $self->{ncc} = $parser->parse_file($uri->path()) };
  die "Parse error for $uri:\n$@\n" if $@;

  # Get the title element only, or read the whole book
  $self->{title} = Daisy::Toc::get_title($self);
  $self->read_contents() if !$title_only;

  # Done
  return $self;
}

# Read the contents of the book
sub read_contents
{
  my $self = shift;

  # Read the full TOC
  $self->{toc} = new Daisy::Toc($self);

  # Read the text from the SMIL files. First, make a list of all the SMIL
  # files to read.
  my @smil = ();
  my %smil = ();
  my %head_map = ();
  my $current_heading = undef;
  for (@{$self->{toc}}) {
    my $uri = $_->{text}->{uri};
    $head_map{$uri} = $_;
    $current_heading = $_ if !defined $current_heading;
    $uri->fragment(undef);
    push @smil, $uri if !$smil{$uri}++;
  }

  # Then read all the text elements of the SMIL files in the list.
  for my $base_uri (@smil) {
    my $smil = $self->get_document($base_uri);
    for ($smil->getElementsByTagNameNS("", "text")) {
      my $text = new Daisy::Text($_, $base_uri, $self, $current_heading);
      if (exists $head_map{$text->{uri}}) {
        $current_heading = $head_map{$text->{uri}};
        $current_heading->{position} = scalar @{$self->{text}};
        push @{$self->{text}}, $current_heading;
      } else {
        push @{$current_heading->{contents}}, $text;
        push @{$self->{text}}, $text;
      }
    }
  }
  return $self;
}

# Get the document element of the document pointed to by the uri and store it
# in the map. If it was already seen, just return the one previously stored.
# If called in list context, return the actual URI as a second parameter.
sub get_document
{
  my ($self, $_uri) = @_;
  # The actual URI of the file needs to be resolved with the DTB's as its
  # base, and a possible fragment should be removed.
  my $uri = new_abs URI($_uri, $self->{base_uri});
  my $full_uri = $uri;
  $uri->fragment(undef);
  if (!exists $self->{uri_map}->{$uri}) {
    # This is the first time that we are seeing this one so let's fetch it,
    # parse it and store it in the URI map
    my $doc;
    eval { $doc = $self->{parser}->parse_file($uri->path()) };
    die "Parse error for $uri:\n$@\n" if $@;
    $self->{uri_map}->{$uri} = $doc;
  }
  return wantarray ?
    ($self->{uri_map}->{$uri}, $full_uri) : $self->{uri_map}->{$uri};
}

# Play the book from the start
sub play
{
  my $self = shift;
  $self->{play_position} = 0;
  $self->{text}->[$self->{play_position}]->play($self);
}

# Advance to the next heading if it exists, otherwise do nothing
sub next_heading
{
  my $self = shift;
  my $pos = $self->{play_position};
  my $heading = $self->{text}->[$self->{play_position}];
  $heading = $heading->{heading} if ref $heading ne "Daisy::Heading";
  $self->{play_position} =
    $heading->{position} + @{$heading->{contents}} + 1;
  $self->{play_position} = $pos if $self->{play_position} >= @{$self->{text}};
}

1;
