#!/usr/bin/perl

use Fcntl;
use File::Basename;
use File::Spec::Functions;


$DEBUG=1;
$INPUT_FILE=$ARGV[0];
$CRC_TAIL_SIZE=4;

if ($^O eq 'MSWin32' || $^O eq 'msys' || $^O eq 'cygwin') {
	$EXE_SUFFIX = ".exe";
} else {
	$EXE_SUFFIX = "_lnx";
}

$TARGET_SIZE=(128*1024-$CRC_TAIL_SIZE);

$CRC_TOOL_EXEC = catfile(dirname(__FILE__), "create_crc_res$EXE_SUFFIX");
debug_msg("CRC_TOOL_EXEC=$CRC_TOOL_EXEC");

debug_msg("INPUT_FILE=$INPUT_FILE");
sysopen(OUTBIN_REF,"$INPUT_FILE",O_BINARY|O_WRONLY|O_APPEND) or error_exit("Can't open [$INPUT_FILE] for further action; $!");

$INPUT_FILE_SIZE=-s "$INPUT_FILE";
debug_msg("INPUT_FILE_SIZE=$INPUT_FILE_SIZE.byte");

if($INPUT_FILE_SIZE > $TARGET_SIZE){
	error_exit("INPUT_FILE_SIZE[$INPUT_FILE_SIZE] overflow TARGET_SIZE[$TARGET_SIZE]");
}

#recorder and pring the append CRC code
#
$crc_exec_result=qx("$CRC_TOOL_EXEC" $INPUT_FILE);
	@split_result=split("\ +|\n|:",$crc_exec_result);
        debug_msg("APPEND_CRC_CODE=$split_result[19]");

#print out output file size
#
$OUTPUT_FILE_SIZE=-s "$INPUT_FILE";
debug_msg("OUTPUT_FILE_SIZE=$OUTPUT_FILE_SIZE.byte");
close OUTBIN_REF;

sub debug_msg{
	unless($DEBUG){return};
	local $DEBUG_MSG="DEBUG >>> :";
	print "$DEBUG_MSG @_";
	print "\n";
}
sub error_exit{
	local $ERROR_MSG="\nERROR:**";
	print "----------------------";
	print "$ERROR_MSG @_";
	print "\n";
	exit(1);
}
