
~const

.test1 = #1;
.test2 = #22;
.test3 = #333;
.sneak = #0;
.test4 = #4444;
.test5 = #55555;
.sneak = #5;
.test6 = [#1,#2,#0];
.test7 = [#11,#22,#33];


~text

_start:
{
    $sp = .test1;

    $8 = #f;

    .test2 = $sp;
    $2 = .test6[#1];
    ! _start;
}
