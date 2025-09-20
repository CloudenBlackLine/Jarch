	~init

.databa:	#3000;

	~data

.input = #7000;
.output = #7008;

.sp = #2400;
.tp = #2200;
.lrp = #2000;

.string_offset = #2500;
.string_base = #2500;
.string_count = #24f8;

.command_list_count = #29f8;
.command_list_base = #2a00;
.command_list_offset = #2a00;

.file_base = #4000;

.copy_buffer = #0;

	~text

@00:start:
{


	$sp = .sp;

	$0 = #74697865;
	$1 = @:exit_program;
	!:_add_program;

	$0 = #79706f63;
	$1 = @:copy_program;
	!:_add_program;
	
	$0 = #65747370;
	$1 = @:pste_program;
	!:_add_program;

	$0 = #6e65706f;
	$1 = @:open_program;
	!:_add_program;

	:start_loop1:
	{
		$0 = @:command_shell_program;
		!:add_task;
		!:run_tasks;
	}!:start_loop1;
}:loop: !halt;



:_add_program:
{
	$2 = .command_list_offset;
	*$2 = $0;
	+$2 = $2, #8;
	*$2 = $1;
	+$2 = $2, #8;
	.command_list_offset = $2;
	$2 = *.command_list_count;
	+$2 = $2, #1;
	*.command_list_count = $2;
	!ret;	
}


:add_task:
{
	.sp = $sp;
	$sp = .tp;
	!push $0;
	.tp = $sp;
	$sp = .sp;
	!ret;
}


:remove_task:
{
	.sp = $sp;
	$sp = .tp;
	*$sp = $0;
	*$sp = #0;
	+$sp = $sp, #8;
	.tp = $sp;
	$sp = .sp;
	!ret;
}


:run_tasks:
{
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .tp;
	$1 = #0;


	:run_tasks_loop1:
	{
		!pop $0;
		!:remove_task;
		!($1 == $0):run_tasks_loop1_done;
		.tp = $sp;
		$sp = .sp;
		!$0;
		$sp = .tp;
	}!:run_tasks_loop1;:run_tasks_loop1_done:
	.tp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;
}




:copy_program:
{
	
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;
	$0 = *.string_offset;
	$1 = #0;
	*.string_offset = $1;
	.copy_buffer = $0;
	.tp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;
}



:pste_program:
{
	
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;
	

	


	.tp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;
}


:open_program:
{
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;
	
	$0 = *.string_offset;
	*800 = $0;
	$1 = #0;
	*.string_offset = $1;


	
	

	.sp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;	
}



:test_program:
{
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;
	
	$0 = #a;
	*.ouput = $0;
	$0 = #61;
	*.output = $0;
	$0 = #62;
	*.output = $0;
	$0 = #63;
	*.output = $0;


	.sp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;	
}



:exit_program:
{
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;
	
	!halt;

	.sp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;
}


:command_shell_program:
{
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;

	!:display_cmd;
	!:get_input;
	!:plu;

	.sp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;

	FUNCTIONS

	:plu:
	{

		.sp = $sp;
		$sp = .lrp;
		!push $lr;
		.lrp = $sp;
		$sp = .sp;
		!push $0;
		!push $1;	
		!push $2;	
		!push $3;
		!push $4;

	CANT FIGURE OUT YET HOW TO MAKE THE SEARCH DYNAMIC I GET SEGMENTATION


		$0 = .command_list_base;
		$1 = *$0;
		$3 = *.string_base;

		!($1 != $3):plu_else2;
		{
			$0 = @:exit_program;			
			!:add_task;
			!:plu_else3;
		}:plu_else2:
		+$0 = $0, #10;
		$1 = *$0;
		!($1 != $3):plu_else3;
		{
			$0 = @:open_program;			
			!:add_task;
			!:plu_else3;
		}:plu_else3:
		


		!pop $4;
		!pop $3;
		!pop $2;	
		!pop $1;	
		!pop $0;	
		.sp = $sp;
		$sp = .lrp;
		!pop $lr;
		.lrp = $sp;
		$sp = .sp;
		!ret;
	}

	:get_input:
	{
		.sp = $sp;
		$sp = .lrp;
		!push $lr;
		.lrp = $sp;
		$sp = .sp;
		!push $0;
		!push $1;
		!push $2;
		!push $3;
		!push $4;
		$2 = .string_base;
		.string_offset = $2;
	
		$2 = #0;
		$3 = #0;
		:get_input_loop1:
		{
			$0 = *.input;
			*.output = $0;
			$1 = #a;
			!($1 == $0):get_input_loop1_done;
			$1 = #20;
			!($1 != $0):get_input_else1;
			{
				*.string_offset = $4;
				$4 = .string_offset;
				+$4 = $4, #8;
				.string_offset = $4;
				$4 = *.string_count;
				+$4 = $4, #1;
				*.string_count = $4;
				$3 = #0;
				$4 = #0;
				!:get_input_loop1;
			}:get_input_else1:	
			<<$2 = $0, $3;
			|$4 = $4, $2;
			+$3 = $3, #8;
		}!:get_input_loop1;:get_input_loop1_done;
		*.string_offset = $4;
		!pop $4;
		!pop $3;
		!pop $2;
		!pop $1;
		!pop $0;
		.sp = $sp;
		$sp = .lrp;
		!pop $lr;
		.lrp = $sp;
		$sp = .sp;
		!ret;
	}

	:display_cmd:
	{
		!push $0;
		$0 = #3e;
		*.output = $0;
		$0 = #20;
		*.output = $0;
		!pop $0;
		!ret;
	}



}


:string_display:
{	
	.sp = $sp;
	$sp = .lrp;
	!push $lr;
	.lrp = $sp;
	$sp = .sp;


			

	
	.sp = $sp;
	$sp = .lrp;
	!pop $lr;
	.lrp = $sp;
	$sp = .sp;
	!ret;
}
