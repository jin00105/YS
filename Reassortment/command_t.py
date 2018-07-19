# was used to make ultimate_test and ultimate_test2 data.
# parameter adding needs to be fixed for comp1.2!!
from os import system as cd
import sys
import numpy as np
import time
from datetime import date
td = date.today() #simulation start date
start = time.time()
seed = np.random.randint(-9223372036854775808,-1)
back = 0
timestep = 0
krecord = 0 #  2= smallest k of the subpop. 1= all indiv's k. 0=mean k.
untilext = 0
rep = 100
L = 300
s = [0.05]
N0 = 1000
K = 1000
mu = [0.0007]#[0.00067] #[0.0002,0.0003,0.0004,0.0005,0.0006,0.0007,0.0008,0.0009,0.0010,0.0011,0.0012]
gen_num = 500
cost = [0.00]
r = 0.5 
r2 = 0.75
N1r = [0.5] #[0.5]
q = [2]
a = 0
b = 0
type = 0

destination = 'test'
file2run = sys.argv[1]
version = file2run[4:7]

count = 0
for m in range(len(mu)):
	for i in range(len(s)):
		for j in range(len(N1r)):
			for k in range(len(cost)):
				for l in range(len(q)):
					if version == '1.2':
						params = '%s %d %d %d %d %d %f %d %d %.5f %d %f %f %f %d %d %f %f %f %d'%(destination,back,timestep,krecord,
																				rep,L,s[i],N0,K,mu[m],gen_num,cost[k],r,N1r[j],seed,untilext,q[l],a,b,type)
					elif version == '1.3':
						params = '%s %d %d %d %d %d %f %d %d %.5f %d %f %f %f %d %d %f %f %f %d'%(destination,back,timestep,krecord,
																				rep,L,s[i],N0,K,mu[m],gen_num,cost[k],r,N1r[j],seed,untilext,q[l],a,b,type)
					elif version == '2.3':
						params = '%s %d %d %d %d %d %f %d %d %.5f %d %f %f %f %d %d %f %f %f %d'%(destination,back,timestep,krecord,
																				rep,L,s[i],N0,K,mu[m],gen_num,cost[k],r,N1r[j],seed,untilext,q[l],a,b,type)
					if file2run[-1] == "y": # file is .py
						cd('python %s %s'%(file2run, params))
					else: # file is .c
						cd('gcc -Wall %s -o cfile -lm'%(file2run))
						cd('./cfile %s'%(params))
					count += 1
					print("%d/%d DONE\n"%(count, len(cost)*len(N1r)*len(s)*len(mu)*len(q)))


end = time.time()
print('it took following minutes: ', (end - start)/60)
cd('mkdir donzo%d.%d.%d'%(td.day,td.month,td.year))

