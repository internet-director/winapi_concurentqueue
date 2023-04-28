#include <iostream>
#include <deque>
#include <Windows.h>

template<typename T>
struct queue {
    queue() {
        mutex = CreateMutexW(NULL, FALSE, L"QUEUE");
        InitializeCriticalSection(&lock);
        InitializeConditionVariable(&cv_full);
        InitializeConditionVariable(&cv_empty);
    }

    queue(size_t size) : queue(), MAX_SIZE(size) {}

    ~queue() {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
        DeleteCriticalSection(&lock);
    }

    void push(const T& value) {
        EnterCriticalSection(&lock);

        while (que.size() >= MAX_SIZE) {
            SleepConditionVariableCS(&cv_full, &lock, INFINITE);
        }

        que.push_back(value);
        WakeConditionVariable(&cv_empty);
        LeaveCriticalSection(&lock);
    }

    T pop() {
        EnterCriticalSection(&lock);

        while (que.empty()) {
            SleepConditionVariableCS(&cv_empty, &lock, INFINITE);
        }

        T value = que.front();
        que.pop_front();
        WakeConditionVariable(&cv_full);
        LeaveCriticalSection(&lock);
        return value;
    }

private:
    size_t MAX_SIZE = 10000;
    std::deque<T> que;
    HANDLE mutex = NULL;
    CRITICAL_SECTION lock;
    CONDITION_VARIABLE cv_full, cv_empty;
};
