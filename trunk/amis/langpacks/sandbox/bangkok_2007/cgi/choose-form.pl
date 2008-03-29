#!/usr/bin/perl

# CGI script for choosing/filling the form

use strict;
use warnings;

use CGI qw/:standard/;
use XML::LibXML;

# Encoding stuff
use Encode;
binmode STDOUT, ":utf8";
binmode STDIN, ":utf8";
binmode STDERR, ":utf8";

# HTML and XML namespaces
my $HTMLNS = "http://www.w3.org/1999/xhtml";
my $XMLNS = "http://www.w3.org/XML/1998/namespace";

# Get the parameters and display the form chooser or the form if its
# specified
my $q = new CGI;
print $q->header(-charset => "UTF-8");
my %params = map { $_ => decode(utf8 => $q->param($_)) } $q->param();
if (!exists $params{lang} || $params{lang} eq "") {
  output_html("choose-lang.html");
} elsif (!exists $params{form} || $params{form} eq "") {
  output_html("$params{lang}/choose-form.html");
} else {
  fill_html(%params);
}

# Simply output an existing HTML document
sub output_html
{
  my $file = shift;
  open HTML, "<:utf8", $file or _die("Error opening file $file: $!.");
  print while <HTML>;
  close HTML;
}

# Output an HTML document (a form), but first fill it with the contents of
# the XML file. Also, save the changes that have been made previously.
sub fill_html
{
  my %params = @_;
  my $lang = $params{lang};
  my $form = $params{form};
  my $html_file = "$lang/$form.html";
  my $xml_file = "$lang/ui.xml";
  my $parser = new XML::LibXML();
  my $html_doc = read_xml($parser, $html_file);
  my $xml_doc = read_xml($parser, $xml_file);
  # Make an id map for XML elements because without a DTD the parser does
  # not know that they are not ids
  my %ids = map { $_->getAttribute("id") => $_ }
    grep { $_->getAttribute("id") }
    $xml_doc->getElementsByTagName("text");
  # Let's commit the changes that we have made
  for (grep /^status(_.+)$/, keys %params) {
    /^status(_.+)$/;
    my $xml_elem = $ids{$1};
    _die("Could not find text node for id $1?!") if !defined $xml_elem;
    $xml_elem->setAttribute(status => $params{$_});
  }
  for (grep /^_/, keys %params) {
    my $xml_elem = $ids{$_};
    _die("Could not find text node for id $_?!") if !defined $xml_elem;
    $xml_elem->removeChildNodes();
    $xml_elem->appendText(normalize_space($params{$_}));
  }
  my %status =
    map { $_->getAttribute("value") . $_->getAttribute("name"), $_ }
    grep { $_->getAttribute("type") eq "radio" }
    $html_doc->getElementsByTagNameNS($HTMLNS, "input");
  # And show them in the HTML version
  for ($html_doc->getElementsByTagNameNS($HTMLNS, "textarea")) {
    my $id = $_->getAttribute("name");
    my $xml_elem = $ids{$id};
    _die("Could not find text node for id $id?!") if !defined $xml_elem;
    $_->removeChildNodes();
    $_->appendText($xml_elem->textContent());
    my $class = $xml_elem->getAttribute("status");
    if ($class) {
      $_->setAttribute(class => $class);
      if ($class eq "ok") {
        $status{"okstatus$id"}->setAttribute(checked => "true");
        $status{"checkstatus$id"}->removeAttribute("checked");
        $status{"todostatus$id"}->removeAttribute("checked");
        $xml_elem->setAttributeNS($XMLNS, lang => $lang);
      } elsif ($class eq "check") {
        $status{"okstatus$id"}->removeAttribute("checked");
        $status{"checkstatus$id"}->setAttribute(checked => "true");
        $status{"todostatus$id"}->removeAttribute("checked");
        $xml_elem->setAttributeNS($XMLNS, lang => $lang);
      } else {
        $status{"okstatus$id"}->removeAttribute("checked");
        $status{"checkstatus$id"}->removeAttribute("checked");
        $status{"todostatus$id"}->setAttribute(checked => "true");
        $xml_elem->setAttributeNS($XMLNS, lang => "en");
      }
    } else {
      $status{"okstatus$id"}->removeAttribute("checked");
      $status{"checkstatus$id"}->removeAttribute("checked");
      $status{"todostatus$id"}->setAttribute(checked => "true");
      $xml_elem->setAttributeNS($XMLNS, lang => "en");
    }
  }
  open XML, ">:utf8", "$lang/ui.xml" or _die("Cannot write XML output: $!.");
  print XML $xml_doc->toString();
  close XML;
  print $html_doc->toString();
}

sub read_xml
{
  my ($parser, $in) = @_;
  my $document;
  eval { $document = $parser->parse_file($in); };
  _die("Error parsing file $in: $@") if $@;
  return $document;
}

sub _die
{
  my $message = join "", @_;
  my (undef, undef, $line) = caller;
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
    <p>$line: $message</p>
  </body>
</html>
HTML
  exit;
}

sub normalize_space
{
  local $_ = shift;
  s/\s+/ /g;
  s/^ //;
  s/ $//;
  $_;
}
