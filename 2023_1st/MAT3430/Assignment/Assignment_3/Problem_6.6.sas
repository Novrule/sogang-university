data sleep;
do ethanol = '0mg', '1mg', '2mg', '3mg';
input count @@;
output;
end;
cards;
86 57 50 30
79 70 43 38
73 57 47 27
72 55 49 38
89 54 42 38
;
run;

proc anova data = sleep;
class ethanol;
model count = ethanol;
means ethanol / hovtest= bartlett;
run;
quit;
