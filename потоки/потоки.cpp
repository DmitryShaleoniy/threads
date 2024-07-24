#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <chrono>

std::mutex mtx;
std::condition_variable cv;
std::queue<int> tasks;

void AddTask(int task) {
	mtx.lock();
	std::cout << "starting adding tasks..." << std::endl;
	mtx.unlock();
	for (int i = 0; i < task; i++) {
		mtx.lock();
		tasks.push(i);
		std::cout << std::this_thread::get_id << " " << "task " << i << " added into queue!" << std::endl;
		cv.notify_one();
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	std::lock_guard<std::mutex> lock(mtx);
	
}

void ProsessTasks() {
	while (true) {
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, []() {return !tasks.empty(); });
		if (tasks.empty()) {
			break;
		}
		std::cout << std::this_thread::get_id << " " << "task " << tasks.front() << " is being proseded" << std::endl;
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(1500));
		lock.lock();
		std::cout << std::this_thread::get_id << " " << "task " << tasks.front() << " completed" << std::endl;
		tasks.pop();
		lock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
	}
}

int main() {
	std::thread do_task(ProsessTasks);
	std::thread add(AddTask, 5);

	for (int i = 0; i < 27; i++) {
		mtx.lock();
		std::cout << "thread main " << std::this_thread::get_id << " " << i << std::endl;
		mtx.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(350));
	}

	add.join();
	do_task.join();
}