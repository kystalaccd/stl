#include "functional.hpp"
#include <random>
#include <iostream>
#include <functional>

namespace{

std::random_device rd;
std::default_random_engine de(rd());
std::uniform_int_distribution<> di{-1000, 1000};

int sum(int a, int b){
    return a+b;
}


class Obj{
public:
    virtual int multiple(const int &a, const int &b){
        return a*b;
    }

    static int minus(int a, int b){
        return a-b;
    }
};

}

bool test_global_func(){
    stl::function<int(int,int)> f(sum);
    for(int i=0; i<10000; ++i){
        auto a = di(de), b=di(de);
        if(f(a,b) != sum(a,b))
            return false;
    }

    return true;
}

bool test_member_func(){
    Obj tmp;
    stl::function<int(Obj *, const int&, const int&)> f(&Obj::multiple);
    for(int i=0; i<10000; ++i){
        auto a = di(de), b=di(de);
        if(f(&tmp,a,b) != tmp.multiple(a,b))
            return false;
    }
    return true;
}

bool test_bind(){
    Obj tmp;
    stl::function<int(const int&, const int&)> f(std::bind(&Obj::multiple, &tmp, std::placeholders::_1, std::placeholders::_2));
    for(int i=0; i<10000; ++i){
        auto a = di(de), b=di(de);
        if(f(a,b) != tmp.multiple(a,b))
            return false;
    }
    return true;
}

bool test_static_member_func(){
    stl::function<int(int,int)> f(&Obj::minus);
    for(int i=0; i<10000; ++i){
        auto a = di(de), b=di(de);
        if(f(a,b) != Obj::minus(a,b))
            return false;
    }

    return true;
}

bool test_lambda(){
    long long i=1,j=2,k=3,w=4,y=5,z=6;
    auto divide = [i,j,k,w,y,z](int a, int b)mutable->int{
        return a/b;
    };

    stl::function<int(int,int)> f(divide);
    
    for(int i=0; i<10000; ++i){
        int a = di(de), b;
        do{
            b = di(de);
        }while(!b);

        if(f(a,b) != divide(a,b))
            return false;
    }

    return true;
}

bool test_assign_copy(){
    stl::function<int(int,int)> f;
    f = sum;
    if(f(1,2) == sum(1,2))
        return true;
    return false;
}

bool test_relations_ship(){
    stl::function<void(float,int)> f;
    if(f == nullptr)
        return true;
    return false;
}

int main(int argc, char *argv[]){
    std::cout<<"--------------test global function start--------------"<<std::endl;
    std::cout<<(test_global_func()?"pass.":"wrong.")<<std::endl;
    std::cout<<"---------------test global function end---------------"<<std::endl<<std::endl;

    std::cout<<"--------------test member function start--------------"<<std::endl;
    std::cout<<(test_member_func()?"pass":"wrong")<<std::endl;
    std::cout<<"---------------test member function end---------------"<<std::endl<<std::endl;

    std::cout<<"--------------test bind start--------------"<<std::endl;
    std::cout<<(test_bind()?"pass":"wrong")<<std::endl;
    std::cout<<"---------------test bind end---------------"<<std::endl<<std::endl;

    std::cout<<"--------------test static member function start--------------"<<std::endl;
    std::cout<<(test_static_member_func()?"pass.":"wrong")<<std::endl;
    std::cout<<"---------------test static member function end---------------"<<std::endl<<std::endl;

    std::cout<<"--------------test lambda start--------------"<<std::endl;
    std::cout<<(test_lambda()?"pass.":"wrong")<<std::endl;
    std::cout<<"---------------test lambda end---------------"<<std::endl<<std::endl;

    std::cout<<"--------------test assgin copy start--------------"<<std::endl;
    std::cout<<(test_assign_copy()?"pass.":"wrong")<<std::endl;
    std::cout<<"--------------test assgin copy start--------------"<<std::endl<<std::endl;

    std::cout<<"--------------test relations ship start--------------"<<std::endl;
    std::cout<<(test_relations_ship()?"pass.":"wrong")<<std::endl;
    std::cout<<"--------------test relations ship start--------------"<<std::endl<<std::endl;

    std::cout<<"All Pass!"<<std::endl;
    return 0;
}