//
// Created by 裴沛东 on 2022/5/14.
//

#ifndef CHAR_NOLOCKQUEUE_H
#define CHAR_NOLOCKQUEUE_H
#include <atomic>
#include <queue>
#include <memory>
#include <iostream>
template<typename T>
struct Node {
    using SPtr =  std::shared_ptr<Node>;
    T _val;
    SPtr _next = nullptr;

};
template<typename  T>
class NoLockQueue final {
private:
    mutable std::atomic_int32_t _size;
    mutable std::atomic_bool _stop_push;
    mutable std::atomic_bool _stop_pop;
    std::shared_ptr<Node<T>> _front;
    std::shared_ptr<Node<T>> _tail;
public:
    NoLockQueue():_size(0), _stop_push(false), _stop_pop(false) {
        _front = std::make_shared<Node<T>>();
        _tail = _front;
    }
    bool push(const T &val) {
        if(_stop_push) return false;
        typename Node<T>::SPtr last = nullptr;
        typename Node<T>::SPtr new_node = std::make_shared<Node<T>>();
        new_node->_val = val;
        last = atomic_load(&_tail);
        //CAS compare and swap
        // 对比期望值和真实值是否相等
        // 若期望值和真实值不相等返回false,将期望值设置为真实值
        // 若期望值和真实值相等交换期望值和设定值返回true
        //1.判断真实_tail是否等于期望last 等于：交换成功_tail == new_node,
        //2.                          不等于：last = 真实_tail,继续尝试交换
        while(!std::atomic_compare_exchange_strong(&_tail,&last,new_node));
        //3.交换成功 _tail = new_node
        //4.last->next = new_node;
        std::atomic_store(&(last->_next),new_node); //
        ++_size;
        return true;
    }
    bool push(T &&val) {
        if(_stop_push) return false;
        typename Node<T>::SPtr last = nullptr;
        typename Node<T>::SPtr new_node = std::make_shared<Node<T>>();
        new_node->_val = std::move(val);
        last = atomic_load(&_tail);
        //CAS compare and swap
        // 对比期望值和真实值是否相等
        // 若期望值和真实值不相等返回false,将期望值设置为真实值
        // 若期望值和真实值相等交换期望值和设定值返回true
        while(!std::atomic_compare_exchange_strong(&_tail,&last,new_node));
        std::atomic_load(&(last->_next),new_node);
        ++_size;
        return true;
    }
    bool pop(T &val) {
        if(_stop_pop) return false;
        typename Node<T>::SPtr first = nullptr;
        typename Node<T>::SPtr first_next = nullptr;

        do {
            first = std::atomic_load(&_front);
            first_next = std::atomic_load(&(_front->_next));
            if(!first_next) return false;
            //CAS compare and swap
            // 对比期望值和真实值是否相等
            // 若期望值和真实值不相等返回false,将期望值设置为真实值
            // 若期望值和真实值相等交换期望值和设定值返回true
        } while (!std::atomic_compare_exchange_strong(&_front,&first,first_next));
        --_size;
        val = std::move(first_next->_val);
        return true;
    }

    void show() {
        for(auto it = _front;it != nullptr;it = it->_next ) {
            std::cout<<it->_val<<" ";
        }
        std::cout<<std::endl;

    }

    int size() {
        return _size;
    }

    bool empty() {
        return _size == 0;
    }



};


#endif //CHAR_NOLOCKQUEUE_H
