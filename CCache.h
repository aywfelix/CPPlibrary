#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pthread.h>
using namespace std;

typedef unsigned int uint32_t;
uint32_t DecodeFixed32(const char* data)
{
	uint32_t res;
	memcpy(&res, data, sizeof(res));
	return res;
}
uint32_t Hash(const char* data, uint32_t n, uint32_t seed=0)
{
	const uint32_t m = 0xc6a4a793;
	const uint32_t r = 24;
	const char* limit = data + n;
	uint32_t h = seed ^ (n * m);

	// Pick up four bytes at a time
	while (data + 4 <= limit) {
		uint32_t w = DecodeFixed32(data);
		data += 4;
		h += w;
		h *= m;
		h ^= (h >> 16);
	}

	// Pick up remaining bytes
	switch (limit - data) {
    case 3:
		h += data[2] << 16;
		// fall through
    case 2:
		h += data[1] << 8;
		// fall through
    case 1:
		h += data[0];
		h *= m;
		h ^= (h >> r);
		break;
	}
	return h;
}

struct Node
{
	void* value;
	Node* nextList;
	Node* next;
	Node* pre;
	uint32_t keyLen;
	uint32_t hash;
	char  keydata[1];
	uint32_t refs;
};

class HashTable
{
public:
    HashTable():elems(0),length(0),list(NULL)
	{
		resize();
	}
    virtual ~HashTable()
	{
		if(list)
			delete[] list;
	}
	Node* find(uint32_t hash, void * value)
	{
		return *findPointer(hash, value);
	}
	Node* insert(Node* node)
	{
		Node** ptr = findPointer(node->hash, node->value);
		Node* old = *ptr;
		node->nextList = (old == NULL?NULL:old->nextList);
		*ptr= node;
		if(old == NULL)
		{
			++elems;
			if(elems > length)
			{
				resize();
			}
		}
		return old;
	}
	Node* remove(uint32_t hash, void *value)
	{
		Node** ptr = findPointer(hash, value);
		Node* res = *ptr;
		if(res != NULL)
		{
			*ptr = res->nextList;
			--elems;
		}
		return res;
	}
	
	void print()
	{
		for(int i=0;i<length; ++i)
		{
			Node* ptr = list[i];
			while(ptr != NULL)
			{
				cout << "key:"<<(char*)ptr->keydata<<" "<<"value:"<<(char*)ptr->value<<endl;
				ptr = ptr->nextList;
			}
		}
	}
private:
	Node** findPointer(uint32_t hash, void* value)
	{
		Node** ptr = &list[hash & (length-1)];
		while(*ptr !=NULL)
		{
			if(strcmp((char*)(*ptr)->value,(char*)value) !=0)
			{
				ptr = &(*ptr)->nextList;
			}
			else{ break;}
		}
		return ptr;
	}

	void resize()
	{
		uint32_t newLength = 2;
		while(newLength < elems)
			newLength *= 2;
        Node** newList = new Node*[newLength];
		memset(newList, 0, sizeof(newList[0])*newLength);
		for(uint32_t i=0; i<length; ++i)
		{
			Node* node = list[i];
			while(node !=NULL)
			{
				Node *next = node->nextList;
				uint32_t hash = node->hash;
				Node** ptr = &newList[hash & (newLength-1)];	
				node->nextList = *ptr;
				*ptr = node;
				node = next;
			}
		}
		delete[] list;
		list = newList;
		length = newLength;
	}
private:
	uint32_t length;
	uint32_t elems;
	Node** list;
};

class LRUCache
{
public:
    LRUCache();
    virtual ~LRUCache();

	void setCapacity(int n){ capacity = n;}
	void insert(const char* key, void* value);
	bool search(const char* key, void* value);
	void release(Node *e);
	void erase(const char* key, void* value);
	void print()
	{
		table.print();
	}
private:
	void lru_remove(Node *e)
	{
		e->next->pre = e->pre;
		e->pre->next = e->next;
	}
	void lru_append(Node* e)
	{
		e->next = &lru;
		e->pre = lru.pre;
		e->pre->next = e;
		e->next->pre = e;
	}
	void unref(Node* e)
	{
		assert(e->refs > 0);
		e->refs--;
		if(e->refs <=0)
		{
			free(e);
		}
	}
private:
	uint32_t capacity;
	pthread_mutex_t mutex;
	Node lru;
	HashTable table;
};

LRUCache::LRUCache():capacity(0)
{
	pthread_mutex_init(&mutex, NULL);
	lru.next = &lru;
	lru.pre = &lru;
}

LRUCache::~LRUCache()
{
	for(Node* e = lru.next; e != &lru;)
	{
		Node* next = e->next;
	    unref(e);
		e = next;
	}
}

void LRUCache::insert(const char* key, void* value)
{
	pthread_mutex_lock(&mutex);
	Node *e = (Node*)malloc(sizeof(Node)-1+sizeof(key));
	e->value = value;
	e->keyLen = sizeof(key);
	e->hash = Hash(key, strlen(key));
	e->refs = 2;
	memcpy(e->keydata, key, e->keyLen);
	lru_append(e);
	Node* old = table.insert(e);
	if(old == NULL)
	{
		lru_remove(e);
		unref(e);
	}
	while(lru.next != &lru)
	{
		Node* old = lru.next;
		lru_remove(old);
		table.remove(old->hash, old->value);
		unref(old);
	}
	pthread_mutex_unlock(&mutex);
}

bool LRUCache::search(const char* key, void* value)
{
	pthread_mutex_lock(&mutex);
	uint32_t hash = Hash(key, strlen(key));
	Node *node = table.find(hash, value);
	pthread_mutex_unlock(&mutex);
	if(node!=NULL)
	{
		node->refs++;
		lru_remove(node);
		lru_append(node); //make the newest node in the list(LRU)
	}
	if(node != NULL)
		return true;
	else
		return false;
}

void LRUCache::release(Node *e)
{
	pthread_mutex_lock(&mutex);
	unref(e);
	pthread_mutex_unlock(&mutex);
}

void LRUCache::erase(const char* key, void* value)
{
	pthread_mutex_lock(&mutex);
	uint32_t hash = Hash(key, strlen(key));
	Node *node = table.remove(hash, value);
	if(node != NULL)
	{
		lru_remove(node);
		unref(node);
	}
	pthread_mutex_unlock(&mutex);
}
