

using namespace std;
/*
    Count Sketch
*/
class CountSketch{
    double memory;
    int*count;
    int row_length,k;
    BOBHash32*bh;
    uint32_t Rseed;
    int n;
public:
    CountSketch(double _mem,int _k,uint32_t _Rseed):memory(_mem),k(_k),Rseed(_Rseed){
        bh = new BOBHash32[k*2];
        double mem = memory - sizeof(CountSketch) - sizeof(BOBHash32)*k*2;
        srand(Rseed);
        for(int i=0; i<k*2;++i)
            bh[i].initialize(rand()%7 + 7*i);
        row_length = mem / 4;
        count = new int[row_length];
        memset(count,0,sizeof(int) * row_length);
    }
    void Insert(Data x){
        for(int i=0;i<k;++i){
            int fx = bh[i].run((const char*)&x,DATA_LEN) % row_length;
            int gx = bh[i+k].run((const char*)&x,DATA_LEN) & 1 ? 1 : -1;
            count[fx] += gx;
        }
        return;
    }
    int Query(Data x){
        int q[k];
        for(int i=0;i<k;++i){
            int fx = bh[i].run((const char*)&x,DATA_LEN) % row_length;
            int gx = bh[i+k].run((const char*)&x,DATA_LEN) & 1 ? 1 : -1;
            q[i] = count[fx] * gx;
        }
        sort(q,q+k);
        return q[k/2];
    }
};