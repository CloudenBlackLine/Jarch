<
 * Clouden Blackline Source Code
 * Author: Judah
 * Project: JArch
>



~bss

.bss_1111[#3];
.bss_55[#6];
.bss_123[#2];


~const

.const_value1 = [#ff, #55];
.const_value2 = #55;



~text


_start:
{
    $5 = #4;
    .data_1111 = $9;
    $0 = #89;
    !exit;
}


exit:
{
    $1 = .tp;
    .sp = $1;
    $0 = #89;
    !push $sp;
    !halt;
}

labels:
    !input;
    $8 = #89;


~data

.data_1111 = [#789, #1];
.data_444 = [#555, #78, #78, #8999];

.data_end = #90;
