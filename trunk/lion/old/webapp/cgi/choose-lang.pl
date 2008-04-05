#!/usr/bin/perl

# CGI script for choosing the language.

use strict;
use warnings;

use CGI qw/:standard/;

my $q = new CGI;
print $q->header(-charset => "UTF-8");
my %params = map { $_ => $q->param($_) } $q->param();
if (!exists $params{lang} || $params{lang} ne "") {
  output_html("choose-lang.html");
} else {
  output_html("$params{lang}/choose-form.html");
}

sub output_html
{
  my $file = shift;
  open HTML, $file or _die("Error opening file $file: $!.");
  print while <HTML>;
  close HTML;
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
    <title>Amis localization interface: Error</title>
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
  exit;
}
