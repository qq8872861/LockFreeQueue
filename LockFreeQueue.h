#pragma once
#include <atomic>


////////////////////////////只支持一个线程push，一个线程pop////////////////////////////////////
template <typename T>
class LockFreeQueue
{
private:
	struct Node
	{
		T data;
		Node* next;

		Node(const T& value) : data(value), next(nullptr) {}
	};

	std::atomic<Node*> head;
	std::atomic<Node*> tail;
	std::atomic<unsigned int> size;
	unsigned int maxSize;

public:
	LockFreeQueue() : head(nullptr), tail(nullptr)
	{
		T data;
		head = tail = new Node(data);
		size = 0;
		maxSize = 50000;
	}

	~LockFreeQueue()
	{
		while (Node* node = head.load()) {
			head.store(node->next);
			delete node;
		}
	}

	void set_max_size(unsigned int max)
	{
		maxSize = max;
	}

	bool push(const T& value)
	{
		if (size.load() >= maxSize)
			return false;

		Node* newNode = new Node(value);

		tail.load()->next = newNode;
		tail.store(newNode);
		++size;

		return true;
	}

	bool pop(T& value)
	{
		if (head.load() == tail.load())
			return false;

		Node* pTmpHead = head.load();
		Node* pNewHead = pTmpHead->next;
		head.store(pNewHead);
		value = pNewHead->data;

		delete pTmpHead;

		--size;
		return true;
	}

	unsigned int get_current_size()
	{
		return size;
	}
};

