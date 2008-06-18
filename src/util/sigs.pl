#!/bin/perl
# $Id: sigs.pl,v 1.4 2000/01/04 05:53:51 stephensk Exp $
print "/* DO NOT MODIFY: Created by perl $0 @ARGV */\n";
while ( <> ) {
#print;
    if ( m@^\s*#define\s+SIG([A-Z0-9]+)\s+(\S+)(\s+/\*\s*(.*)\s*(\*/))?@ ) {
	 $name = $1;
	 $sig = $2;
	 $comment = $4;
	 $comment =~ s/^\s+//;
	 $comment =~ s/\s+$//;
	 if ( $sig !~ /[()]/ && $comment ne '' ) {
	     print "#ifdef SIG$name\n";
	     print "signal_def($name,SIG$name,\"$comment\") /* $sig */\n";
	     print "#endif\n";
	 }
     }
}
print "#undef signal_def\n";

1;
