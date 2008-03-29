#!/usr/bin/perl

use strict;
use warnings;

use DBI;

my $dbn = "DBI:mysql:database=a86978_amisdb";
my $user = "a86978admin";
my $pwd = "crazydaisyamisdb";
my $dbh = DBI->connect($dbn, $user, $pwd);

print "Login:    ";
chomp(my $login = <>);
print "Password: ";
chomp(my $passwd = <>);

my $a = $dbh->selectall_arrayref(qq{
  SELECT password,realname FROM users WHERE username='$login'
});
if (@$a == 1 && $a->[0]->[0] eq $passwd) {
  print "Welcome, $a->[0]->[1]!\n";
} else {
  print "Close, but no cigar.\n";
}

$dbh->disconnect();
