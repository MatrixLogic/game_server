#!/bin/sh


 awk -F ',' 'BEGIN{t=0;}{
		
		
		t++;		
		if(t== 1) {
			print "{\"itemes\":["
		}
		if(t <= 136) {
			print  "{\"id\":" $1 ",\"name\":\""$2 "\",\"unlock_level\":"$3 ",\"time_show\":"$4 ",\"material_1\":\""$5 "\",\"count_1\":"$6 ",\"material_2\":\""$7 "\",\"count_2\":"$8 ",\"material_3\":\""$9 "\",\"count_3\":"$10 ",\"material_4\":\""$11 "\",\"count_4\":"$12 ",\"price\":"$13 ",\"dimond_cost\":"$14 "},"
			print "\n"
		}
		else {
			print  "{\"id\":" $1 ",\"name\":\""$2 "\",\"unlock_level\":"$3 ",\"time_show\":"$4 ",\"material_1\":\""$5 "\",\"count_1\":"$6 ",\"material_2\":\""$7 "\",\"count_2\":"$8 ",\"material_3\":\""$9 "\",\"count_3\":"$10 ",\"material_4\":\""$11 "\",\"count_4\":"$12 ",\"price\":"$13 ",\"dimond_cost\":"$14 "}"
			print "\n"
			print "]}"
		}
 		
	}' $1  > $1.out

