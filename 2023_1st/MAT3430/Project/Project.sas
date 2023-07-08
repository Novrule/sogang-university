data EPL;a
input team $ win draw loss gf ga gd points european $ lastticket nowticket salary @@;
cards;
mancity 28 5 5 94 33 61 89 yes 325 350 2297
arsenal 26 6 6 88 43 45 84 yes 891 927 2379
manu 23 6 9 58 43 15 75 yes 532 532 3305
newcastle 19 14 5 68 33 35 71 yes 370 610 985
liverpool 19 10 9 75 47 28 67 yes 685 685 2300
brighton 18 8 12 72 53 19 62 yes 545 545 679
villa 18 7 13 51 46 5 61 yes 370 531 1157
tottenham 18 6 14 70 63 7 60 no 807 807 1800
brentford 15 14 9 58 46 12 59 no 419 419 265
fullham 15 7 16 55 53 2 52 no 349 385 534
palace 11 12 15 40 49 -9 45 no 495 520 1050
chelsea 11 11 16 38 47 -9 44 no 750 750 2915
wolves 11 8 19 31 58 -27 41 no 449 510 740
westham 11 7 20 42 55 -13 40 no 320 299 1001
bournemouth 11 6 21 37 71 -34 39 no 450 550 378
nottingham 9 11 18 38 68 -30 38 no 365 385 424
everton 8 12 18 34 57 -23 36 no 420 565 1016
leicester 9 7 22 51 68 -17 34 no 365 385 1170
leeds 7 10 21 48 78 -30 31 no 349 384 357
southampton 6 7 25 36 73 -37 25 no 399 399 705
;
run;

proc univariate data=EPL mu0=600 cibasic alpha=0.05;
var nowticket;
run;

data EPL1;
set EPL;
if gd>0 then gdgroup='good';
else gdgroup='bad';
run;

proc freq data=EPL1 order=data;
tables gdgroup/binomial (p=0.3) alpha=0.05;
run;

proc ttest data=EPL order=data;
class european;
var nowticket;
run;

proc ttest data=EPL;
paired lastticket*nowticket;
run;

data EPL2;
set EPL;
if nowticket>500 then ntgroup='expensive';
else ntgroup='cheap';
run;

proc freq data=EPL2 order=data;
tables european*ntgroup/chisq fisher;
run;

data EPL3;
set EPL2;
if lastticket>500 then ltgroup='expensive';
else ltgroup='cheap';
run;

proc freq data=EPL3;
exact mcnem;
tables ltgroup*ntgroup;
run;

proc rank data=EPL out=prankEPL descending;
var points;
ranks pranking;
run;

proc rank data=prankEPL out=grankEPL descending;
var gf;
ranks granking;
run;

data EPL4;
set grankEPL;
if pranking<=5 then pgroup='a';
else if pranking<=10 then pgroup='b';
else if pranking<=15 then pgroup='c';
else pgroup='d';
if granking<=10 then gfgroup='high';
else gfgroup='low';
run;

proc freq data=EPL4;
exact fisher;
tables pgroup*gfgroup/chisq;
run;

proc freq data=EPL4;
exact fisher;
tables european*gfgroup/chisq;
run;

proc rank data=EPL out=srankEPL descending;
var salary;
ranks sranking;
run;

data EPL5;
set srankEPL;
if sranking<=5 then sgroup='a';
else if sranking<=10 then sgroup='b';
else if sranking<=15 then sgroup='c';
else sgroup='d';
run;

proc anova data=EPL5;
class sgroup;
model points=sgroup;
means sgroup/lines alpha=0.05 lsd;
means sgroup/hovtest=bartlett;
run;
quit;

data EPL6;
set EPL5;
if nowticket>500 then ntgroup='expensive';
else ntgroup='cheap';
run;

proc anova data=EPL6;
class sgroup ntgroup;
model points=sgroup ntgroup sgroup*ntgroup;
means sgroup ntgroup sgroup*ntgroup;
run;
quit;

proc reg data=EPL;
model points=gd/p clm cli dw;
run;
quit;

proc reg data=EPL;
model points=gd nowticket salary/stb selection=backward;
run;
quit;
