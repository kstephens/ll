#!/usr/bin/env perl
#
# This progrnam reads GCC stab debugging info to
# create a type and symbol table to be read by cthc.c
# If a source file is given, the file will be compiled and then scanned.
#

$pid = $$ < 0 ? - $$ : $$;

use Carp;

use Data::Dumper;

sub pp {
  my ($x) = @_;

  print STDOUT Data::Dumper->Dump([ $x ], [qw($x)]);
}


sub tobj {
    my($cntx) = shift;
    my($tobj);

    $tobj = { @_ };

    if ( $tobj->{id} ) {
	$cntx->{tobjs}->{$tobj->{id}} = $tobj;
    }
    if ( $tobj->{name} ) {
	$cntx->{tobjs_}->{$tobj->{name}} = $tobj;
    }

    #print STDERR "tobj: ", join(" ", $tobj, @_), "\n";

    $tobj;
}

sub tobj_signature {
    my($tobj) = @_;
    my($sig);

    if ( ! ref($tobj) ) {
	$sig = $tobj;
    }
    elsif ( ! ($sig = $tobj->{signature}) ) {
	my($t) = $tobj->{type};
	if ( $t eq 'i' ) {
	    $sig = $tobj->{name} . ';';
	} 
	elsif ( $t eq 'p' ) {
	    $sig = 'p' . tobj_signature($tobj->{subtype});
	} 
	elsif ( $t =~ /^[ab]/ ) {
	    $sig = $t . $tobj->{size} . tobj_signature($tobj->{subtype});
	}
	elsif ( $t =~ /^[suef]/ ) {
	    $sig = $t;

	    $sig .= $obj->{name} . ':';

	    # Avoid recursion on struct foo { struct foo * next; };
	    $tobj->{signature} = $sig;

	    if ( $t eq f ) {
		$sig .= tobj_signature($tobj->{subtype}) . ':';
	    }
	    my($e);
	    foreach $e ( @{$tobj->{elems}} ) {
		$sig .= join(',', map(tobj_signature($_), @{$e})) . ':';
	    }

	    $sig .= ';';
	}
	else {
	    carp "Unknown type '$t' for {'" . join("', '", $tobj, %$tobj) . "'}";
	    return '';
	}

	$tobj->{signature} = $sig;
	#print STDERR "sig: ['", join("' '", $tobj, %$tobj), "']\n";
    }

    $sig;
}

sub unique_type {
    my($cntx,$tobj) = @_;
    my($sig);
    my($g_tobj);

    if ( ! ref($tobj) ) {
	$g_tobj = $tobj;
    } else {
	$sig = tobj_signature($tobj);
	if ( ! ($g_tobj = $cntx->{global}->{tobjs_sig}->{$sig}) ) {
	    # Remember it by it's signature
	    $cntx->{global}->{tobjs_sig}->{$sig} = $tobj;

	    $g_tobj = $tobj;

	    # Twizzle interior type references first

	    $tobj->{id} = -1;

	    if ( $tobj->{type} =~ /^[su]/ ) {
		# Give it a new id
		$tobj->{id} = scalar @{$cntx->{global}->{tobjs}};
		push(@{$cntx->{global}->{tobjs}}, $tobj);
	    }

	    if ( $tobj->{subtype} ) {
		$tobj->{subtype} = unique_type($cntx, $tobj->{subtype});
	    }
	    if ( $tobj->{elems} ) {
		my($e);
		foreach $e ( @{$tobj->{elems}} ) {
		    $e->[0] = unique_type($cntx, $e->[0]);
		}
	    }

	    # Give it a new id
	    if ( $tobj->{id} == -1 ) {
		$tobj->{id} = scalar @{$cntx->{global}->{tobjs}};
		push(@{$cntx->{global}->{tobjs}}, $tobj);
	    }
	}
    }

    $g_tobj;
}

sub uniquify_types {
    my($cntx) = @_;

    my($id);
    my($x, $y);

    # Add function types to ftobjs.
    $id = 0;
    foreach $x ( @{$cntx->{ftobjs}} ) {
	$cntx->{tobjs}->{$x->{id} = -- $id} = $x;
    }
    @{$cntx->{ftobjs}} = ();

    # Uniquify types.
    foreach $id ( sort keys %{$cntx->{tobjs}} ) {
	$x = $cntx->{tobjs}->{$id};
	#print STDERR "unique_type($x, $id)\n"; $id ++;
	unique_type($cntx, $x);
    }
    %{$cntx->{tobjs}} = ();
    %{$cntx->{tobjs_}} = ();

    # Fix typedef types and save.
    T: foreach $x ( @{$cntx->{typedefs}} ) {
	$x->{type} = unique_type($cntx, $x->{type});

	if ( ($y = $cntx->{global}->{typedefs_n}->{$x->{name}}) ) {
	    if ( $x->{type} eq $y->{type} ) {
		next T;
	    }
	    if ( $x->{file} eq $y->{file} ) {
		die "typedef $name occurs more than once in $file";
	    }
	}

	$cntx->{global}->{typedefs_n}->{$x->{name}} = $x;
	push(@{$cntx->{global}->{typedefs}}, $x);
    }
    @{$cntx->{typedefs}} = ();

    # Fix symbol types.
    foreach $x ( @{$cntx->{syms}} ) {
	$x->{type} = unique_type($cntx, $x->{type});
	if ( $cntx->{global}->{syms_n}->{$x->{name}} ) {
	    $x->{name} .= ":" . $x->{file};
	}
	push(@{$cntx->{global}->{syms}}, $x);
	$cntx->{global}->{syms_n}->{$x->{name}} = $x;
    }
    @{$cntx->{syms}} = ();
}


###########################################################
# DWARF support.
#

sub parse_dwarf_elem {
  my($cntx, $in) = @_;
  my(%h, $got);
  
  while ( defined($_ = shift @$in) ) {
    if ( /<(\d+)><(\d+)>:\s*Abbrev Number:\s*(\d+)\s*[(]DW_TAG_([^\)]+)[)]/ ) {
      $got = 1;
      ($h{'id'}, $h{'tag'}) = ($2, $4);
      
      while ( defined($_ = shift @$in) ) {
	if ( /<(\d+)>\s*DW_AT_([^\)]+)[)]\s*:\s*(.*)/ ) {
	  $h{$2} = $3;
	} else {
	  unshift @$in, $_;
	  last;
	}
      }
      last;
    }
  }

  $got ? \%h : undef;
}


sub parse_dwarf_elems {
  my($cntx, $in) = @_;
  my($tag, @elems);

  while ( defined($_ = shift @$in) ) {
    $last_line = $_;
    push(@elems, $_);
  }
  ($tag, \@elems);
}


sub parse_dwarf {
  my($cntx, $in) = @_;
  my(@elems);

  pp($in);

  while ( $_ = parse_dwarf_elem($cntx, @$in) ) {
    push(@elems, $_);
  }

  pp(\@elems);

  \@elems;
}


###########################################################################

# Currently unused

$stab_type_2_code_defined = 0;
%stab_type_2_code = ();
%stab_code_2_type = ();

sub stab_type_2_code { 
    
    return if ( $stab_type_2_code_defined );

    $stab_type_2_code_defined ++;
    
    $file = '/usr/include/stab.def';
    open(F, "<$file") || die "Cannot open $file: $!";
    my($l);
    while ( $l = <F> ) {
	#print STDERR "$file: $l";
	if ( $l =~ /^\s*__define_stab\s*[(]\s*N_(\w+)\s*,\s*(\w+)\s*,\s*"([^"]*)"/ ) {
           my($type, $code) = ($1, $2);
           $code = oct($code) if ( $code =~ /^0/ );
           #print STDERR "stab $type $code\n";
           $stab_type_2_code{$type} = $code;
           $stab_code_2_type{$code} = $type;
        }
    }
    close(F);
}


############################################################################

sub parse_stabs_1 {
    my($cntx,$name,$id,$typedef) = @_;
    my($tobj);

    my($type) = 0;

    #m/^(\S+)/; print STDERR "parse_stabs_1: '$name' '$id' '$1'\n";

    if ( $name eq 'void' ) {
	s/^([(]\d+,\d+[)]|\d+)=([(]\d+,\d+[)]|\d+)//;
	die "$_" if ( $1 ne $2 );
	$id = $1;
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx,
		     type, i,
		     id, $id, 
		     name, $name);
    }
    elsif ( s/^([(]\d+,\d+[)]|\d+)=// ) {
	$id = $1;
	die "id not defined" if ( ! $id );
	$tobj = parse_stabs_1($cntx, $name, $id);
	$cntx->{tobjs}->{$id} = $tobj;
	if ( $typedef ) {
	    #print STDERR "$id => $tobj->{id}\n";
	    $cntx->{global}->{tobjs_}->{$id} = $tobj;
	}
    }
    elsif ( s/^([(]\d+,\d+[)]|\d+)// ) {
	$id = $1;
	if ( $id eq '(3,2)' ) { # egcs bug??
	    $id = '(2,2)';
	}
	die "id not defined" if ( ! $id );
	$tobj = $cntx->{tobjs}->{$id};
	$tobj = $cntx->{global}->{tobjs_}->{$id} if ( ! $tobj );
	if ( ! $tobj ) {
	    warn "Type $id undefined in '$last_line'";
	    $tobj = $cntx->{tobjs_}->{"int"};
	    $cntx->{global}->{tobjs_}->{$id} = $tobj;
        }
    }
    elsif ( s/^r([(]\d+,\d+[)]|\d+);([^;]+);([^;]+);// ) {
	# Intrinsic type
	my($rid, $min, $max) = ($1, $2, $3);
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx, 
		     type, i, 
		     id, $id, 
		     name, $name, 
		     rid, $rid, 
		     min, $min, 
		     max, $max);
    }
    elsif ( s/^([su])(\d+)// ) {
	# Struct/Union
	$tobj = $cntx->{tobjs}->{$id};
	if ( ! $tobj ) {
	    die "id not defined" if ( ! $id );
	    $tobj = tobj($cntx,
			 type, $1, 
			 name, $name,
			 id, $id,
			 size, $2,
			 elems, [ ]);
	}


	while ( ! s/^;// ) {
	    s/^([^:]+)://;
	    my($ename) = $1;
	    my($etype) = parse_stabs_1($cntx);
	    my($eoff, $esize);
	    if ( s/^,(-?\d+),(-?\d+);// ) {
		($eoff, $esize) = ($1, $2);
	    }

	    # Non-byte aligned elements are bitfields!
	    if ( ($eoff & 7) || ($esize & 7) ) {
		$etype = tobj($cntx,
			      type, b,
			      subtype, $etype,
			      size, $esize);

		push(@{$cntx->{ftobjs}}, $etype);
	    }

	    # elems are [ $etype, $ename, $off, $size ]
	    my($elem) = [ $etype, $ename, $eoff, $esize ];
	    push(@{$tobj->{elems}}, $elem);
	}
    }
    elsif ( s/^e// ) {
	# Enum
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx,
		     type, e, 
		     id, $id, 
		     name, $name, 
		     elems, [ ]);

	# elems are [ $evalue, $ename ]
	while ( ! s/^;// ) {
	    if ( s/^([^:]*):(-?\d+),// ) {
		push(@{$tobj->{elems}}, [ $2, $1 ]);
	    }
	}
     }
    elsif ( s/^\*// ) {
	# Pointer
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx,
		     type, p, 
		     id, $id, 
		     subtype, parse_stabs_1($cntx));
    }
    elsif ( s/^ar// ) {
	# Array
	my($rid) = parse_stabs_1($cntx);
	my($min, $max);
	if ( s/^;(-?\d+);(-?\d+);// ) {
	    ($min, $max) = ($1, $2);
	}
	$etype = parse_stabs_1($cntx);

	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx, 
		     type, a, 
		     id, $id, 
		     subtype, $etype,
		     rid, $rid,
		     min, $min,
		     max, $max,
		     size, ($max - $min + 1));
    }
    elsif ( s/^f// ) {
	# Function
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx, 
		     type, f, 
		     id, $id, 
		     subtype, parse_stabs_1($cntx),
		     elems, [ ] );
    }
    elsif ( s/^x([sue])([^:]+):// ) {
	# Forward type ref
	die "id not defined" if ( ! $id );
	$tobj = tobj($cntx,
		     type, $1,
		     id,   $id,
		     name, $2,
		     size, 0,
		     elems, [ ]);
    }

    else {
	die "id not defined" if ( ! $id );
	$tobj = $cntx->{tobjs}->[$id];
	if ( ! $tobj ) {
	    m/^(\S+)/;
	    die "parse_stabs_1: Cannot parse '$1'";
	}
    }

    $tobj;
}


sub parse_stabs {
    my($cntx, $in) = @_;
    my($sym, $tobj, $ftobj);
    my($file) = '';

    while ( defined($_ = shift @$in) ){
	$last_line = $_;
	while ( $_ ne '' ) {
	    s/^\s+//;
	    
	    #m/^(\S+)/; print STDERR "parse_stabs: [$type] $1\n";

	    if ( s/^@([^@]+)@// ) {
		$type = $1;
	    } else {
		if ( $type =~ /^(LSYM),/ ) {
		    #m/^(\S+)/; print STDERR "parse_stabs: [$type] $1\n";
		    if ( s/^([^:]*):([A-Za-z]?)// ) {
			my($name, $typedef) = ($1, $2);
			
			$tobj = parse_stabs_1($cntx, $name, undef, $typedef eq 't');
			
			if ( $name ne '' 
			     && $tobj->{id}
			     && $typedef eq 't' ) {
			    my($tdef);
			    
			    $tdef = {
				type => $tobj,
				name => $name,
				file => $file
				};
			    
			    #print STDERR "$name => $tobj->{id}\n";
			    $cntx->{global}->{tobjs_}->{$tobj->{id}} = $tobj;

			    push(@{$cntx->{typedefs}}, $tdef);
			}
		    } else {
			s/^(.*)//;
			print STDERR "Eating $type '$1'\n";
		    }
		}
		
		elsif ( $type =~ /^(FUN),(\d+),(\d+),(.*)/ ) {
		    my($lineno, $addr) = ($3, $4);
		    if ( s/^([^:]*):([fFS])// ) {
			my($name, $scope) = ($1, $2);
			if ( $scope eq 'S' ) {
			    $scope = lc($scope);
			    $tobj = parse_stabs_1($cntx);

			    # data symbol
			    $sym = {
				scope => $scope, 
				name  => $name,
				addr  => $addr,
				type  => $tobj, 
				file  => $file, 
				line  => $lineno
				};
			    
			    push(@{$cntx->{syms}}, $sym);
			} else {
			    $scope = $scope eq 'f' ? 's' : 'e';
			
			
			    $ftobj = tobj($cntx,
					  type, f, 
					  subtype, parse_stabs_1($cntx),
					  elems, []);
			    
			    push(@{$cntx->{ftobjs}}, $ftobj);
			    
			    # Function symbol
			    $sym = {
				scope => $scope, 
				name  => $name,
				addr  => $addr,
				type  => $ftobj, 
				file  => $file, 
				line  => $lineno
				};
			    
			    push(@{$cntx->{syms}}, $sym);
			}
		    } else {
			s/^(.*)//;
			print STDERR "Eating $type '$1'\n";
		    }
		    
		}
		elsif ( $type =~ /^(PSYM),(\d+),(\d+),(.*)/ ) {
		    my($lineno, $fpoffset) = ($3, $4);
		    if ( s/^([^:]*):(p)// ) {
			my($name) = $1;
			push(@{$ftobj->{elems}}, [ parse_stabs_1($cntx), $name ] );
		    } else {
			s/^(.*)//;
			print STDERR "Eating $type '$1'\n";
		    }
		}
		elsif ( $type =~ /^(GSYM|STSYM|LCSYM|RSYM),(\d+),(\d+),(.*)/ ) {
		    my($lineno, $addr) = ($3, $4);
		    if ( s/^([^:]*):([a-zA-Z])// ) {
			my($name, $scope) = ($1, $2);
			$ftobj = parse_stabs_1($cntx);
			
			if ( $scope =~ /^[GS]$/ ) {
			    $scope = $scope eq 'S' ? 's' : 'e';
			    # Data symbol
			    $sym = {
				scope => $scope, 
				name  => $name,
				addr  => $addr,
				type  => $ftobj,
				file  => $file, 
				line  => $lineno
				};
			    
			    push(@{$cntx->{syms}}, $sym);
			}
		    } else {
			s/^(.*)//;
			print STDERR "Eating $type '$1'\n";
		    }
		}
		
		elsif ( $type =~ /^SO,/ ) {
		    s/^(.+)//;
		    my($path) = $1;
		    if ( $file =~ m@/$@ ) {
			$file .= $path;
			print STDERR "$0: processing $file\n";
			uniquify_types($cntx);
		    } else {
			$file = $path;
		    }
		}
		
		
		else {
		    s/^([^@]+)//;
		    #print STDERR "Eating $type '$1'\n";
		}
	    }
	}
    }

    $tobj;
}


sub read_lines {
    my($tmp, $lines) = @_;

    open(TMP, "<$tmp") || return "Cannot read '$tmp': $!";
    $lines = [] if ( ! $lines );
    my($l);
    while ( defined($l = <TMP>) ) {
	#print STDERR "read_lines: $l";
	chop $l;
	$l =~ s/\r$//;
	push(@$lines, $l);
    }
    close(TMP);

    $lines;
}


sub extract_dwarf_objdump {
    my($file) = shift @_;

    # Run cc -S on input file
    my($tmp, $cmd);

    $tmp = "/tmp/ctpc$pid";
    die "$tmp exists" if ( -e $tmp );

    if ( ! -r $file ) {
	$file .= ".exe" if ( -r "$file.exe" );
    }

    $cmd = "objdump --dwarf @_ \"$file\" > \"$tmp\"";
    print STDERR "$0: $cmd\n";
    system($cmd) && die "Cannot run $cmd: $!";
    
    my($in) = read_lines($tmp);

    $in;
}

sub extract_stabs_objdump {
    my($file) = shift @_;

    # Run cc -S on input file
    my($tmp, $cmd);

    $tmp = "/tmp/ctpc$pid";
    die "$tmp exists" if ( -e $tmp );

    if ( ! -r $file ) {
	$file .= ".exe" if ( -r "$file.exe" );
    }

    $cmd = "objdump --stabs @_ \"$file\" > \"$tmp\"";
    print STDERR "$0: $cmd\n";
    system($cmd) && die "Cannot run $cmd: $!";
    
    my($in) = read_lines($tmp);

    unlink($tmp);

    my($out) = [];    
    my($type) = 0;
    
    my($l);
    my($x) = '';
    while ( defined ($l = shift @$in) ) {
	#print STDERR "line : $l\n";
	#               sn    n_type  n_othr  n_desc  n_value          n_strx String
	if ( $l =~ /^\s*\d+\s+(\w+)\s+(\d+)\s+(\d+)\s+([a-fA-f0-9]+)\s+\d+\s+(.*)$/ ) {
	    #print STDERR "line matched: $l\n";
	    my($n_type, $n_othr, $n_desc, $n_value, $str) = ($1, $2, $3, $4, $5);
	    my($t);
	    
	    $t = "$n_type,$n_othr,$n_desc,$n_value";
	    if ( $type ne $n_type ) {
		$type = $n_type;
		push(@$out, '@' . $t . '@');
	    }

	    if ( $str =~ s/\\$// ) {
		$x .= $str;
	    } else {
		$x .= $str;
		push(@$out, $x);
		$x = '';
	    }
        }
    }

    if ( $x ne '' ) {
	push(@$out, $x);
    }

    #print STDERR "stabs = { $out }\n";

    $out;
}

sub extract_stabs_cc {
    my($file) = shift;

    # Run cc -S on input file
    my($tmp);
    my($cmd);

    $tmp = "/tmp/ctpc$pid.o";
    die "$tmp exists" if ( -d $tmp );
    if ( $ENV{CTHC_CC_OPTS} ) {
	unshift(@_, split(/\s+/, $ENV{CTHC_CC_OPTS}));
    }

    $cmd = "cc @_ -g -s -D__CTHC__ -dr \"$file\" -o $tmp";
    print STDERR "$0: $cmd\n";
    system($cmd) && die "Cannot run $cmd: $1";
    
    my($out) = extract_stabs_objdump($tmp);
    
    unlink($tmp);

    $out;
}

sub scan_file {
    my($cntx, $file, @opts) = @_;

    if ( ! $cntx ) {
	$cntx = {
	    tobjs => {},    # tobjs indexed by id
	    tobjs_ => {},   # tobjs indexed by name
	    typedefs => [], # typedefs
	    ftobjs => [],   # tobjs created by function argument type extraction
	    syms => [],     # symbols 
	    global => {     # global
		tobjs => [],       # tobjs by id, uniqued by tobj_signature
		tobjs_ => {},      # tobjs by global id
		tobjs_sig => {},   # tobjs by tobj_signature
		typedefs => [],    # typedefs
		typedefs_n => {},  # typedefs by name
		syms => [],        # symbols
		syms_n => {}       # symbols by name
	    }
	};
    }


    # Add in varargs type
    $cntx->{tobjs}->{0} = { 
	type => i,
	id => 0,
	name => '...'
	    };

    # Read stabs
    my($in);
    if ( $file =~ /\.[chC]/ && ! -B $file ) {
      $in = extract_dwarf_cc($file, @opts);
      # $in = extract_stabs_cc($file, @opts);
    } else {
      $in = extract_dwarf_objdump($file, @opts);
      # $in = extract_stabs_objdump($file, @opts);
    }

    # Parse stabs
    parse_dwarf($cntx, $in);
    # parse_stabs($cntx, $in);

    # Uniquify remaining types
    uniquify_types($cntx);

    $cntx;
}

sub print_table {
    my($cntx, $output, $in) = @_;

    ###################################################################
    #
    # Print table
    #

    open(OUT, ">$output") || die "Cannot open $output for write: $!";

    # Uniquify remaining types
    uniquify_types($cntx);

    my($c);
    $c = $cntx->{global};

    print OUT "V 1.01\n";
    print OUT "D ", time(), "\n";
    print OUT "F \"$in\"\n";

    print OUT "T ", scalar @{$c->{tobjs}}, "\n";
    my $x;
    foreach $x ( @{$c->{tobjs}} ) {
	my($t) = $x->{type};
	print OUT $x->{id}, " ", $t, " ";
	if ( $t =~ /^[i]$/ ) {
	    print OUT '"', $x->{name}, "\"\n";
	}
	elsif ( $t =~ /^[su]$/ ) {
	    print OUT scalar @{$x->{elems}}, ' "', $x->{name}, "\" ", $x->{size}, "\n";
	    my $e;
	    foreach $e ( @{$x->{elems}} ) {
		print OUT "  ", $e->[0]->{id}, " \"", $e->[1], "\" ", $e->[2], " ", $e->[3], "\n";
	    }
	}
	elsif ( $t =~ /^[e]$/ ) {
	    print OUT scalar @{$x->{elems}}, ' "', $x->{name}, "\"\n";
	    my $e;
	    foreach $e ( @{$x->{elems}} ) {
		print OUT "  ", $e->[0], " \"", $e->[1], "\"\n";
	    }
	}
	elsif ( $t =~ /^[f]$/ ) {
	    print OUT scalar @{$x->{elems}}, ' ', $x->{subtype}->{id}, "\n";
	    my $e;
	    foreach $e ( @{$x->{elems}} ) {
		print OUT "  ", $e->[0]->{id}, " ", $e->[1], "\n";
	    }
	}
        elsif ( $t eq 'p' ) {
	    print OUT $x->{subtype}->{id}, "\n";
	}
	elsif ( $t =~ /^[ab]/ ) {
	    print OUT $x->{subtype}->{id}, " ", $x->{size}, "\n";
	}
    }

    print OUT "D ", scalar @{$c->{typedefs}}, "\n";
    foreach $x ( @{$c->{typedefs}} ) {
	print OUT $x->{type}->{id}, " \"", $x->{name}, "\" \"", $x->{file}, "\"\n";
    }

    print OUT "S ", scalar @{$c->{syms}}, "\n";
    foreach $x ( @{$c->{syms}} ) {
	print OUT join(" ", 
		   $x->{scope}, 
		   $x->{type}->{id}, 
		   '"' . $x->{name} . '"',
		   $x->{addr}, 
		   '"' . $x->{file} . '"',
		   $x->{line}), "\n";
    }

    print OUT "E\n";

    close(OUT);

    1;
}

sub cthc {
    my @opts = ();
    my @files = ();
    my $output = '-';

    if ( $ENV{CTHC_OPTS} ) {
	unshift(@opts, split(/\s+/, $ENV{CTHC_OPTS}));
    }

    while ( defined($_ = shift @_) ) {
	if ( /^-/ ) {
	    if ( /^-o/ ) {
		$output = shift @_;
	    } else {
		push(@opts, $_);
	    }
	} else {
	    push(@files, $_);
	}
    }

    my($cntx);
    
    $cntx = 0;
    foreach $_ ( @files ) {
	$cntx = scan_file($cntx, $_, @opts);
    }

    print_table($cntx, $output, join("\t", @files));

    printf STDERR "$0: done.\n";
}


cthc(@ARGV);

1;





