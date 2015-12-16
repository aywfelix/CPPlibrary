#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
using namespace std;

typedef unsigned int uint32_t;
uint32_t DecodeFixed32(const char* data)
{
	uint32_t res;
	memcpy(&res, data, sizeof(res));
	return res;
}
uint32_t Hash(const char* data, uint32_t n, uint32_t seed)
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
	char value[1024];
	uint32_t hash;
	char key[100];
	uint32_t keyLength;
	uint32_t refs;
	Node* next;
	Node* pre;
	Node* nextList;
};

class CCache
{
public:
    CCache();
	CCache(int capacity);
    virtual ~CCache();

	Node* find(const char* key);
    void remove(const char* key, void* value=NULL);
	void insert(Node *node);
	void resize();
private:
	int capacity;
	Node *nodeTable;
};

CCache::CCache():capacity(1024)
{
	nodeTable = calloc(capacity, sizeof(Node));
}

CCache::CCache(int capacity):capacity(capacity)
{
	nodeTable = calloc(capacity, sizeof(Node));
}

CCache::~CCache()
{
	free(nodeTable);
	nodeTable = NULL;
}

Node* CCache::find(const char* key)
{
	assert(key != NULL);
	uint32_t pos = Hash(key, strlen(key), 0)%capacity;
    Node node = nodeTable[pos];
	Node tmp = node;
	int n = 0 ;
	while(tmp != NULL)
	{
		++n;
		tmp = tmp->nextList;
	}
	Node* nodes = calloc(n, sizeof(Node));
	int i =0;
	while(node != NULL)
	{
		nodes[i] = node;
		node = node->nextList;
		i++;
	}
	return nodes;
}

void CCache::remove(const char* key, void* value=NULL)
{
    if(key && value ==NULL)
	{
		uint32_t pos = Hash(key, strlen(key), 0)%capacity;

		if(nodeTable[pos] != NULL)
		{
			Node* node = nodeTable[pos];
			while(node != NULL)
			{
				Node* tmp = node;
				free(tmp);
				node = node->nextList;
			}
		}
	}
	else{
        uint32_t pos = Hash(key, strlen(key), 0)%capacity;
		if(nodeTable[pos] != NULL)
		{
			Node* node = nodeTable[pos];
			while(node!=NULL)
			{
				Node* tmp = node;
				if(strcmp((char*)value, (char*)tmp->value) == 0)
				{
					free(tmp);					
				}
			}
		}
	}
}

void CCache::insert(Node* node)
{
	uint32_t pos = Hash(node->key, strlen(node->key), 0)%capacity;
	Node* nodes = nodeTable[pos];
	if(nodes == NULL)
		nodes = node;
	else
	{
		while(nodes)
		{
			nodes = nodes->nextList;
		}
		nodes->nextList = node;
	}	
}







