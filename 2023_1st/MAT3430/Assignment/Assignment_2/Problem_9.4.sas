data company;
do rank = 'Clerical', 'Executive', 'Factory', 'Supervisor';
do type = 'A', 'B', 'C';
input count @@;
output;
end;
end;
cards;
140 40 20 70 20 10 160 30 10 80 10 10
;
run;

proc freq data = company order = data;
weight count;
tables rank*type / expected chisq measures;
run;
