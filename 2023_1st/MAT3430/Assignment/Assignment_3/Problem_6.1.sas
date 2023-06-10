data fiberpower;
do cotton = '15%', '20%', '25%', '30%', '35%';
input power @@;
output;
end;
cards;
7 12 14 19 7
7 17 18 25 10
15 12 18 22 11
11 18 19 19 15
9 18 19 23 11
;
run;

proc anova data = fiberpower;
class cotton;
model power = cotton;
means cotton / hovtest = bartlett;
run;
quit;
