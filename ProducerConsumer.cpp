class Queue
{
	vector<int> queue;
	int start, end;
	int size;
	mutex queueMu;
	condition_variable cv;

public:
	Queue(int n) : size(0), start(0), end(-1) {
		queue.resize(n);
	}

	Enqueue(int data) {
		unique_lock<mutex> locker(queueMu);
		cv.wait(locker, [this]() { return !IsFull(); });

		end = (end + 1) % queue.size();
		queue[end] = data;
		size ++;

		locker.unlock();
		cv.notify_all();
	}

	int Dequeue() {
		unique_lock<mutex> locker(queueMu);
		cv.wait(locker, [this]() { return !IsEmpty(); });

		int ret = queue[start++];
		size --;

		locker.unlock();
		cv.notify_all();
		return ret;
	}

	bool IsFull() {
		return size == queue.size();
	}

	bool IsEmpty() {
		return size == 0;
	}
}