# Audio elements
# $Id: Audio.pm 1434 2006-04-20 19:12:22Z julienq $

use strict;
use warnings;

package Daisy::Audio;

# Create a new audio element from an audio node
sub new
{
  my ($class, $node) = @_;
  my $src = $node->getAttributeNS("", "src");
  my $id = $node->getAttributeNS("", "id");
  my $begin = $node->getAttributeNS("", "clip-begin");
  $begin =~ s/^npt=(.*)s$/$1/;
  my $end = $node->getAttributeNS("", "clip-end");
  $end =~ s/^npt=(.*)s$/$1/;

  return bless {
    src => $src,      # source of the audio file
    id => $id,        # id of the audio element
    begin => $begin,  # begin time
    end => $end,      # end time
  }, $class;
}

# String version for inline display
sub to_string
{
  my $self = shift;
  return "$self->{src} \@$self->{id} [$self->{begin}-$self->{end}]";
}

sub play
{
  my $self = shift;
  print "[PLAY AUDIO] ", $self->to_string(), "\n";
}

1;
