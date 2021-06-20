#include<iostream>
#include<functional>

class Test
{
public:
    void print()
    {
        std::cout<<1<<std::endl;
    }
    void printAddr()
    {
        std::cout<<&Test::print<<std::endl;
    }

};

int main()
{
    Test inst;
    std::cout<<&Test::print<<std::endl;
    //auto callback=std::bind(&Test::print,&inst,3);
    //callback();
}