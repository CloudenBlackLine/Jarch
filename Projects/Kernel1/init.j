	~data
.stack_pointer = #3800;
.jmp_start = #100;

	~text
@00:init:
{
	$sp = .stack_pointer;
	!.jmp_start;
}
