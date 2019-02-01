1.generate_build_config.sh
将策划的建筑配置csv文件生成json格式，方便策划配置；
使用示例：
./generate_build_config.sh Buildings2A.csv 
输出到 Buildings2A.csv.out文件
csv文件格式(不要header只留数据)：
id,lvl,upgrade_cd,accelerate_time,cost_coins,cost_wood,value,out_exp,out_exploit
101,1,22,100,6,4,14,1,0