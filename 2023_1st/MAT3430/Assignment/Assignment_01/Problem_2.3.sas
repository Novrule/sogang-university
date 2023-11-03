data king;
input name $ age @@;
if _N_<=14 then war='Before';
else war='After';
agegroup=int(age/10)*10;
cards;
세조 73 정종 62 태종 45 세종 53 문종 38 단종 16 세조 51 예종 28 성종 37 연산군 30
중종 56 인종 30 명종 33 선조 56 광해군 66 인조 54 묘종 40 현종 33 숙종 59 경종 36
영조 82 정조 48 순조 44 헌종 22 철종 32 고종 67 순종 52
;
run;

ods graphics off;

proc univariate data=king plot;
class war;
var age;
run;

proc means data=king;
class war;
var age;
output out=mean_king;
run;

proc print data=mean_king;
run;

proc freq data=king;
tables war agegroup war*agegroup;
run;
