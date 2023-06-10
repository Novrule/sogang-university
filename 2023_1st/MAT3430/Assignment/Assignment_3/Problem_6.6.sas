data sleep;
do ethanol = '0mg', '1mg', '2mg', '3mg';
input count @@;
output;
end;
cards;
86 79 73 72 89
57 70 57 55 54
50 43 47 49 42
30 38 27 38 38
;
run;

proc anova data = sleep;
class ethanol;
model count = ethanol;
means ethanol / hovtest= bartlett;
run;
quit;
