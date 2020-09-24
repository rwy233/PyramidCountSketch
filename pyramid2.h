#include <bits/stdc++.h>
#include "data.h"
#include "BOBHash32.h"

using namespace std;
/*
    进位 捆绑
*/
struct Count{
	int*count;
	int bitnum;
	int n,innerk;
	BOBHash32*bh;
	void init(int _bitnum,int _n,int _innerk){
		n = _n;
        innerk = _innerk;
		count = new int[n];
		bitnum = _bitnum;
		memset(count,0,sizeof(int) * n);
		for (int i=0; i<innerk ;++i){
			bh[i].initialize( random()%7 + 7*i);
		}	
	}
};
const int step_num = 4; // 4 8 16 32
class PyramidSketch4{
    double memory;
    double alpha;
    int k,innerk;
    // int step_num;
    Count*a;
    uint32_t Rseed;
public:
    PyramidSketch4(int _mem,int _alp,int _k,int _innerk,uint32_t _Rseed)
    :memory(_mem),alpha(_alp),k(_k),innerk(_innerk),Rseed(_Rseed){
        srand(Rseed);
        a = new Count[step_num];
        int each_bit = (memory * 8) *(1-alpha) / (1 - pow(alpha,step_num));
        int bitnum = 4;
        for(int i=0;i<step_num;++i){
            a[i].init(bitnum, each_bit/bitnum,innerk);
            bitnum *= 2;
		    each_bit = each_bit * alpha;
        }
    }

    HashRet f(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i){
            ret.ret[i] = bh[i].run((const char*)&x, sizeof(x)) % a[0].n;
        }
        return ret;
    }

    HashRet g(const frame&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i)
            ret.ret[i] = (bh[i + k].run((const char*)&x, sizeof(x)) & 1) ? 1 : -1;
        return ret;
    }

#define nxtx(i) (a[d].bh[i].run((const char*)&tmp,sizeof(tmp)) %  a[d+1].n)

    int query(int d,int x,const Data&data){
        if( d == step_num ) return a[d].count[x]; 
        if(a[d].count[x] + 1 == (1<<a[d].bitnum-1) || a[d].count[x] == -(1<<a[d].bitnum-1) ) {
            ele tmp = (ele){data,x};
            int q[k];
            for(int i=0;i<innerk;++i) q[i] = query(d+1,nxtx(i),data);
            sort(q,q+innerk);
            return q[innerk/2] + a[d].count[x];
        }
        return a[d].count[x];
    }

    void modify(int d,int x,int v,const Data&data){
        if( d == step_num ) return;
        if(a[d].count[x] + 1 == (1<<a[d].bitnum-1)){
            ele tmp = (ele){data,x};
            for (int i=0; i<innerk; ++i){
                modify(d+1,nxtx(i),1,data);
            }
            return;
        }
        if(a[d].count[x] == -(1<<a[d].bitnum-1)){
            ele tmp = (ele){data,x};
            for (int i=0; i<innerk; ++i){
                modify(d+1,nxtx(i),-1,data);
            }
            return;
        }
        a[d].count[x] += v;
    }

    void add(frame x){
	HashRet y = f(x);
	HashRet z = g(x);
	for(int i=0; i<k;++i){
		modify(0,y.ret[i],z.ret[i],x);
	}
}

};
#undef nxtx(i)