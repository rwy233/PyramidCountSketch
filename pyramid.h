
struct Count{
	int*count;
    bool*up;
	int bitnum;
	int n,innerk;
	BOBHash32*bh;
	void init(int _bitnum,int _n,int _innerk){
		n = _n;
        innerk = _innerk;
		count = new int[n];
		bitnum = _bitnum;
		memset(count,0,sizeof(int) * n);
        bh = new BOBHash32[innerk];
		for (int i=0; i<innerk ;++i){
			bh[i].initialize( rand()%7 + 7*i);
		}	
	}
    void init_up(int _bitnum,int _n,int _innerk){
        init(_bitnum,_n,_innerk);
        up = new bool[n];
        memset(up,0,sizeof(bool) * n);
    }
};
/*
    不进位 不捆绑
*/
 // 4 8 16 32
class PyramidSketch4{
    double memory;
    double alpha;
    int k,innerk;
    BOBHash32*bh;
    // int step_num;
    Count**a;
    uint32_t Rseed;
    const int step_num = 4;
public:
    PyramidSketch4(int _mem,double _alp,int _k,int _innerk,uint32_t _Rseed)
    :memory(_mem),alpha(_alp),k(_k),innerk(_innerk),Rseed(_Rseed){
        srand(Rseed);
        
        bh = new BOBHash32[k*2];
        for(int i=0;i<k*2;++i) bh[i].initialize( rand()%10 + 10*i );

        a = new Count* [k];
        for(int pyi=0;pyi<k;++pyi) {
            a[pyi] = new Count[step_num];

            int each_bit = (memory * 8) *(1-alpha) / (1 - pow(alpha,step_num)) / k;
            int bitnum = 4;
            for(int i=0;i<step_num;++i){
                a[pyi][i].init(bitnum, each_bit/bitnum,innerk);
                bitnum *= 2;
                each_bit = each_bit * alpha;
            }
        }
    }

    HashRet f(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i){
            ret.ret[i] = bh[i].run((const char*)&x, DATA_LEN) % a[i][0].n;
        }
        return ret;
    }

    HashRet g(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i)
            ret.ret[i] = (bh[i + k].run((const char*)&x, DATA_LEN) & 1) ? 1 : -1;
        return ret;
    }

#define nxtx(i) (A.bh[i].run((const char*)&tmp,sizeof(tmp)) %  a[pyi][d+1].n)

    int query(int d,int x,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return A.count[x]; 
        if(A.count[x] + 1 == (1<<A.bitnum-1) || A.count[x] - 1 == -(1<<A.bitnum-1) ) {
            ele tmp = (ele){data,x};
            int q[k];
            for(int i=0;i<innerk;++i) q[i] = query(d+1,nxtx(i),data,pyi);
            sort(q,q+innerk);
            return q[innerk/2] + A.count[x];
        }
        return A.count[x];
    }

    void modify(int d,int x,int v,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return;
        if(A.count[x] + 1 == (1<<A.bitnum-1)){
            ele tmp = (ele){data,x};
            for (int i=0; i<innerk; ++i){
                modify(d+1,nxtx(i),1,data,pyi);
            }
            return;
        }
        if(A.count[x] - 1 == -(1<<A.bitnum-1)){
            ele tmp = (ele){data,x};
            for (int i=0; i<innerk; ++i){
                modify(d+1,nxtx(i),-1,data,pyi);
            }
            return;
        }
        A.count[x] += v;
    }

    void Insert(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        for(int i=0; i<k;++i){
            modify(0,y.ret[i],z.ret[i],x,i);
        }
    }

    int Query(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        vector<int>q;
        for(int i=0; i<k;++i){
            q.push_back ( query(0,y.ret[i],x,i) * z.ret[i] ); 
            
        }
        sort(q.begin(),q.end());
        return q[k/2];
    }
#undef nxtx
};
/*
    不进位 捆绑
*/
class PyramidSketch3{
    double memory;
    double alpha;
    int k,innerk;
    BOBHash32*bh;
    // int step_num;
    Count**a;
    uint32_t Rseed;
    const int step_num = 4;
public:
    PyramidSketch3(int _mem,double _alp,int _k,int _innerk,uint32_t _Rseed)
    :memory(_mem),alpha(_alp),k(_k),innerk(_innerk),Rseed(_Rseed){
        srand(Rseed);
        
        bh = new BOBHash32[k*2];
        for(int i=0;i<k*2;++i) bh[i].initialize( rand()%10 + 10*i );

        a = new Count* [k];
        for(int pyi=0;pyi<k;++pyi) {
            a[pyi] = new Count[step_num];

            int each_bit = (memory * 8) *(1-alpha) / (1 - pow(alpha,step_num)) / k;
            int bitnum = 4;
            for(int i=0;i<step_num;++i){
                a[pyi][i].init(bitnum, each_bit/bitnum,innerk);
                bitnum *= 2;
                each_bit = each_bit * alpha;
            }
        }
    }

    HashRet f(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i){
            ret.ret[i] = bh[i].run((const char*)&x, DATA_LEN) % a[i][0].n;
        }
        return ret;
    }

    HashRet g(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i)
            ret.ret[i] = (bh[i + k].run((const char*)&x, DATA_LEN) & 1) ? 1 : -1;
        return ret;
    }

    int query(int d,int x,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return A.count[x]; 
        if(A.count[x] + 1 == (1<<A.bitnum-1) || A.count[x] - 1== -(1<<A.bitnum-1) ) {

            int q[k];
            for(int i=0;i<innerk;++i) q[i] = query(d+1,x % a[pyi][d+1].n,data,pyi);
            sort(q,q+innerk);
            return q[innerk/2] + A.count[x];
        }
        return A.count[x];
    }

    void modify(int d,int x,int v,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return;
        if(A.count[x] + 1 == (1<<A.bitnum-1)){

            for (int i=0; i<innerk; ++i){
                modify(d+1,x % a[pyi][d+1].n,1,data,pyi);
            }
            return;
        }
        if(A.count[x] - 1 == -(1<<A.bitnum-1)){

            for (int i=0; i<innerk; ++i){
                modify(d+1,x % a[pyi][d+1].n,-1,data,pyi);
            }
            return;
        }
        A.count[x] += v;
    }

    void Insert(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        for(int i=0; i<k;++i){
            modify(0,y.ret[i],z.ret[i],x,i);
        }
    }

    int Query(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        vector<int>q;
        for(int i=0; i<k;++i){
            q.push_back ( query(0,y.ret[i],x,i) * z.ret[i] ); 
            
        }
        sort(q.begin(),q.end());
        return q[k/2];
    }

};
/*
    进位 捆绑
*/
class PyramidSketch1{
    double memory;
    double alpha;
    int k,innerk;
    BOBHash32*bh;
    Count**a;
    uint32_t Rseed;
    const int step_num = 8;
    const int BITN[8] = {8,4,4,4,4,4,4,2};
public:
    PyramidSketch1(int _mem,double _alp,int _k,int _innerk,uint32_t _Rseed)
    :memory(_mem),alpha(_alp),k(_k),innerk(_innerk),Rseed(_Rseed){
        srand(Rseed);
        
        bh = new BOBHash32[k*2];
        for(int i=0;i<k*2;++i) bh[i].initialize( rand()%10 + 10*i );

        a = new Count* [k];
        for(int pyi=0;pyi<k;++pyi) {
            a[pyi] = new Count[step_num];

            int each_bit = (memory * 8) *(1-alpha) / (1 - pow(alpha,step_num)) / k;
            int bitnum = 8;
            for(int i=0;i<step_num;++i){
                bitnum = BITN[i];
                a[pyi][i].init_up(bitnum, each_bit/(bitnum+1),innerk); // 标记位
                // bitnum *= 2;
                each_bit = each_bit * alpha;
            }
        }
    }

    HashRet f(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i){
            ret.ret[i] = bh[i].run((const char*)&x, DATA_LEN) % a[i][0].n;
        }
        return ret;
    }

    HashRet g(const Data&x){
        HashRet ret(k);
        for (int i=0; i<k; ++i)
            ret.ret[i] = (bh[i + k].run((const char*)&x, DATA_LEN) & 1) ? 1 : -1;
        return ret;
    }

    int query(int d,int x,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return A.count[x]; 
        if(A.up[x] ) {
            int q[k];
            for(int i=0;i<innerk;++i) q[i] = query(d+1,x % a[pyi][d+1].n,data,pyi);
            sort(q,q+innerk);
            return q[innerk/2] * (1<<A.bitnum-1)  + A.count[x];
        }
        return A.count[x];
    }

    void modify(int d,int x,int v,const Data&data,int pyi){
        Count&A = a[pyi][d];
        if( d == step_num ) return;
        A.count[x] += v;
        if(A.count[x] == (1<<A.bitnum-1)){
            A.count[x] = 0;
            A.up[x] = 1;
            for (int i=0; i<innerk; ++i){
                modify(d+1,x % a[pyi][d+1].n,1,data,pyi);
            }
            return;
        }
        if(A.count[x] == -(1<<A.bitnum-1)){
            A.count[x] = 0;
            A.up[x] = 1;
            for (int i=0; i<innerk; ++i){
                modify(d+1,x % a[pyi][d+1].n,-1,data,pyi);
            }
            return;
        }
        
    }

    void Insert(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        for(int i=0; i<k;++i){
            modify(0,y.ret[i],z.ret[i],x,i);
        }
    }

    int Query(Data x){
        HashRet y = f(x);
        HashRet z = g(x);
        vector<int>q;
        for(int i=0; i<k;++i){
            q.push_back ( query(0,y.ret[i],x,i) * z.ret[i] ); 
            
        }
        sort(q.begin(),q.end());
        return q[k/2];
    }

};