#!/usr/bin/perl

use strict;
use warnings;

use CGI qw/:standard/;

my $FORM_TEMPLATE = "form.in";

my $q = new CGI;
print $q->header(-charset => "UTF-8");
my %params = map { $_ => $q->param($_) } $q->param();
print "<p>$_ = $params{$_}</p>\n" for keys %params;

sub output_template
{
  my $template = shift;
  my %params = @_;
  open TEMPLATE, $template or _die("Missing template file \"$template\".");
  my $cond = 0;
  my $buf = "";
  while (<TEMPLATE>) {
    if (s/^#s(\s+)/  $1/) {
      $buf = $_;
    } elsif (s/^#s/s/) {
      eval "\$buf =~ $_";
      print $buf;
    } elsif (/^#if /) {
      $cond = eval $';
    } elsif (/^#endif/) {
      $cond = 0;
    } elsif (s/^#/ /) {
      print $_ if $cond;
    } else {
      print $_;
    }
  }
  close TEMPLATE;
}

sub _die
{
  my $message = join "", @_;
  print <<HTML;
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.1//EN"
  "http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="Content-Type" content="text/xml+html; charset=UTF-8"/>
    <title>Amis localization interface</title>
    <style type="text/css">
      body { background-color: white; color: black; font-family: sans-serif }
      td { border: solid black thin; padding: 4px }
    </style>
  </head>
  <body>
    <h1>Amis localization interface</h1>
    <h2>Error</h2>
    <p>$message</p>
  </body>
</html>
HTML
}
