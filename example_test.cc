#include <iostream>
#include <set>
#include <memory>

using namespace std;


class Person{
public:
    Person(int x)
        :age(x)
    {}
    int age;
};


typedef std::shared_ptr<Person> PersonPtr;

typedef std::pair<int,PersonPtr> Entry;
typedef std::set<Entry> TimerList;



//这个是最小堆，从堆中取出的第一个元素就是堆中的最小的元素


int main()
{
    TimerList timer_;
    timer_.insert(Entry(1,new Person(1)));
    timer_.insert(Entry(100,new  Person(100)));
    timer_.insert(Entry(3,new Person(3)));

    int i = 0;

    for(auto iter = timer_.begin();iter != timer_.end();)
    {
        if(iter->first == 1)
        {
            cout << iter->second->age << endl;
            cout << "i = " << i << endl;
        }
        iter++;
        i++;
    }

    return 0;
}
