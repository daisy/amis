#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use CGI::Session;

my $query = new CGI();

if (my $cookie = $query->cookie(-name => "session")) {
  CGI::Session->name($cookie);
  my $session = new CGI::Session("driver:File", $cookie,
    { Directory => "/tmp/persistent/amis/sessions/" });
  print "Content-type: text/plain\n\n";
  print "Hello, ", $session->param("realname"), "!\n";
  print "Your language is: ", $session->param("language"), ".\n";
} else {
  print "Location: /amis/login.html\n\n";
}
