# $Id: fixpound.pl,v 1.2 1999/02/19 09:26:01 stephensk Exp $
while ( <> ) {
    chop;
    if ( /^#/ ) {
	 print "/* ", $_, "*/\n";
     } else {
	 print $_, "\n";
     }
}
