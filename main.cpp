#include <iostream>
#include <vector>
#include <thread>
#include "NoLockQueue.h"
void AlterPrint() {
    int sum = 0;
    std::atomic_bool flag = false;
    std::thread t1([&] () {
        while(true) {
            if (!flag.load()) {
                if(sum == 100) {
                    flag.store(true);
                    break;
                }
                ++sum;
                std::cout << "thread1 " << sum << std::endl;
                flag.store(true);
            }
        }

    });
    std::thread t2([&] () {
        while (true) {
            if (flag.load()) {
                if (sum == 100) {
                    flag.store(false);
                    break;
                }
                ++sum;
                std::cout << "thread2 " << sum << std::endl;
                flag.store(false);
            }
        }
    });
    t1.join();
    t2.join();

}
int main() {
    AlterPrint();
    NoLockQueue<int> freelockque;
    std::vector<std::thread> threadvec;
    for(auto i = 0;i<10;++i) {
        threadvec.emplace_back(std::thread([&]() {
            for (int i = 0; i < 10; ++i) {
                freelockque.push(i);
            }
        }));
    }
    for(auto &i:threadvec) {
        i.join();
    }
    freelockque.show();
    int u;
    freelockque.pop(u);
    std::cout<<u<<std::endl;
    freelockque.pop(u);
    std::cout<<u<<std::endl;
    freelockque.pop(u);
    std::cout<<u<<std::endl;
    std::cout<<"queue size is"<<freelockque.size();
    freelockque.show();


    return 0;
}
