#!/usr/bin/perl

use strict;
use warnings;
use CGI;
use CGI::Session;
use DBI;

my $dbn = "DBI:mysql:database=a86978_amisdb";
my $user = "a86978admin";
my $pwd = "crazydaisyamisdb";
my $dbh = DBI->connect($dbn, $user, $pwd);

my $query = new CGI();

if ($query->param("login")) {
  my $session = new CGI::Session("driver:File", undef,
    { Directory => "/tmp/persistent/amis/sessions" });
  my $login = $query->param("login");
  $login =~ s/\s//g;
  my $passwd = $query->param("pwd");
  $passwd =~ s/\s//g;
  my $a = $dbh->selectall_arrayref(qq{
    SELECT password,realname, language FROM users WHERE username='$login'
  });
  if (@$a == 1 && $a->[0]->[0] eq $passwd) {
    $session->param("realname", $a->[0]->[1]);
    $session->param("language", $a->[0]->[2]);
    my $id = $session->id();
    my $host = $ENV{HTTP_HOST};
    print "Set-cookie: session=$id; domain=.$host; path=/\n";
    print "Location: /cgi-bin/hello.cgi\n\n";
  } else {
    print "Content-type: text/plain\n\n";
    print "Close, but no cigar.\n";
  }
} else {
  print "Location: /amis/login.html\n\n";
}

$dbh->disconnect();
