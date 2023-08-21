#ifndef BLIB_TEST_H
#define BLIB_TEST_H

#include <string>
#include <iostream>
#include "single_tq_tp.h"
#include "multi_tq_tp.h"

namespace blib {


    void test_thread_pool() {
        std::cout << "test_thread_pool()" << std::endl;
        blib::ThreadPool threadPool;
        threadPool.enqueue([] { std::cout << "hello\n"; });
        auto future = threadPool.enqueue([](std::string str) { return "hello" + str; }, "world");
        std::cout << future.get() << std::endl;
    }
    
    std::string funA(std::string str) {
        return "hello" + str;
    }
    
    void test_simple_thread_pool() {
        std::cout << "test_simple_thread_pool()" << std::endl;
        blib::SimplePool threadPool;
        threadPool.enqueue([] { std::cout << "hello\n"; });
        // * 此处必须使用shared_ptr进行包装，
        // * 否则在std::function<void()>中会尝试生成std::packaged_task的拷贝构造函数，
        // ! std::packaged_task禁止拷贝操作
        auto task = std::make_shared<std::packaged_task<std::string()>>(std::bind(funA, "world"));
        std::future<std::string> res = task->get_future();
        threadPool.enqueue([task = std::move(task)] { (*task)(); });
        // ! 以下实现方法是错误的
        //  auto task = std::packaged_task<std::string()>(std::bind(funA, "world"));
        //  std::future<std::string> res = task.get_future();
        //  threadPool.enqueue(std::move(task));
        std::cout << res.get() << std::endl;
    }
    
    void test_multiple_thread_pool() {
        std::cout << "test_multiple_thread_pool" << std::endl;
        blib::MultiplePool threadPool;
        threadPool.schedule_by_id([] { std::cout << "hello\n"; });
        auto task = std::make_shared<std::packaged_task<std::string()>>(std::bind(funA, "world"));
        std::future<std::string> res = task->get_future();
        threadPool.schedule_by_id([task = std::move(task)] { (*task)(); });
        std::cout << res.get() << std::endl;
    }
}
#endif //BLIB_TEST_H
