#pragma once

#include <iostream>
#include <string>
using namespace std;



struct link {
	int type;
	void *data;
	link *next;
	link *prev;

	link() {
		data = nullptr;
		next = nullptr;
		prev = nullptr;
		type = 0;
	}

	link(link *cpy) {
		data = cpy->data;
		next = nullptr;
		prev = nullptr;
	}

	link(void *dt, int ty) {
		data = dt;
		type = ty;
	}

	void *getData() {
		return data;
	}

	void setData(void *dt) {
		data = dt;
	}
};

struct linked_list {
	link *first;

	linked_list(link *elem) {
		insert(elem);
	}

	linked_list() {
		first = nullptr;
	}

	bool isEmpty() {
		return(first == nullptr);
	}

	int size() {
		if (isEmpty()) return 0;
		link *cur = first;
		int count = 1;
		while (cur->next != nullptr) {
			cur = cur->next;
			count++;
		}
		return count;
	}

	link *getLink(int linkNum) {
		if (size() < linkNum) {
			return nullptr;
		}
		link *node = first;
		for (int i = 1; i < size(); i++) {
			node = node->next;
			if (node == nullptr || i == linkNum) break;
		}
		return node;
	}

	bool remove(link *key) {
		link *old = first;
		while (old != nullptr) {
			if (old == key) break;
			old = old->next;
		}
		if (old == nullptr || key == nullptr || isEmpty()) {
			return false;
		}

		link *oldPrev = old->prev;
		link *oldNext = old->next;
		if (old == first) {	// step forward
			first = first->next;
			if (first != nullptr) first->prev = nullptr;
			return true;
		} else {
			if (oldPrev != nullptr) oldPrev->next = oldNext;
			if (oldNext != nullptr) oldNext->prev = oldPrev;
		}
		old->setData(nullptr);
		old = nullptr;
		return true;
	}

	bool insert(link *insert) {
		if (isEmpty()) {
			insert->prev = nullptr;
			insert->next = nullptr;
			first = insert;
			return true;
		}
		link *it = first;
		while (it->next != nullptr) {
			it = it->next;
		}
		it->next = insert;
		insert->prev = it;
		insert->next = nullptr;
		return true;
	}
};