#!/usr/bin/perl

#I think this file strips the audio files from a Daisy book and assigns them to amisAccessibleUi.xml audio element src's
#Each audio clip has to be its own file (done in Sigtuna by making each prompt item its own section)

use strict;
use warnings;

binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

use lib qw(perl);
use Daisy;

use Getopt::Long;
use URI::file;

my $AUDIODIR = "./audio";
my $BARF = 1;
my $DEBUG = 0;
my $HELP = 0;
my $OUTFILE = undef;
my $REMOVAL = 1;
my $AMISNS = "http://amisproject.org";
GetOptions("a|audiodir=s" => \$AUDIODIR, "b|barf!" => \$BARF,
  "d|debug!" => \$DEBUG, "h|help" => \$HELP, "n|namespace" => \$AMISNS,
  "o|output=s" => \$OUTFILE, "r|removal!" => \$REMOVAL);
usage() if $HELP;
usage("Error: expected 2 arguments but got " . @ARGV . ".")
  if @ARGV != 2;

my $parser = Daisy::get_parser("perl/catalog/catalog.xml");
print STDERR "Reading DTB..." if $DEBUG;
my $book = new Daisy::DTB($parser, new_abs URI::file($ARGV[0]),
  $Daisy::DTB::FULL_BOOK);
print STDERR " done\n" if $DEBUG;
my $prompts = $parser->parse_file($ARGV[1]);
my @text_nodes = $prompts->getElementsByTagNameNS($AMISNS, "text");
for (@{$book->{toc}}) {
  if ($DEBUG) {
    print STDERR $_->to_string(), "\n";
    print STDERR "  > $_\n" for $_->{text}->audio_files();
  }
  next if $_->{class} eq "title";
  if (@text_nodes) {
    my $text = shift @text_nodes;
    my $content = Daisy::normalize($text->textContent());
    print STDERR "  = $content\n" if $DEBUG;
    if ($content ne $_->{text}->raw_text()) {
      my $msg = "Wrong prompt for " . $_->to_string() . "?! >>> $content";
      if ($BARF) {
        die "$msg; bailing out.\n";
      } else {
        warn "$msg; continuing...\n";
      }
    }
    my @files = $_->{text}->audio_files();
    if (@files) {
      my $parent = $text->parentNode();
      if ($REMOVAL) {
        $parent->removeChild($_)
          for $parent->getChildrenByTagName("audio");
      }
      my $audio = $prompts->createElement("audio");
      $audio->setAttribute(src => "$AUDIODIR/$files[0]");
      $audio->setAttribute(from => $_->{text}->{uri});
      $parent->appendChild($audio);
      warn "Too many audio files for ", $_->to_string(),
        ": kept first one out of ", scalar @files, "\n" if @files > 1;
    } else {
      warn "No audio for ", $_->to_string(), "\n";
    }
    # } else {
    # warn "No more text for ", $_->to_string(), "\n";
  }
}

# Output the result in UTF-8
if (defined $OUTFILE) {
  open OUT, ">:utf8", $OUTFILE
    or die "Could not open $OUTFILE for output: $!\n";
  print OUT $prompts->toString();
  close OUT;
} else {
  print $prompts->toString();
}

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

Usage: $0 [OPTIONS] ncc.html ui.xml

Read in an NCC file and a prompts file, then fill the audio nodes with the
audio nodes from the DTB.

Options are:
  -a, --audiodir  directory for audio files
  -b, --barf      dies when text nodes are different (also --nobarf)
  -d, --debug     output debug information (also --nodebug)
  -h, --help      show this help message
  -n, --namespace AMIS namespace
  -o, --output    output XML file
  -r, --removal   remove the previous audio nodes (also --noremoval)

USAGE
  exit defined $message;
}
