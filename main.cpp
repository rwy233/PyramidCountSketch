#include<bits/stdc++.h>
#include"BOBHash32.h"
#include"data.h"
#include"count.h"
#include"pyramid.h"

using namespace std;

int main(){
    long long input_max = 10000000;
    double memory = 1 * 1024 * 1024;
    double alpha = 0.4;
    int k = 5; 
    int innerk = 1;
    uint32_t Rseed = time(NULL);
#define algo 1

#if algo==1
    PyramidSketch1 S(memory,alpha,k,innerk,Rseed);
#elif algo==2
    PyramidSketch3 S(memory,alpha,k,innerk,Rseed);
#elif algo==3    
    CountSketch S(memory,k,Rseed);
#endif
    FILE* f =fopen("../130000.dat","rb");

    map<Data,int> s;
	int heavy = 0;
	double aeS=0,reS=0,eS = 0;
	int start = clock();
	Packet packet;
    for(int i=0; i<input_max;++i){
		fread(&packet,1,(DATA_LEN+TIME_LEN),f);
		S.Insert(packet.data);
		s[packet.data] += 1;
		if ( i % 1000000 == 0) {
			printf("%d\n",i);
		}
    }
	for(auto i = s.begin(); i != s.end(); ++i){
		heavy ++;
		double stdCount = i->second;
		double myCount  = S.Query(i->first);
		double ae = fabs(myCount - stdCount);
		double re = ae / stdCount;	
		double e = (myCount - stdCount);
		aeS += ae; reS +=re;eS +=e;
	}
	printf("%d\n",s.size());
	printf("%lf %lf %lf\n", aeS/heavy, reS/heavy,eS/heavy);
	
	return 0;
}