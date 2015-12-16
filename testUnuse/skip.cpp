#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <cstdlib>
#include <time.h>

#define maxLevel 12
struct Node
{
	int key;
	int value;
	Node* forward[1];
};

struct skipList
{
	int level;
	Node* head;
};

class SkipList
{
public:
    SkipList()
	{
		srand((int)time(0)); 
		slist = (skipList*)malloc(sizeof(skipList));
		slist->head = createNode(maxLevel, 0, 0);
		slist->level = 0;
		for(int i=0; i<maxLevel; ++i)
		{
			slist->head->forward[i] = NULL;
		}
	}

	int search(int key)
	{
		Node *p, *q;
		p = slist->head;
		int k = slist->level;
        for(int i=k; i>=0; --i)
		{
			while(p->forward[i] != NULL && p->forward[i]->key <= key)
			{
				if(p->forward[i]->key == key)
				{
					q = p->forward[i];
					return q->value;
				}
				p = p->forward[i];
			}
		}
	}
	bool insert(int key, int value)
	{
		Node *p, *q;
		Node *update[maxLevel];
		p = slist->head;
		int k = slist->level;
		for(int i = k; i>=0; --i)
		{
			while(p->forward[i] != NULL && p->forward[i]->key < key)
			{
				p = p->forward[i];
			}
			update[i] = p;
		}
		
		q = p->forward[0];
		if(q && q->key == key)
			return false;
		
		k = random();
		if(k > slist->level)
		{
			for(int i=slist->level; i<=k; ++i)
			{
				update[i] = slist->head;
			}
			slist->level = k;
		}

		q = createNode(k, key, value);
		for(int i = 0; i<=k; ++i)
		{
			q->forward[i] = update[i]->forward[i];
			update[i]->forward[i] = q;
		}

		printList();
	}
	bool remove(int key)
	{
		Node *update[maxLevel];  
		Node *p,*q;  
		p = slist->head;  
		int k = slist->level;  
		for(int i = k ; i >=0 ; i --){  
			while(p->forward[i] != NULL && (p->forward[i]->key < key)){  
				p = p->forward[i];  
			}  
			update[i] = p;  
		}  
		q = p->forward[0];  
		if(q && q->key == key){  
			for(int i = 0 ; i <= k; ++i){  
				if(update[i]->forward[i] != q)  
					break;  
				update[i]->forward[i] = q->forward[i];  
			}  
			free(q);  
			for(int i = slist->level ; i > 0 ; i --){  
				if(slist->head->forward[i] == NULL){  
					slist->level --;  
				}  
			}
			//printList();
			return true;  
		}  
		else{  
			return false;  
		}  
	}
    virtual ~SkipList()
	{
		destroy();
	}
	
private:
	Node* createNode(int level, int key, int value)
	{
		Node* node = (Node*)malloc(sizeof(Node) + level*sizeof(Node*));
		node->key = key;
		node->value = value;
		return node;
	}
	int random()
	{
		int k = 0;
		while(rand() %2) k++;
		k = k>maxLevel?maxLevel:k;
		return k;
	}
	void printList()
	{
		Node *p,*q;  
		int k=slist->level;  
		for(int i=k; i >= 0; i--)  
		{  
			p = slist->head->forward[i];  
			printf("level[%d]: ",i);  
			while(p->forward[i] != NULL)  
			{  
				printf("%d -> ",p->value);  
				p = p->forward[i];  
			}  
			printf("%d\n",p->value);  
		}  
		printf("\n");  
	}
	void destroy()
	{
		Node *p, *q;
		int k = slist->level;
		p = slist->head;
		for(int i= k; i>=0; --i)
		{
			while((q = p) !=NULL)
			{
				free(q);
				p = p->forward[i];
			}
		}
		free(slist);
		slist = NULL;
	}
private:
    skipList* slist;
};


int main(){
	
	SkipList skip;
	for(int i=1; i<20; ++i)
	{
		skip.insert(i, 2*i);		
	}

	int v = skip.search(5);
	std::cout << v << "\n";
	skip.remove(5);
	std::cout << skip.search(5)<< "\n";
	sleep(2);
    return 0;  
}  
