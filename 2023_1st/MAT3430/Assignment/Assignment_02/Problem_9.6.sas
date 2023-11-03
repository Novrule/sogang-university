data vegetable;
input vegetable $ level $ count @@;
cards;
lettuce A 32 lettuce B 8
spinach A 28 spinach B 12
tomato A 19 tomato B 21
;
run;

proc freq data = vegetable order = data;
weight count;
exact fisher;
tables vegetable*level / expected chisq;
run;
