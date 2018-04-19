# was used for making back_test2 data.

from os import system as cd
import sys
import numpy as np
seed = np.random.randint(-9223372036854775808,-1)
back = 0
timestep = 1
krecord = 0 #  2= smallest k of the subpop. 1= all indiv's k. 0=mean k.
untilext = 0
rep = 1
L = 300
s = 0.05
N0 = 1000
K = 1000
mu = 0.0005
gen_num = 100
cost = 0
r = 0.5
N1r = 0
destination = 'ctest'
file2run = sys.argv[1]
version = file2run[4:7]

count = 0

if version == '1.2':
	params = '%s %d %d %d %d %d %d %f %d %d %.5f %d %f %f %f'%(destination,back,timestep,krecord,untilext,
																rep,L,s,N0,K,mu,gen_num,cost,r,N1r)
elif version == '1.3':
	params = '%s %d %d %d %d %d %f %d %d %.5f %d %f %f %f'%(destination,back,timestep,krecord,
															rep,L,s,N0,K,mu,gen_num,cost,r,N1r)
if file2run[-1] == "y": # file is .py
	cd('python %s %s'%(file2run, params))
else: # file is .c
	cd('gcc -Wall %s -o cfile -lm'%(file2run))
	cd('./cfile %s %d'%(params, seed))
count += 1
# print("%d/%d DONE\n"%(count, len(mu)*len(back)*len(N1r)))