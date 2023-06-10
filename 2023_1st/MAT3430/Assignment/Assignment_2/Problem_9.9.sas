data region;
do region = 'city', 'farm';
do level = 'A', 'B', 'C', 'D';
input count @@;
output;
end;
end;
cards;
76 53 59 48 124 147 141 152
;
run;

proc freq data = region order = data;
weight count;
tables region*level / expected chisq measures;
run;
