//
// Created by zahar on 03.03.2018.
//

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <condition_variable>

template <typename T>
class threadsafe_queue
{
private:
    std::mutex mutex;
    std::queue <T> data_queue;
    std::condition_variable data_c;
public:
    threadsafe_queue() //  конструктор по умолчанию
    {
        std::cout << "ordinary";
    }
    threadsafe_queue(const threadsafe_queue &obj) // конструктор копирования
    {
        std::cout << "copy";
        std::lock_guard <std::mutex>lk(mutex);
        data_queue =obj.data_queue;
    }

    void push(T new_value)
    {
        std::lock_guard <std::mutex>lk(mutex);
        data_queue.push(new_value);
        data_c.notify_one();
    }
    void wait_and_pop(T & value)
    {
        std::unique_lock < std::mutex > lk(mutex);
        data_c.wait(lk,[this]{ return !data_queue.empty();});
        value=data_queue.front();
        data_queue.pop();
    }
    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock < std::mutex > lk(mutex);
        data_c.wait(lk,[this]{ return !data_queue.empty();});
        std::shared_ptr <T> ptr(new T(data_queue.pop()));
    }
    bool try_pop(T & value)
    {
        std::unique_lock < std::mutex > lk(mutex);
        if (data_queue.empty())
        {
            data_queue.pop();
        } else{
            std::cout<<"Sorry, I can't do it"<<std::endl;
        }
    }
    std::shared_ptr<T>try_pop()
    {
        std::unique_lock < std::mutex > lk(mutex);
        if (data_queue.empty())
        {
            std::shared_ptr <T> ptr(new T(data_queue.pop()));
        } else{
            std::cout<<"Sorry, I can't do it"<<std::endl;
        }
    }
    bool empty() const
    {
        std::unique_lock < std::mutex > lk(mutex);
        return data_queue.empty();
    }

    int size()
    {
        std::unique_lock < std::mutex > lk(mutex);
        return data_queue.size();
    }

};

struct data_v {};

std::mutex m;
threadsafe_queue <data_v> data_q;
std::condition_variable data_c;

data_v prepare_data()
{
    return data_v();
}

void data_preparation()
{
    /*/data_v data = prepare_data();
    std::lock_guard<std::mutex> lk(m);
    data_q.push(data);
    data_c.notify_one();/*/
    data_v data = prepare_data();
    data_q.push(data);
}

void data_processing()
{
    /*/std::unique_lock<std::mutex> lk(m);
    data_c.wait(lk,[]{return !data_q.empty(); });
    data_v data = data_q.front();
    data_q.pop();
    lk.unlock();/*/
    data_v data;
    data_q.wait_and_pop(data);

    data_q.try_pop(data);
}

int main(int argc, char ** argv) {
    std::thread t1(data_preparation);
    std::thread t2(data_processing);

    t1.join();
    t2.join();


    //lambda
    //[](){};
    // последние скобки - выполняемое выражение,
    // вторые - переменные,
    // первые - синтаксис захвата переменных из вне (сама переменная, по ссылке (&) по равенству (=)
    //пример auto square = [==max](int x){return x*x}
}


