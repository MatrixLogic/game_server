#!/bin/sh


 awk -F ',' 'BEGIN{t=0;}{
		upgrade_cd = $3;
		accelerate_time = $4;
		cost_coins = $5;
		cost_wood = $6;
		value = $7;
		out_exp = $8;
		out_exploit = $9;	
		t++;		
		
		if (t  % 100 == 0) {
			print "#"$1"#" "{\"lvl\":" $2 ",\"upgrade_cd\":"$3 ",\"accelerate_time\":"$4 ",\"cost_coins\":"$5 ",\"cost_wood\":"$6 ",\"value\":"$7 ",\"out_exp\":"$8 ",\"out_exploit\":"$9 "}" 
			print "\n\n\n"
		} else
		{
		 print "#"$1"#" "{\"lvl\":" $2 ",\"upgrade_cd\":"$3 ",\"accelerate_time\":"$4 ",\"cost_coins\":"$5 ",\"cost_wood\":"$6 ",\"value\":"$7 ",\"out_exp\":"$8 ",\"out_exploit\":"$9 "}," 
		}
 		
	}' $1  > $1.out

