/*
METAPOPULATION MODEL
VIRAL REPLICATION HAPPENS IN MULTIPLE HOSTS WHO ARE CAPABLE OF TRANSMITTING THE VIRUS TO EACH OTHER IN A RANCOM NETWORK.
in 1.1.2, the factor in mutation is made so that it just multiplies the n(l,k) without going through for loops to subtract 
each factors everytime.
Also, mutation matrix is made to not do the redundant for loops which will drastically improve the speed of mutate function.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

// function pre-decleration
//ran1
//poidev
void mutate(double**** pop, int* curpop, double u, int kmax, int host_num, double factor[], double** mutmax);
void reast(double**** pop, int* curpop, int kmax, int host_num, double r, double N);
void repr(double**** pop, int* curpop, int kmax, int host_num, double s, double* N, double c, double K, long* seed);
float ran1(long *seed);
float gammln(float xx);
float poidev(float xm,long *idum);
double fact(int num);
double poipmf(double l, int k);

#define PRINTF 0

int main(int argc, char *argv[])
{
	// clock start
	//clock_t begin = clock();

	// prarmeter calling from os command
	char *destination = argv[1];
	char *timestep_s = argv[2];
	char *krecord_s = argv[3];
	char *untilext_s = argv[4];
	char *rep_s = argv[5];
	char *s_s = argv[6];
	char *N0_s = argv[7];
	char *K_s = argv[8];
	char *u_s = argv[9];
	char *gen_num_s = argv[10];
	char *c_s = argv[11];
	char *r_s = argv[12];
	char *seed_s = argv[13];
	char *host_num_s = argv[14];
	char *kmax_s = argv[15];
	char *end1;

	int timestep = (int) strtol(timestep_s,&end1,10);
	int krecord = (int) strtol(krecord_s, &end1, 10);
	int untilext = (int) strtol(untilext_s,&end1,10);
	int rep = (int) strtol(rep_s,&end1,10);
	double s = (double) strtof(s_s, NULL);
	int N0 = (int) strtol(N0_s,&end1,10);
	int K = (int) strtol(K_s,&end1,10);
	double u = (double) strtof(u_s,NULL);
	int gen_num = (int) strtol(gen_num_s,&end1,10);
	double c = (double) strtof(c_s,NULL);
	double r = (double) strtof(r_s,NULL);
	long seed = strtol(seed_s,&end1,10);
	int host_num = (int) strtol(host_num_s,&end1,10);
	int kmax = (int) strtol(kmax_s,&end1,10);
	printf("destination=%s, timestep=%d, krecord=%d, untilext=%d, rep=%d, s=%.2f, N0=%d, K=%d, u=%.5f, gen_num=%d, c=%.2f, r=%.2f, kmax=%d\n",destination,timestep,krecord,untilext,rep,s,N0,K,u,gen_num,c,r,kmax);
	
	//check if the destination folder exists and if not, make one.
	char* dest2 = (char*) malloc(sizeof(char)*50);
	sprintf(dest2,"./data/%s",destination);
	struct stat st = {0};
	if (stat(dest2,&st) == -1)
	{
		mkdir(dest2, 0700);
	}

	char* filename = (char*) malloc(sizeof(char)*1000);
	sprintf(filename,"%s/m1.1.2s_%d,%d,%d,%.3f,%d,%d,%.5f,%d,%.2f,%.2f,%d,%d(0).csv",dest2,timestep,krecord,rep,s,N0,K,u,gen_num,c,r,kmax,host_num);
	int filenum = 0;

	while ( access(filename, F_OK) != -1) 
	{
		filenum += 1;
		sprintf(filename,"%s/m1.1.2s_%d,%d,%d,%.3f,%d,%d,%.5f,%d,%.2f,%.2f,%d,%d(%d).csv",dest2,timestep,krecord,rep,s,N0,K,u,gen_num,c,r,kmax,host_num,filenum);
	}
	FILE* fPointer;
	fPointer = fopen(filename,"w");
	free(dest2);
	free(filename);
	if (timestep)
	{
		fprintf(fPointer,"rep,gen,pop2,k2\n");
		//printf("rep,gen%s\n",str);
	}
	else
	{
		fprintf(fPointer,"rep,pop2,k2\n");
		//printf("rep,gen%s\n",str);
	}


	// todo:
	
	// set the parameters with os command.
	// set timer, file
	
	int k,i,j,m;
	int gen,repe; //current generation and repetition.
	int curpop; //current population index ur working witih.
	double* factorpre = (double*) malloc(sizeof(double)*(2*kmax + 1)); // probability of getting n mutations organized in an array
	double factor[2*kmax + 1]; // sum of factorpre. factor[3] is a sum of factorpre[0] through factorpre[3] 
	double**** pop;
	double** mutmat;
	double N; // current pop size
	double record; // record of mutation amount in a pop of a host (mean or minimum depending on krecord)	
	
	for (i=0; i<=2*kmax; i++)
	{
		factorpre[i] = poipmf(2*u,i); // probability of choosing i amount of mutation in a generation step.
		factor[i] = 0;
		if (i > 0)
		{
			for (j=1; j<=i; j++)
			{
				factor[i] += factorpre[j];
			}
		}
	}
	
	int ksq = (int)pow((kmax+1),2);
	mutmat = (double**) malloc(sizeof(double*)*ksq); // mutation matrix
	for(i=0; i<ksq; i++)
	{
		mutmat[i] = (double*) malloc(sizeof(double)*ksq);
	}
	int left,l,l2,l3,select,matcoor1,matcoor2;
	for(j=0; j<=kmax; j++)
	{
		for(k=0; k<=kmax; k++)
		{
			matcoor1 = (kmax + 1)*j + k;
			left = 2*kmax - (j + k);
			for(l=1; l<=left; l++)
			{
				for(l2=0; l2<=l; l2++)
				{
					l3 = l -l2;
					if (l2 + j <= kmax && l3 + k <= kmax)
					{
						matcoor2 = (kmax + 1)*(j + l2) + (k + l3);
						if (l <= kmax - k && l <= kmax - j)
						{
							//printf("j=%d, k=%d, l=%d, j2=%d, k2=%d matcoor1=%d, matcoor2=%d, factorpre[l]/(x)=%.6f, choice=1\n",j,k,l,j+l2,k+l3,matcoor1,matcoor2,factorpre[l]/(l + 1));
							mutmat[matcoor2][matcoor1] = factorpre[l]/(l + 1);
						}
						else if(l <= kmax - k || l <= kmax - j)
						{

							if (k > j)
							{
								select = k;
							}
							else
							{
								select = j;
							}
							//printf("worked2 added=%.3f\n",factor2/(left + 1 - l));
							mutmat[matcoor2][matcoor1] = factorpre[l]/(kmax - select + 1);
							//printf("j=%d, k=%d, l=%d, j2=%d, k2=%d matcoor1=%d, matcoor2=%d, factorpre[l]/(x)=%.6f, choice=2\n",j,k,l,j+l2,k+l3,matcoor1,matcoor2,factorpre[l]/(kmax - select + 1));
							//printf("pop[%d][%d][%d][%d]=%.3f\n",m,i,j+l2,k+l3,pop[m][i][j+l2][k+l3]);
						}
						else
						{
							mutmat[matcoor2][matcoor1] = factorpre[l]/(2*kmax - k - j - l + 1);
							//printf("j=%d, k=%d, l=%d, j2=%d, k2=%d matcoor1=%d, matcoor2=%d, factorpre[l]/(x)=%.6f, choice=3\n",j,k,l,j+l2,k+l3,matcoor1,matcoor2,factorpre[l]/(2*kmax - k - j - l + 1));
						}
					}
				}
			}
		}
	}
	
	free(factorpre);
	//pop: entire population including all metapops in each host. pop[0][1][2] = number of individual with 1 mutation in 1st segment and 2 mutation in 2nd segment in host 0.
	
	pop = (double****) malloc(sizeof(double***)*2);
	for (m=0; m<2; m++)
	{
		pop[m] = (double***) malloc(sizeof(double**)*host_num);
		for (i=0; i<host_num; i++)
		{
			pop[m][i] = (double**) malloc(sizeof(double*)*(kmax+1));
			for (j=0; j<=kmax; j++)
			{
				pop[m][i][j] = (double*) malloc(sizeof(double)*(kmax+1));
			}
		}
	}
	
	double mutate_time = 0;
	double reast_time = 0;
	double repr_time = 0;
	clock_t begin, end;
	
	for (repe=0; repe < rep; repe++)
	{
		if (repe % 100 == 0)
		{
			printf("\rREP = %d",repe);
			fflush(stdout);
		}
		for(i=0; i<host_num; i++)
		{
			pop[0][i][0][0] = (double) N0; //N0 of virus with 0 mutations at initial condition.
		}
		N = (double) N0;
		curpop = 0;
		for (gen=0; gen < gen_num; gen++)
		{
			if(N>0)
			{
				begin = clock();
				mutate(pop, &curpop, u, kmax, host_num, factor, mutmat);
				end = clock();
				mutate_time += (double) (end - begin)/ CLOCKS_PER_SEC;
				begin = clock();
				reast(pop, &curpop, kmax, host_num, r, N);
				end = clock();
				reast_time += (double) (end - begin)/ CLOCKS_PER_SEC;
				begin = clock();
				repr(pop, &curpop, kmax, host_num,s,&N,c,K,&seed);
				end = clock();
				repr_time += (double) (end - begin)/ CLOCKS_PER_SEC;
			}
			if(PRINTF)
			{
				float count;
				count = 0;
				printf("after repr outside the function\n");
				for (i=0; i<host_num; i++)
				{
					for (j=0; j<=kmax; j++)
					{
						for (k=0; k<=kmax; k++)
						{
							printf("pop[%d][%d][%d][%d]=%.3f\n",curpop,i,j,k,pop[curpop][i][j][k]);
							count += pop[curpop][i][j][k];
						}
					}
				}
				printf("count=%.3f\n",count);
			}
			
			//printf("gen=%d\n",gen);
			//printf("-----------------\n");
			// record it to the file
			if (timestep == 1)
			{
				if(krecord == 0) // record mean
				{
					if(N>0)
					{
						record = 0;
						for (i=0; i<host_num; i++)
						{
							for (j=0; j<=kmax; j++)
							{
								for (k=0; k<=kmax; k++)
								{
									record += pop[curpop][i][j][k]/N * (j + k);
								}
							}
						}
					}
					else
					{
						record = -1.0;
					}
					fprintf(fPointer,"%d,%d,%.2f,%.2f\n",repe+1,gen+1,N,record);
				}
				else // record minimum
				{
					record = kmax*2 + 1;
					if (N>0)
					{
						for (i=0; i<host_num; i++)
						{
							for (j=0; j<=kmax; j++)
							{
								for (k=0; k<=kmax; k++)
								{
									if((j + k) < record)
									{
										if(pop[curpop][i][j][k] > 0)
										{
											record = j + k;
										}
									}
								}
							}
						}
					}
					else
					{
						record = -1.0;
					}
					fprintf(fPointer,"%d,%d,%.2f,%.2f\n",repe+1,gen+1,N,record);
				}
			}
		}
		if(timestep == 0)
		{	
			if(krecord == 0) // record mean
			{
				if(N>0)
				{
					record = 0;
					for (i=0; i<host_num; i++)
					{
						for (j=0; j<=kmax; j++)
						{
							for (k=0; k<=kmax; k++)
							{
								record += pop[curpop][i][j][k]/N * (j + k);
							}
						}
					}
				}
				else
				{
					record = -1.0;
				}
				fprintf(fPointer,"%d,%.2f,%.2f\n",repe+1,N,record);									
			}
			else // record minimum
			{
				record = kmax*2 + 1;
				if (N > 0)
				{
					for (i=0; i<host_num; i++)
					{
						for (j=0; j<=kmax; j++)
						{
							for (k=0; k<=kmax; k++)
							{
								if((j + k) < record)
								{
									if(pop[curpop][i][j][k] > 0)
									{
										record = j + k;
									}
								}
							}
						}
					}
				}
				else
				{
					record = -1.0;
				}
				fprintf(fPointer,"%d,%.2f,%.2f\n",repe+1,N,record);									
			}
		}
		for (m=0; m<2; m++)
		{
			for (i=0; i<host_num; i++)
			{
				for (j=0; j<=kmax; j++)
				{
					for (k=0; k<=kmax; k++)
					{
						pop[m][i][j][k] = 0;
					}
				}
			}
		}
	}
	
	
	// freeing pop
	
	for (m=0; m<2; m++)
	{
		for (i=0; i<host_num; i++)
		{
			for (j=0; j<=kmax; j++)
			{
				free(pop[m][i][j]);
			}
			free(pop[m][i]);
			}
		free(pop[m]);	
	}
	free(pop);
		
	
	// freeing mutmax

	for (i=0; i<ksq; i++)
	{
		free(mutmat[i]);
	}
	free(mutmat);
	// make mutation, recombination, reproduction into a single process.
	fclose(fPointer);




	// clock_t end = clock();
	//double time_spent = (end - begin)/ CLOCKS_PER_SEC;
	printf("\n");
	printf("mutate=%.3f, reast=%.3f, repr=%.3f\n",mutate_time, reast_time, repr_time);
	//printf("time spend was %.2f minutes\n", time_spent/60.0);
	return 0;
}


void mutate(double**** pop, int* curpop, double u, int kmax, int host_num, double factor[], double** mutmax)
{
	int m,m2,i,j,k;
	if (*curpop == 0)
	{
		m2 = *curpop;
		m = 1;
		*curpop = m;
	}
	else 
	{
		m2 = *curpop;
		m = 0;
		*curpop = m;
	}
	/*
	double count;
	count = 0;
	printf("before mutation inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
			printf("pop[%d][%d][%d][%d]=%.2f\n",m2,i,j,k,pop[m2][i][j][k]);
			count += pop[m2][i][j][k];
			}
		}
	}
	printf("count=%.3f\n",count);
	printf("\n");
	*/
	//double equate;
	int left; // max of number of mutations that n(l,k) can give rise to.
	int ii, coor1, coor2, matcoor;
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				//printf("loop start (i,j,k)=(%d,%d,%d)\n",i,j,k);
				// going to sum all the mutation_rate*n(l,k)
				left = 2*kmax - (j + k);
				//printf("left=%d\n",left);
				pop[m][i][j][k] += pop[m2][i][j][k] - factor[left]*pop[m2][i][j][k];
				//printf("factorsum= %.3f\n",factor[left]*pop[m2][i][j][k]);
				matcoor = (kmax + 1)*j + k;

				// do matrix computation
				for (ii=0; ii<(int)pow((kmax+1),2); ii++)
				{
					// convert matrix coordinate to j,k coordinate
					coor1 = ii/(kmax+1); // j
					coor2 = ii - (coor1*(kmax+1)); // k
					pop[m][i][j][k] += pop[m2][i][coor1][coor2]*mutmax[matcoor][ii];
				}
			}
		}
	}
	/*
	count = 0;
	printf("after mutation inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				pop[m2][i][j][k] = 0;
				printf("pop[%d][%d][%d][%d]=%.2f\n",m,i,j,k,pop[m][i][j][k]);
				count += pop[m][i][j][k];
			}
		}
	}
	printf("count=%.3f\n",count);
	*/
}


void reast(double**** pop, int* curpop, int kmax, int host_num, double r, double N)
{
	int m,m2,i,j,k;
	int jj;
	if (*curpop == 0)
	{
		m2 = *curpop;
		m = 1;
		*curpop = m;
	}
	else 
	{
		m2 = *curpop;
		m = 0;
		*curpop = m;
	}
	/*
	double count;
	count = 0;
	printf("before reast inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
			printf("pop[%d][%d][%d][%d]=%.2f\n",m2,i,j,k,pop[m2][i][j][k]);
			count += pop[m2][i][j][k];
			}
		}
	}
	printf("count=%.3f\n",count);
	*/
	double jp, kp; // proportion of population with certain j value (jp) and k value (kp)
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				pop[m][i][j][k] = pop[m2][i][j][k]*(1 - r);
				jp = 0;
				kp = 0;
				for(jj=0; jj<=kmax; jj++)
				{
					kp += pop[m2][i][jj][k];
					jp += pop[m2][i][j][jj]; 
				}
				kp = kp/N;
				jp = jp/N;
				pop[m][i][j][k] += N*kp*jp*r;
				
			}
		}
	}
	/*
	count = 0;
	printf("after reast inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				pop[m2][i][j][k] = 0;
				printf("pop[%d][%d][%d][%d]=%.2f\n",m,i,j,k,pop[m][i][j][k]);
				count += pop[m][i][j][k];
			}
		}
	}
	printf("count=%.3f\n",count);
	*/
}


void repr(double**** pop, int* curpop, int kmax, int host_num, double s, double* N, double c, double K, long* seed)
{
	int m,m2,i,j,k;
	if (*curpop == 0)
	{
		m2 = *curpop;
		m = 1;
		*curpop = m;
	}
	else 
	{
		m2 = *curpop;
		m = 0;
		*curpop = m;
	}
	double newN = 0; //reset N to 0.
	/*
	double count;
	count = 0;
	printf("before repr inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
			printf("pop[%d][%d][%d][%d]=%.2f\n",m2,i,j,k,pop[m2][i][j][k]);
			count += pop[m2][i][j][k];
			}
		}
	}
	printf("count=%.3f\n",count);
	*/
	double poirate;
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				if (k + j >= kmax)
				{
					pop[m][i][j][k] = poidev(0,seed);
					//printf("poirate=0\n");
					newN += pop[m][i][j][k];

				}
				else
				{
					poirate = pop[m2][i][j][k] * pow((1 - s), (k + j)) * (1 - c) * ((double)2 /(1.0 + (*N/K)));
					//printf("poirate=%.3f, pow=%.3f, 1-c=%.3f, carrying=%.3f\n",poirate, pow((1-s),(k+j)),(1-c),((double)2 /(1.0 + (*N/K))));
					pop[m][i][j][k] = poidev(poirate,seed);
					newN += pop[m][i][j][k];
				}
				//printf("pop[%d][%d][%d][%d]=%d\n",m,i,j,k,pop[m][i][j][k]);
			}
		}
	}
	
	//count = 0;
	//printf("after repr inside the function\n");
	for (i=0; i<host_num; i++)
	{
		for (j=0; j<=kmax; j++)
		{
			for (k=0; k<=kmax; k++)
			{
				pop[m2][i][j][k] = 0;
				//printf("pop[%d][%d][%d][%d]=%.2f\n",m,i,j,k,pop[m][i][j][k]);
				//count += pop[m][i][j][k];
			}
		}
	}
	//printf("count=%.3f\n",count);
	
	*N = newN;
}

// random number generating functions ran1=uniform [0,1], bnldev= binomial. gammln is needed for bnldev.
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7
#define RNMX (1.0-EPS)	
float ran1(long *idum)
{
  int j;
  long k;
  static long iy=0;
  static long iv[NTAB];
  float temp;

  if (*idum <= 0 || !iy) {
    if (-(*idum) < 1) *idum=1;
    else *idum = -(*idum);
    for (j=NTAB+7;j>=0;j--) {
      k=(*idum)/IQ;
      *idum=IA*(*idum-k*IQ)-IR*k;
      if (*idum < 0) *idum += IM;
      if (j < NTAB) iv[j] = *idum;
    }
    iy=iv[0];
  }
  k=(*idum)/IQ;
  *idum=IA*(*idum-k*IQ)-IR*k;
  if (*idum < 0) *idum += IM;
  j=iy/NDIV;
  iy=iv[j];
  iv[j] = *idum;
  if ((temp=AM*iy) > RNMX) return RNMX;
  else return temp;
}
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef NTAB
#undef NDIV
#undef EPS
#undef RNMX

float gammln(float xx)
{
	double x,y,tmp,ser;
	static double cof[6]={76.18009172947146,-86.50532032941677,
		24.01409824083091,-1.231739572450155,
		0.1208650973866179e-2,-0.5395239384953e-5};
	int j;

	y=x=xx;
	tmp=x+5.5;
	tmp -= (x+0.5)*log(tmp);
	ser=1.000000000190015;
	for (j=0;j<=5;j++) ser += cof[j]/++y;
	return -tmp+log(2.5066282746310005*ser/x);
}

#define PI 3.141592654

float poidev(float xm,long *idum)
{
	float gammln(float xx);
	float ran1(long *idum);
	static float sq,alxm,g,oldm=(-1.0);
	float em,t,y;

	if (xm < 12.0) {
		if (xm != oldm) {
			oldm=xm;
			g=exp(-xm);
		}
		em = -1;
		t=1.0;
		do {
			++em;
			t *= ran1(idum);
		} while (t > g);
	} else {
		if (xm != oldm) {
			oldm=xm;
			sq=sqrt(2.0*xm);
			alxm=log(xm);
			g=xm*alxm-gammln(xm+1.0);
		}
		do {
			do {
				y=tan(PI*ran1(idum));
				em=sq*y+xm;
			} while (em < 0.0);
			em=floor(em);
			t=0.9*(1.0+y*y)*exp(em*alxm-gammln(em+1.0)-g);
		} while (ran1(idum) > t);
	}
	return em;
}

#undef PI

double fact(int num)
{
	// factorial
	double val = 1;
	int i;
	for (i=1; i<=num; i++)
	{
		val *= i;
	}
	return val;
}

double poipmf(double l, int k)
{
	//pmf function of poisson
	double val = (pow(l,k)*exp(-1*l))/fact(k);
	return val;
}
