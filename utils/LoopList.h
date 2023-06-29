#pragma once
//#include <concepts>
template<typename T, unsigned int N>
//	requires requires(T a) {
//		{ bool(a.busy) };
//	}
struct LoopList {
	static constexpr int M = N;
	T arr[N];
	unsigned int head, tail;
	bool full() {
		return head == tail && arr[head].busy;
	}
	bool empty() const {
		return head == tail && !arr[head].busy;
	}
	void push(T x) {
		arr[tail++] = x;
		if (tail == N) tail = 0;
	}
	void pop() {
		arr[head].busy = false;
		if (++head == N) head = 0;
		while (head != tail && !arr[head].busy) {
			if (++head == N) head = 0;
		}
	}
	T &operator[](int x) { return arr[x]; }
	void clear() {
		head = tail = 0;
		for (auto &x: arr) x.busy = false;
	}
	[[nodiscard]] constexpr static int max_size() { return N; }
};
