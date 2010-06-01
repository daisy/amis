use File::BOM qw(defuse);
require Encode;
use Unicode::Normalize;

sub encode2 {
	my ($infilepath, $outfilepath) = @_;
	printf(STDERR "in: %s, out: %s\n", $infilepath, $outfilepath);
	
	open (DATA, "<:raw:encoding(UTF-8)", $infilepath) || die("oh no!!");
  
  my @lines = <DATA>;
  printf(STDERR "# of lines: %d\n", scalar(@lines));

  open $out, ">:raw:encoding(UTF-16LE):crlf:via(File::BOM)", $outfilepath;
  #open $out, ">:raw:encoding(UTF-8):cr:via(File::BOM)", $outfilepath;
	foreach(@lines) {
    my $line = $_;

		# newline = 0085
		# cr = 0d
		# lf = 0a
		
		$line=~s/\x{0085}//g;
		#doesn't work...
		#$line=~s/\x0d\x0a//g;
    print $out $line
  }
  #print $out $data;
	close(DATA);
	close($out);
}


print &encode2("in", "out");

