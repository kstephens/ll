while ( <> ) {
    chop;
    if ( /^#/ ) {
	 print "/* ", $_, "*/\n";
     } else {
	 print $_, "\n";
     }
}
