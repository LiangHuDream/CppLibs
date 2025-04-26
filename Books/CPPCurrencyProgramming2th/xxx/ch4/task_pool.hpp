#pragma once

/*
    aim:
        1 priority task
        2 producer add task, consumer process task
        3 finite wait time when consumer find no data 
        4 multi-threads safe operations
*/

class TaskPool {
public:
    TaskPool(int threadNum) : stop(false) {
        for (int i = 0; i < threadNum; ++i) {
            workers.emplace([this] {
                while (true) {
                    std::unique_lock<std::mutex> lk(mtx);
                    cv.wait(lk, [this] {return stop || tasks.empty(); });
                    if (stop && tasks.empty()) {
                        return;
                    }

                    if (!tasks.empty()) {
                        Task task = std::move(tasks.top());
                        tasks.pop();
                        lk.unlock();
                        task.func();
                    }
                }
            });
        }
    } 

    ~TaskPool() {
        std::unique_lock<std::mutex> lk(mtx);
        stop = true;

        cv.notify_all();

        for (std::thread &worker : workers) {
            worker.join();
        }
    }

    TaskPool(const TaskPool&) = delete;
    TaskPool& operator=(const TaskPool&) = delete;
    TaskPool(TaskPool&&) = delete;
    TaskPool& operator=(TaskPool&&) = delete;

    template <class F, class... Args>
    auto enqueue(int priority, F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type> {
        using return_type = typename std::result_of<F(Args...)>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lk(mtx);
            if (stop) {
                std::cerr << "pool stoped" << std::endl;
                return res; // TODO
            }

            tasks.emplace(priority, [task]{(*task)();});
        }

        cv.notify_one();
        return res;
    }
private:
    struct Task {
        int priority;
        std::function<void()> func;

        bool operator()(const Task& other) const {
            return priority < other.priority;
        }
    };  

    std::vector<std::thread> workers;
    std::priority_queue<Task> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool stop;
};

