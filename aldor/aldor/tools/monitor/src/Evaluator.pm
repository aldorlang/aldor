package Evaluator;

use strict;
use warnings FATAL => 'all';
use open ':utf8';

BEGIN {
   use utf8;
}

use DynaLoader;

our $VERSION = '0.05';
our @ISA = qw(DynaLoader);

bootstrap Evaluator;

our $BITS = bits ();


1
