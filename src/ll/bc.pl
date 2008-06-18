
$isns = [
  { name => "lit",       nargs => 1, body => '%S(LIT(%1));' },
  { name => "arg",       nargs => 1, body => '%S(ARG(%1));' },
  { name => "set_arg",   nargs => 1, body => 'ARG(%1) = %R;' },
  { name => "save",      nargs => 1, body => 'SAVE(%1);' },
  { name => "set_loc",   nargs => 1, body => 'LOC(%1) = %R;' },
  { name => "loc",       nargs => 1, body => '%S(LOC(%1));' },
  { name => "glo",       nargs => 1, body => '%S(GLO(%1));' },
  { name => "set_glo",   nargs => 1, body => 'GLO(%1) = %R;' },
  { name => "call",      nargs => 1, body => '%S(CALL(%1));' },
  { name => "tail_call", nargs => 1, body => 'TAIL_CALL(%1);' },
  { name => "rtn",       nargs => 0, body => 'return %R; ' }
];

$op_imm = 8;

$bc = 1;
sub p_bc {
  my($i,$suf) = @_;
  print "  bc_$i->{name}$suf = $bc,\n";
  $bc ++;
}

print "enum bc {\n";
foreach $i ( @$isns ) {
  if ( $i->{nargs} ) {
    p_bc($i);
    $bc --;
    $i->{imm} = $op_imm if ( ! defined($i->{imm}) );
    foreach $j ( 0..$i->{imm} ) {
      p_bc($i, "_$j");
    }
    p_bc($i, "_B1");
    p_bc($i, "_B2");
    p_bc($i, "_B3");
    p_bc($i, "_B4");
  } else {
    p_bc($i);
  }
  print "\n";
}
print "}\n";

print q@

#ifdef USE_METER
unsigned char this_op, prev_op;
extern unsigned long meter[256][256];
#define METER(OP) (prev_op = this_op, this_op = OP, meter[0][this_op] ++, meter[prev_op][this_op] ++, this_op)
#else
#define METER(OP) OP
#endif

again:
switch ( METER(FETCH()) ) {
@;

sub p_body {
  my($i,$sub,$q,$pre) = @_;
  my($x);
  
  $x = $i->{body};
  $x =~ s/%1/$sub/g;
  $x =~ s/%([0-9]+)/oparg$1/g;
  $x =~ s/%S/PUSH/g;
  $x =~ s/%R/POP()/g;
  foreach $j ( 2..$i->{nargs} ) {
    $pre .= "  int oparg$j = FETCH4();\n"
  }
  print "  case bc_$i->{name}$q:\n";
  print "  {\n  $pre\n" if ( $pre );
  print "  $x\n";
  print "  }\n" if ( $pre );
  print "  goto again;\n\n";
}


foreach $i ( @$isns ) {
  print "\n  /* $i->{name} */\n";
  if ( $i->{nargs} ) {
    foreach $j ( 0..$i->{imm} ) {
      p_body($i, $j, "_$j");
    }
    p_body($i, "oparg1", "_B1", "int oparg1 = FETCH1();");
    p_body($i, "oparg1", "_B2", "int oparg1 = FETCH2();");
    p_body($i, "oparg1", "_B3", "int oparg1 = FETCH3();");
    p_body($i, "oparg1", "_B4", "int oparg1 = FETCH4();");
  } else {
    p_body($i, "");
  }
}

print q@
  default: abort();
}
@;


1;


