	~data
.stack_pointer = #3800;

	~text
@100:jmp_start:
@00:init:
{
	$sp = .stack_pointer;
	!:jmp_main;
}
