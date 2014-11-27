/**
@file aisdihashmap.h

AISDIHashMap and related functions interface.

@author
Pawel Cichocki

@date
last revision
- 2006.03.24 Pawel Cichocki: wrote it

COPYRIGHT:
Copyright (c) 2006 Instytut Informatyki, Politechnika Warszawska
ALL RIGHTS RESERVED

Tomasz Jakubczyk
3I3 253093
25.11.2014
moja hash mapa to tablica wskaznikow gdzie indeksami sa hashe
pod jednym hashem jest lista dwukierunkowa bez straznikow, zabezpieczona NULL
element posiada jeszcze dwa wskazniki listy dwukierunkowej(user) w kolejnosci dodawania,
ma to na celu znaczne ulatwienie pracy z iteratorami, na koncu tej listy jest sraznik,
ktorego nie ma pod zadnym hashem.
dla tablicy rozmiaru 2^16 najlepsza funkcja hashujaca okazal sie one-at-a-time by Bob Jenkins
z wynikiem kolizji: 115148
*******************************************************************************/

#include <utility>
//#include<iostream>
//#include<cstdlib>
#include<string>

#define MY_HASH_NUMBER 65536
//262144
//519227
//#define MY_MASK 0xFFFF

typedef  unsigned long  int  ub4;   /* unsigned 4-byte quantities */

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)

//#define DEBUG

#ifdef DEBUG
#define DEBUG if(true)
#else
#define DEBUG if(false)
#endif

/// Default keys' comparing function for AISDIHashMap - it uses
/// operator== by default.
/// @returns true if keys are equal, false otherwise.
/// @param key1 First key to compare.
/// @param key2 Second key to compare.
template <class Key>
inline bool _compFunc(const Key& key1,const Key& key2)
{
   return (key1==key2);
};

inline unsigned hashF0(const std::string& str)//dla 2^16 kolizji: 175748
{
    if(str.empty())
    {
//DEBUG throw std::string("hashF str.empty()");
        return 0;
    }
    unsigned int h=str[0];
    for(unsigned int i=1;i<str.length();i++)
    {
        h=(h<<9)+str[i];
    }
    //return h%MY_HASH_NUMBER;
    return (h & hashmask(16));
};

//http://en.wikipedia.org/wiki/Jenkins_hash_function
inline unsigned hashF(const std::string& str)//dla 2^16 kolizji: 115148
{
    unsigned int h=0, i=0, len=str.size();

    for(; i < len; ++i)
    {
        h += str[i];
        h += (h << 10);
        h ^= (h >> 6);
    }
    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);
    //return h%MY_HASH_NUMBER;
    //return (h&MY_MASK);
    return (h & hashmask(16));
}

//http://en.wikipedia.org/wiki/MurmurHash
inline unsigned hashF2(const std::string& str) {//dla 2^16 kolizji: 115153
    unsigned len=str.size();
    const char *key=str.c_str();
    unsigned seed=519227;
	static const unsigned c1 = 0xcc9e2d51;
	static const unsigned c2 = 0x1b873593;
	static const unsigned r1 = 15;
	static const unsigned r2 = 13;
	static const unsigned m = 5;
	static const unsigned n = 0xe6546b64;

	unsigned h = seed;

	const int nblocks = len / 4;
	const unsigned *blocks = (const unsigned *) key;
	int i;
	for (i = 0; i < nblocks; i++) {
		unsigned k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;

		h ^= k;
		h = ((h << r2) | (h >> (32 - r2))) * m + n;
	}

	const unsigned char *tail = (const unsigned char *) (key + nblocks * 4);
	unsigned k1 = 0;

	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];

		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		h ^= k1;
	}

	h ^= len;
	h ^= (h >> 16);
	h *= 0x85ebca6b;
	h ^= (h >> 13);
	h *= 0xc2b2ae35;
	h ^= (h >> 16);

	return (h & hashmask(16));
	//return (h & hashmask(18));
}

template<class K, class V>
struct MyElementS
{
    typedef std::pair<K, V> Data;
        //static int elementow;
    Data data;
    MyElementS<K, V>* user_next;//lista dwukierunkowa do iterowania po calej HashTable
    MyElementS<K, V>* user_prev;
    MyElementS<K, V>* hash_next;//lista dwukierunkowa do operacji na jednym hashu
    MyElementS<K, V>* hash_prev;
    MyElementS(const std::pair<K,V>& d):
        data(d), user_next(NULL), user_prev(NULL), hash_next(NULL), hash_prev(NULL)
    {
        //elementow++;
    };
    ~MyElementS()
    {
            //elementow--;
    };
};

/// A map with a similar interface to std::map.
template<class K, class V,
         unsigned hashFunc(const K&),
         bool compFunc(const K&,const K&)=&_compFunc<K> >
class AISDIHashMap
{
public:
    typedef std::pair<K, V> Data;
    typedef MyElementS<K, V> MyElement;
    MyElement* user_first;
    MyElement* user_last;
    MyElement* tab[MY_HASH_NUMBER];
    int elementow;
    //int kolizja;

   typedef K key_type;
   typedef V value_type;
   typedef unsigned size_type;

    void print_data()
    {
        /*iterator i=iterator(user_first);
        while(i!=end())
        {
            std::cout<<i->first<<" "<<i->second<<std::endl;
            std::cout<<i.node->user_prev<<" "<<i.node<<" "<<i.node->user_next<<std::endl;
            std::cout<<i.node->hash_prev<<" "<<i.node<<" "<<i.node->hash_next<<std::endl;
            i++;
        }*/
    }

    AISDIHashMap()
    {
        //tab=new MyElement[MY_HASH_NUMBER];
        for(unsigned i=0;i<MY_HASH_NUMBER;i++)
            tab[i]=NULL;
        user_first=new MyElement(Data("",0));
        user_last=user_first;
        elementow=0;
        //kolizja=0;
    };
    ~AISDIHashMap()
    {
        //clear();
        //delete user_last;
        MyElement* tmp=user_first;
        MyElement* tmp_next;
        while(tmp!=NULL)
        {
            tmp_next=tmp->user_next;
            delete tmp;
            tmp=tmp_next;
        }
        //std::cout<<"kolizji: "<<kolizja<<std::endl;
    };

   /// Coping constructor.
    explicit AISDIHashMap(const AISDIHashMap<K, V, hashFunc, compFunc>& a)
    {
        for(unsigned i=0;i<MY_HASH_NUMBER;i++)
            tab[i]=NULL;
        user_first=new MyElement(Data("",0));
        user_last=user_first;
        elementow=0;
        iterator i=iterator(a.user_first);
        while(i!=a.end())
        {
            insert(i.node->data);
            i++;
        }
    };

   /// const_iterator.
    class const_iterator : public std::iterator<std::forward_iterator_tag,
                                               std::pair<K, V> >
    {
    public:
        typedef std::pair<K, V> Data;
        //MyElement* node;
    protected:
        MyElement* node;
        friend class AISDIHashMap;
        const_iterator(MyElement* x) : node(x) {};
    public:
        const_iterator() {};
        const_iterator(const const_iterator& a) : node(a.node) {};
        inline const Data& operator*() const
        {
            return node->data;//sedno dzialania iteratora
        };
        inline const Data* operator->() const
        {
            return &(node->data);
        };
        // preincrementacja
        inline const_iterator& operator++()
        {
//DEBUG std::cout<<"operator++()"<<std::endl;
//            if(node==NULL)
//DEBUG std::cout<<"if(node==NULL)"<<std::endl;
            if(node->user_next!=NULL)
            {
//DEBUG std::cout<<node<<" > "<<node->user_next<<std::endl;
                node=node->user_next;
            }
            return *this;
        };
        // postincrementacja
        inline const_iterator operator++(int)
        {
            const_iterator i=*this;
            ++(*this);
            return i;
        };
        // predekrementacja
        inline const_iterator& operator--()
        {
            if(node->prev!=NULL)
            {
                node=node->user_prev;
            }
            return *this;
        };
        // postdekrementacja
        inline const_iterator operator--(int)
        {
            const_iterator i=*this;
            --(*this);
            return i;
        };
        inline bool operator==(const const_iterator& a) const
        {
            return node == a.node;
        };
        inline bool operator!=(const const_iterator& a) const
        {
            return node != a.node;
        }
    };
   /// iterator.
    class iterator : public const_iterator
    {
        friend class AISDIHashMap;
        //MyElement* node;
        iterator(MyElement* x) : const_iterator(x) {};
   public:
      iterator() {};
      iterator(const iterator& a) { const_iterator::node = a.node; };

      inline Data& operator*() const
      {
         return const_iterator::node->data;
      };
      inline Data* operator->() const
      {
         return (&(const_iterator::node->data));
      };
      inline iterator& operator++()
      {  // preincrementacja
         ++(*(const_iterator*)this);
         return (*this);
      };
      inline iterator operator++(int)
      {  // postincrementacja
         iterator temp = *this;
         ++*this;
         return temp;
      };
      inline iterator& operator--()
      {  // predekrementacja
         --(*(const_iterator*)this);
         return (*this);
      };
      inline iterator operator--(int)
      {  // postdekrementacja
         iterator temp = *this;
         --*this;
         return temp;
      };
    };

   /// Returns an iterator addressing the first element in the map.
    inline iterator begin()
    {
        return iterator(user_first);
    };
    inline const_iterator begin() const
    {
        return const_iterator(user_first);
    };

   /// Returns an iterator that addresses the location succeeding the last element in a map.
    inline iterator end()
    {
        return iterator(user_last);
    };
    inline const_iterator end() const
    {
        return const_iterator(user_last);
    };

   /// Inserts an element into the map.
   /// @returns A pair whose bool component is true if an insertion was
   ///          made and false if the map already contained an element
   ///          associated with that key, and whose iterator component coresponds to
   ///          the address where a new element was inserted or where the element
   ///          was already located.
    std::pair<iterator, bool> insert(const std::pair<K, V>& entry)
    {
//DEBUG std::cout<<"insert(const std::pair<K, V>& entry)"<<std::endl;
        unsigned index=hashFunc(entry.first);
        MyElement* tmp_hash=tab[index];
        MyElement* tmp_prev;
        iterator i;
        while(tmp_hash!=NULL)
        {
//DEBUG std::cout<<"key: "<<tmp_hash->data.first<<std::endl;
            if(compFunc(tmp_hash->data.first, entry.first))//jest juz taki
            {
//DEBUG std::cout<<"if(compFunc(tmp_hash->data.first, entry.first))"<<std::endl;
                i=iterator(tmp_hash);
                return std::make_pair(i,(bool)false);
            }
            if(tmp_hash->hash_next==NULL)//to jest ostatni
            {
                break;
            }
            tmp_hash=tmp_hash->hash_next;
        }
        if(tmp_hash==NULL)//jesli lista byla pusta
        {
//DEBUG std::cout<<"if(tmp_hash==NULL)"<<std::endl;
            tmp_hash=new MyElement(entry);
            tab[index]=tmp_hash;
        }
        else//kolizja
        {
            tmp_prev=tmp_hash;
            tmp_hash=new MyElement(entry);
            //tab[index]=tmp_hash;
            tmp_prev->hash_next=tmp_hash;
            tmp_hash->hash_prev=tmp_prev;
            //kolizja++;
        }
        if(user_first==user_last)//jesli w mapie jest tylko straznik
        {
            user_first=tmp_hash;
            user_first->user_next=user_last;
            user_last->user_prev=user_first;
        }
        else
        {
            tmp_hash->user_prev=user_last->user_prev;
            tmp_hash->user_next=user_last;
            user_last->user_prev->user_next=tmp_hash;
            user_last->user_prev=tmp_hash;
        }
//DEBUG std::cout<<tmp_hash->data.first<<" "<<tmp_hash<<std::endl;
        elementow++;
        return std::make_pair(iterator(tmp_hash), (bool)true);
    };

   /// Returns an iterator addressing the location of the entry in the map
   /// that has a key equivalent to the specified one or the location succeeding the
   /// last element in the map if there is no match for the key.
    iterator find(const K& k)
    {
//DEBUG std::cout<<"find(const K& k)"<<std::endl;
        MyElement* tmp_hash=tab[hashFunc(k)];
        while(tmp_hash!=NULL)
        {
            if(compFunc(tmp_hash->data.first, k))
                return iterator(tmp_hash);
            tmp_hash=tmp_hash->hash_next;
        }
        return end();
    };

   /// Returns an const iterator
    const_iterator find(const K& k) const
    {
//DEBUG std::cout<<"find(const K& k) const"<<std::endl;
        MyElement* tmp_hash=tab[hashFunc(k)];
        while(tmp_hash!=NULL)
        {
            if(compFunc(tmp_hash->data.first, k))
                return const_iterator(tmp_hash);
            tmp_hash=tmp_hash->hash_next;
        }
        return end();
    };

   /// Inserts an element into a map with a specified key value
   /// if one with such a key value does not exist.
   /// @returns Reference to the value component of the element defined by the key.
    V& operator[](const K& k)
    {
//DEBUG std::cout<<"V& operator[](const K& k)"<<std::endl;
        std::pair<iterator, bool> p;
        iterator i;
        /*i=find(k);
        if(i!=end())
        {
            return i->second;
        }*/
        p=insert(Data(k,0));
//DEBUG std::cout<<"p=insert(Data(k,0));"<<std::endl;
//DEBUG std::cout<<p.second<<std::endl;
        i=p.first;
//DEBUG std::cout<<"iterator i=p.first;"<<std::endl;
//DEBUG std::cout<<i.node<<std::endl;
//DEBUG std::cout<<i->first<<std::endl;
//DEBUG std::cout<<i->second<<std::endl;
        return i->second;
    };

   /// Tests if a map is empty.
    inline bool empty( ) const
    {
//DEBUG std::cout<<"empty( ) const"<<std::endl;
        if(begin()==end())
        {
            return true;
        }
        return false;
    };

   /// Returns the number of elements in the map.
   inline size_type size() const
   {
//DEBUG std::cout<<"size() const"<<std::endl;
       return elementow;
   };

   /// Returns the number of elements in a map whose key matches a parameter-specified key.
    inline size_type count(const K& k) const
    {
        if(find(k)!=end())
        {
            return 1;
        }
        return 0;
    };

   /// Removes an element from the map.
   /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator i)
    {
//DEBUG std::cout<<"erase(iterator i)"<<std::endl;
        iterator pom=i;
        pom++;
        unsigned index=hashFunc(i->first);
        MyElement* tmp_hash=tab[index];
        if(i.node==user_last)//erase(end()) nie powinno do tego dojsc
        {
//DEBUG std::cout<<"if(i.node==user_last)"<<std::endl;
            return i;
        }
        if(i.node->hash_next==NULL && i.node->hash_prev==NULL)//jedyny w liscie
        {
//DEBUG std::cout<<"if(i.node->hash_next==NULL && i.node->hash_prev==NULL)"<<std::endl;
            tab[index]=NULL;
            if(i.node==user_first)//pierwszy w mapie
            {
//DEBUG std::cout<<"if(i.node==user_first)"<<std::endl;
                user_first=i.node->user_next;
                i.node->user_next->user_prev=NULL;
            }
            else
            {
                i.node->user_next->user_prev=i.node->user_prev;
                i.node->user_prev->user_next=i.node->user_next;
            }
//DEBUG std::cout<<pom.node<<" "<<pom.node->user_prev<<" "<<pom.node->user_next<<std::endl;
//DEBUG std::cout<<"delete i.node;"<<std::endl;
            elementow--;
            delete i.node;
//DEBUG std::cout<<"return i;"<<std::endl;
            return pom;
        }
        if(i.node==tmp_hash)//pierwszy w liscie
        {
//DEBUG std::cout<<"if(i.node==tmp_hash)"<<std::endl;
            tab[index]=i.node->hash_next;
            if(i.node==user_first)//pierwszy w mapie
            {
                user_first=i.node->user_next;
                i.node->user_next->user_prev=NULL;
            }
            else
            {
                i.node->user_next->user_prev=i.node->user_prev;
                i.node->user_prev->user_next=i.node->user_next;
            }
            elementow--;
            delete i.node;
            return pom;
        }
        if(i.node->hash_next==NULL)//ostatni w liscie
        {
//DEBUG std::cout<<"if(i.node->hash_next==NULL)"<<std::endl;
            i.node->hash_prev->hash_next=NULL;
            if(i.node==user_first)//pierwszy w mapie
            {
                user_first=i.node->user_next;
                i.node->user_next->user_prev=NULL;
            }
            else
            {
                i.node->user_next->user_prev=i.node->user_prev;
                i.node->user_prev->user_next=i.node->user_next;
            }
            elementow--;
            delete i.node;
            return pom;
        }
        if(i.node==user_first)//pierwszy w mapie. i nie pierwszy w liscie to dziwne
        {
//DEBUG std::cout<<"f(i.node==user_first)"<<std::endl;
            i.node->hash_next->hash_prev=i.node->hash_prev;
            i.node->hash_prev->hash_next=i.node->hash_next;
            user_first=i.node->user_next;
            i.node->user_next->user_prev=NULL;
            elementow--;
            delete i.node;
            return pom;
        }
        i.node->hash_next->hash_prev=i.node->hash_prev;
        i.node->hash_prev->hash_next=i.node->hash_next;
        i.node->user_next->user_prev=i.node->user_prev;
        i.node->user_prev->user_next=i.node->user_next;
        elementow--;
        delete i.node;
        return pom;
    };

   /// Removes a range of elements from the map.
   /// The range is defined by the key values of the first and last iterators
   /// first is the first element removed and last is the element just beyond the last elemnt removed.
   /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator first, iterator last)
    {//tak jest najszybciej napisac, ale dzialalo by minimalnie szybciej bez iteratorow
//DEBUG std::cout<<"erase(iterator first, iterator last)"<<std::endl;
        for(iterator i=first;i!=last;i++)
        {
            erase(i);
//DEBUG std::cout<<" erase(i); i++"<<std::endl;
//DEBUG system("pause");
        }
        return end();
    };

   /// Removes an element from the map.
   /// @returns The number of elements that have been removed from the map.
   ///          Since this is not a multimap itshould be 1 or 0.
    size_type erase(const K& key)
    {
//DEBUG std::cout<<"erase(const K& key)"<<std::endl;
        iterator i=find(key);
        if(i!=end())
        {
            erase(i);
            return 1;
        }
        return 0;
    };

   /// Erases all the elements of a map.
    void clear( )
    {
//DEBUG std::cout<<"clear( )"<<std::endl;
        MyElement* tmp=user_first;
        MyElement* tmp_next=tmp->user_next;
        while(tmp_next!=NULL)
        {
            tmp_next=tmp->user_next;
            delete tmp;
            tmp=tmp_next;
        }
        for(unsigned i=0;i<MY_HASH_NUMBER;i++)
            tab[i]=NULL;
        elementow=0;
    };


};
