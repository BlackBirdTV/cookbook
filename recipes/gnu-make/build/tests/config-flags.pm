# This is a -*-perl-*- script
#
# Set variables that were defined by configure, in case we need them
# during the tests.

%CONFIG_FLAGS = (
    AM_LDFLAGS   => '-Wl,--export-dynamic',
    AR           => 'x86_64-unknown-redox-gcc-ar',
    CC           => 'x86_64-unknown-redox-gcc',
    CFLAGS       => '-DPOSIX -DNO_ARCHIVES -DNO_OUTPUT_SYNC',
    CPP          => 'x86_64-unknown-redox-gcc -E',
    CPPFLAGS     => '',
    GUILE_CFLAGS => '',
    GUILE_LIBS   => '',
    LDFLAGS      => '-static',
    LIBS         => ''
);

1;
