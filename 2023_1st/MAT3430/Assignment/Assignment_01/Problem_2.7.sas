data before_smoke;
input when $ heart @@;
cards;
before 72 before 70 before 68 before 67 before 73 before 71 before 72 before 70 before 69 before 70 before 68
before 72 before 69 before 66 before 73 before 71 before 70 before 72 before 70 before 69 before 72 before 73
;
run;

data after_smoke;
input when $ heart @@;
cards;
after 74 after 72 after 69 after 68 after 72 after 72 after 72 after 71 after 67 after 73 after 69
after 71 after 68 after 74 after 73 after 70 after 74 after 68 after 71 after 74 after 74 after 69
;
run;

proc freq data=before_smoke;
tables heart;
run;

proc univariate data=before_smoke noprint;
histogram heart;
run;

proc univariate data=before_smoke;
var heart;
run;

proc freq data=after_smoke;
tables heart;
run;

proc univariate data=after_smoke noprint;
histogram heart;
run;

proc univariate data=after_smoke;
var heart;
run;
