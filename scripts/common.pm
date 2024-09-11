package perlModules::common;

use strict;
use warnings;

use Exporter qw(import);
our @EXPORT_OK = qw( LineStartMultiLineComment
                     LineStartSingleLineComment
                     LineStartCleanComments
                     LineStartParenthesis
                     LineStartTypeStar
                     LineStartVariable
                     LineStartDefault
                     LineStartSeparator
                   );


#############################################################################
# LineStartMultilineComment
#
# Returns the string from the "/*" if found in start of $line, blank string 
# if not found. Matches the smallest string between /* and */
# 
sub LineStartMultiLineComment { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(/\*[\w\W]*?\*/)##;
  return ($1, $line);
}

#############################################################################
# LineStartSingleLineComment
#
# Returns the string after the "//" to the first "\n" if found 
# in start of $line, blank string if not found.
# 
sub LineStartSingleLineComment { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*//\s*(.*)\s*\n##;
  #$line =~ s#^\s*//(.*)##;
  return ($1, $line);
}

#############################################################################
# LineStartCleanComments
#
# Throws away comments in begginning of $line
# 
sub LineStartCleanComments { # ($line) {
  my $line = $_[0];

  my $multi  = "foo";
  my $single = "foo";

  #print "LineStartCleanComments:\n";

  while ($multi or $single){
    ($multi, $line) = LineStartMultiLineComment($line);
    ($single, $line) = LineStartMultiLineComment($line);

    if ($single){
      #print "Single = $single\n";
    }
    if ($multi){
      #print "Multi = $multi\n";
    }
  }
  return $line;
}

#############################################################################
# LineStartParenthesis
#
# Returns the string after the "(" 
# in start of $line, blank string if not found.
# 
sub LineStartParenthesis { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*\((.*)##;
  return $1;
}

#############################################################################
# LineStartType
#
# Returns the datatype in start of $line, blank string if not found.
# 
sub LineStartType { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)##;
  return ($1, $line);
}

#############################################################################
# LineStartTypeStar
#
# Returns the string after the datatype and, * if present,
# in start of $line, blank string if not found.
# 
sub LineStartTypeStar { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(unsigned)?(\s*[a-zA-Z_]\w*[\s*\*]*)##;
  my $result1 = $1;
  my $result2 = $2;
  $result2 =~ s#\s##g;
  my $result;
  if ($result1 eq ""){
    $result = $result2;
  }
  else
  {
    $result = "$result1 $result2";
  }

  return ($result, $line);
}

#############################################################################
# LineStartVariable
#
# Returns the variable name 
# in start of $line, blank string if not found.
# 
sub LineStartVariable { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*([a-zA-Z_]\w*)##;
  return ($1, $line);
}

#############################################################################
# LineStartDefault
#
# Returns the string, after the variable
# in start of $line, that defines its default value, blank string if not found.
# 
sub LineStartDefault { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(=\s*-?\s*[\.\w]*)##;
  return ($1, $line);
}

#############################################################################
# LineStartSeparator
#
# Returns the string after the first "," or ")" found
# in start of $line, blank string if not found.
# 
sub LineStartSeparator { # ($line) {
  my $line = $_[0];

  $line =~ s#^\s*(,|\))##;
  return ($1, $line);
}






1;
