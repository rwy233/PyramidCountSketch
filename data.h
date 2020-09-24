#define DATA_LEN 13
#define TIME_LEN 8

class Data{
public:
    unsigned char str[DATA_LEN];
    Data& operator = (Data an);
};
class TimeStamp{
public:
    unsigned char str[TIME_LEN];
};
class Packet{
public:
    Data data;
    TimeStamp timestamp;
};

bool operator < (Data bn, Data an);
bool operator == (Data bn, Data an);

// class My_Hash{
// public:
//     size_t operator()(const Data dat) const{
//         return RSHash(dat.str, DATA_LEN);
//     }
// };
Data& Data::operator = (Data an){
    for(int i = 0;i < DATA_LEN;++i){
        str[i] = an.str[i];
    }
    return *this;
}

bool operator < (Data bn, Data an){
    for(int i = 0;i < DATA_LEN;++i){
        if(bn.str[i] < an.str[i])
            return true;
        else if(bn.str[i] > an.str[i])
            return false;
    }
    return false;
}

bool operator == (Data bn, Data an){
    for(int i = 0;i < DATA_LEN;++i){
        if(bn.str[i] != an.str[i])
            return false;
    }
    return true;
}

struct ele{
	Data data;
	int x;
};

struct HashRet{
	int*ret;
    HashRet(int k){
        ret = new int[k];
    }
};