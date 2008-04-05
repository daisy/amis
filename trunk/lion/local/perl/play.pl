#!/usr/bin/perl

use strict;
use warnings;

binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

use lib qw(.);
use Daisy;
use URI::file;
use Getopt::Long;
use Term::ReadKey;

BEGIN { ReadMode "raw"; }
END { ReadMode "normal"; }

my $HELP = 0;
GetOptions("h|help" => \$HELP);
usage() if $HELP;

my $parser = Daisy::get_parser("./catalog/catalog.xml");
for (@ARGV) {
  my $book = new Daisy::DTB($parser, new_abs URI::file($ARGV[0]),
    $Daisy::DTB::FULL_BOOK);
  $book->{play_callback} = sub
  {
    # Wait for a key press
    my $key;
    1 while (!defined ($key = ReadKey(0)));
    # Continue if we're not quitting or not at the end of the book
    ++$book->{play_position};
    if (defined $book->{text}->[$book->{play_position}]) {
      if ($key =~ /^n$/i) {
        # n goes to the next heading
        $book->next_heading();
        $book->{text}->[$book->{play_position}]->play($book);
      } elsif ($key =~ /^t$/i) {
        # t shows the table of contents
        $book->{toc}->dump();
        $book->{play_callback}->();
      } elsif ($key !~ /^q$/i) {
        # q quits
        $book->{text}->[$book->{play_position}]->play($book);
      }
    }
  };
  $book->play();
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

Usage: $0 [OPTIONS] ncc.html [ncc.html]

Plays a Daisy book (or several in succession.)

Options are:
  -h, --help      show this help message

USAGE
  exit defined $message;
}
