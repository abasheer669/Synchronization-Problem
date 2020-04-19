#include<pthread.h>
#include<iostream>
#include<time.h>
#include<unistd.h>
#include<iomanip>

#define VILLCNT 5
#define STARVCNT 1

using namespace std;
void* ATB(void*);
void* BTA(void*);
pthread_mutex_t mutex,atb,bta;
time_t start;
int atbcnt,btacnt;
int atbcyc,btacyc;

void display(int id,char *str="")
{
	for(int i=0;i<VILLCNT;i++)
	{ 
		if(id==-1) cout<<setw(7)<<"Vill. "<<i+1<<'|';
		else
		{
			if(i==id-1) cout<<setw(8)<<str<<'|';
			else cout<<setw(9)<<'|';
		}
	}
	cout<<time(NULL)-start<<"sec"<<endl;
}

void createvillagers(pthread_t vill,int *id)
{
	void* (*villgen)(void*);
	srand(rand());
	if(rand()%2)	//villager creation random village
		villgen=&ATB;
	else
		villgen=&BTA;
	sleep(rand()%4);	//Villager creation random time
	pthread_create(&vill,NULL,villgen,(id));
}

void* ATB(void* id)
{
	int traveltime;
	display(*(int*)id,"Appeared");
	display(*(int*)id,"A to B");
	display(*(int*)id,"Waiting");	
	pthread_mutex_lock(&atb);
	display(*(int*)id,"Got Perm");
	pthread_mutex_lock(&mutex);	
	::atbcnt++;
	::atbcyc++;	
	if(::atbcnt==1)
		pthread_mutex_trylock(&bta);
	if(::atbcyc!=STARVCNT)
		pthread_mutex_unlock(&atb);
	else
		::atbcyc=0;
	pthread_mutex_unlock(&mutex);
	traveltime=rand()%10;
	display(*(int*)id,"Crossing");
	sleep(traveltime);
	display(*(int*)id,"Crossed!");
	pthread_mutex_lock(&mutex);
	::atbcnt--;
	if(::atbcnt==0)
		pthread_mutex_unlock(&bta);
	pthread_mutex_unlock(&mutex);
}
void* BTA(void* id)
{
	int traveltime;
	display(*(int*)id,"Appeared");
	display(*(int*)id,"B to A");
	display(*(int*)id,"Waiting");	
	pthread_mutex_lock(&bta);
	display(*(int*)id,"Got Perm");
	pthread_mutex_lock(&mutex);
	::btacnt++;
	::btacyc++;
	if(::btacnt==1)
		pthread_mutex_trylock(&atb);
	if(::btacyc!=STARVCNT)
		pthread_mutex_unlock(&bta);
	else
		::btacyc=0;
	pthread_mutex_unlock(&mutex);
	traveltime=rand()%10;
	display(*(int*)id,"Crossing");
	sleep(traveltime);
	display(*(int*)id,"Crossed!");
	pthread_mutex_lock(&mutex);
	::btacnt--;
	if(::btacnt==0)
		pthread_mutex_unlock(&atb);
	pthread_mutex_unlock(&mutex);
}

int main()
{
	::start=time(NULL);
	pthread_t vill[VILLCNT];
	::atbcnt=0;::btacnt=0;
	::atbcyc=0;::btacyc=0;
	int id[VILLCNT];
	pthread_mutex_init(&mutex,NULL);
	pthread_mutex_init(&atb,NULL);
	pthread_mutex_init(&bta,NULL);
	display(-1);
	for(int i=0;i<VILLCNT;i++)
	{
		id[i]=i+1;	
		createvillagers(vill[i],&id[i]);
	}
	sleep(100);
	return 0;
}
