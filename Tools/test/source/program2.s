    ~data
.tp = #3000;
.sp = #4000;
.lp = #5000;

    ~text
init:
{
    $sp = .sp;
    !_start;
}

add_task:
{
    .sp = $sp;
    $sp = .lp;
    !push $lr;
    .lp = $sp;
    $sp = .tp;

    !push $1;
    
    .tp = $sp;
    $sp = .lp;
    !pop $lr;
    .lp = $sp;
    $sp = .sp;
    !ret;
}


remove_task:
{
    .sp = $sp;
    $sp = .lp;
    !push $lr;
    .lp = $sp;
    $sp = .tp;

    !pop $1;
    
    .tp = $sp;
    $sp = .lp;
    !pop $lr;
    .lp = $sp;
    $sp = .sp;
    !ret;
}




input:
{
    
    .sp = $sp;
    $sp = .lp;
    !push $lr;
    .lp = $sp;
    $sp = .sp;

    !program;

    .sp = $sp;
    $sp = .lp;
    !push $lr;
    .lp = $sp;
    $sp = .sp;
    !ret;
}