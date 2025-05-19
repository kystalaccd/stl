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
    void get_random(int &val){
        val = di(de);
    }

    static int minus(int a, int b){
        return a-b;
    }
};

}

bool test_global_func(){
    stl::function<int(int,int)> f(sum);
    for(int i=0; i<1000; ++i){
        auto a = di(de), b=di(de);
        if(f(a,b) != sum(a,b))
            return false;
    }

    return true;
}

#if 0
bool test_member_func(){    //暂不支持成员函数
    Obj tmp;
    int val;
    stl::function<void(Obj &, int&)> f(&Obj::get_random);
    f(tmp, val);
    std::cout<<val<<std::endl;
    return true;
}

bool test_member_func2(){
    Obj tmp;
    int val;
    stl::function<void(int&)> f(std::bind(&Obj::get_random, &tmp));
    f(val);
    std::cout<<val<<std::endl;
    return true;
}
#endif

bool test_static_member_func(){
    stl::function<int(int,int)> f(&Obj::minus);
    for(int i=0; i<1000; ++i){
        auto a = di(de), b=di(de);
        if(f(a,b) != Obj::minus(a,b))
            return false;
    }

    return true;
}

bool test_lambda(){
    auto divide = [](int a, int b)->int{
        return a/b;
    };

    stl::function<int(int,int)> f(divide);
    
    for(int i=0; i<1000; ++i){
        int a = di(de), b;
        do{
            b = di(de);
        }while(!b);

        if(f(a,b) != divide(a,b))
            return false;
    }

    return true;
}

int main(int argc, char *argv[]){
    std::cout<<"--------------test global function start--------------"<<std::endl;
    std::cout<<(test_global_func()?"pass.":"wrong.")<<std::endl;
    std::cout<<"---------------test global function end---------------"<<std::endl;

#if 0
    std::cout<<"--------------test member function start--------------"<<std::endl;
    std::cout<<(test_member_func()?"pass":"wrong")<<std::endl;
    std::cout<<"---------------test member function end---------------"<<std::endl;
#endif

    std::cout<<"--------------test static member function start--------------"<<std::endl;
    std::cout<<(test_static_member_func()?"pass.":"wrong")<<std::endl;
    std::cout<<"---------------test static member function end---------------"<<std::endl;

    std::cout<<"--------------test lambda start--------------"<<std::endl;
    std::cout<<(test_lambda()?"pass.":"wrong")<<std::endl;
    std::cout<<"---------------test lambda end---------------"<<std::endl;

    return 0;
}