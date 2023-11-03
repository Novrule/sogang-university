data chol;
input chol @@;
cards;
239 161 210 179 212 195 301 357 233 256
234 195 199 284 245 174 310 286 176 212
297 249 282 233 205 286 269 305 247 292
;
run;

proc freq data=chol;
tables chol;
run;

proc univariate data=chol noprint;
histogram chol;
run;

proc univariate data=chol;
var chol;
run;
