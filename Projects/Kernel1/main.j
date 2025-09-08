	~data
	
.tasks_buffer = #3600;

	~text

@100:start:
{
	!:setup_task;

	!:loop;
}


:loop: !:loop;


:setup_task:
{
	!push $lr;

	$0 = .tasks_buffer;
		

	!pop $lr;
	!ret;
}