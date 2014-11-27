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

http://users.ift.uni.wroc.pl/~zkoza/dydaktyka/ksiazka/pointer/8hash.html
*******************************************************************************/

/**
 * Tomasz Jakubczyk
 * grupa 3I3
 * data: 22.11.2014
 *
 *
 *
 */

#include <utility>
#include<iostream>
#include<cstdlib>

#define MY_HASH_NUMBER 65537

#define DEBUG if(true)

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

inline unsigned hashF(const std::string& str)
{
    if(str.empty())
    {
DEBUG throw std::string("hashF str.empty()");
        return 0;
    }
    unsigned int h=str[0];
    for(unsigned int i=1;i<str.length();i++)
    {
        h=(h<<9)+str[i];
    }
    return h%MY_HASH_NUMBER;
};
/// A map with a similar interface to std::map.
template<class K, class V,
         unsigned hashFunc(const K&),
         bool compFunc(const K&,const K&)=&_compFunc<K> >
class AISDIHashMap
{
private:
    struct MyElement;
public:
    class iterator;
    typedef unsigned size_type;
    MyElement* user_first;
    MyElement* user_last;
private:
    class MyList;
    typedef std::pair<K, V> Data;
    struct MyElement
    {
        //static int elementow;
        Data data;
        MyElement* user_next;//lista dwukierunkowa do iterowania po calej HashTable
        MyElement* user_prev;
        MyElement* hash_next;//lista dwukierunkowa do operacji na jednym hashu
        MyElement* hash_prev;
        MyElement* user_first;
        MyElement* user_last;
        MyElement(const std::pair<K,V>& d, MyElement* f, MyElement* l):
            data(d), user_first(f), user_last(l), user_next(NULL), user_prev(NULL), hash_next(NULL), hash_prev(NULL)
        {
            //elementow++;
        };
        ~MyElement()
        {
            //elementow--;
        };
    };
    friend class MyList;
    class MyList
    {
    private:
        friend class AISDIHashMap;
        MyElement* first;//poczatek listy
        MyElement* last;//koniec listy
        MyElement* user_first;
        MyElement* user_last;
    public:
        MyList()
        {
            first=NULL;
            last=NULL;
            user_first=NULL;
            user_last=NULL;
        }
        MyList(MyElement* f, MyElement* l) :
            user_first(f), user_last(l)
        {
            first=NULL;
            last=NULL;
        };
        ~MyList()
        {
            MyElement* tmp1=first;
            MyElement* tmp2;
            while(tmp1!=NULL)
            {
                tmp2=tmp1->hash_next;
                delete tmp1;
                //elementow--;
                tmp1=tmp2;
            }
        };
        inline iterator insert(const std::pair<K, V>& entry)
        {
            MyElement* nowy=new MyElement(entry, user_first, user_last);
            if(first==NULL || last==NULL)//pierwszy w hashu
            {
DEBUG if(first!=NULL || last!=NULL) throw std::string("(first==NULL || last==NULL) (first!=NULL || last!=NULL)");
                first=nowy;
                last=nowy;
                if(user_first==user_last)//pierwszy w calej tablicy
                {
                    user_first=nowy;
                    user_first->user_next=user_last;
                    user_last->user_prev=nowy;
                }
                else
                {
                    nowy->user_next=user_last;
                    nowy->user_prev=user_last->user_prev;
                    user_last->user_prev=nowy;
                    nowy->user_prev->user_next=nowy;
                }
                return iterator(nowy);
            }
            last->hash_next=nowy;
            nowy->hash_prev=last;
            last=nowy;
            nowy->user_next=user_last;
            nowy->user_prev=user_last->user_prev;
            user_last->user_prev=nowy;
            nowy->user_prev->user_next=nowy;
            return iterator(nowy);
        };
        inline iterator find(const K& k)
        {
            MyElement* tmp1=first;
            while(tmp1!=NULL)
            {
                if(compFunc(tmp1->data.first,k))
                {
                    return iterator(tmp1);
                }
                tmp1=tmp1->hash_next;
            }
            return iterator(tmp1->user_last);//end()
        };
        inline size_type erase(const K& k)
        {
            MyElement* tmp1=first;
            while(tmp1!=NULL)
            {
                if(compFunc(tmp1->data.first, k))
                {
                    if(tmp1->hash_next!=NULL)//lista nie ma straznika
                    {
                        tmp1->hash_next->hash_prev=tmp1->hash_prev;
                    }
                    else
                    {
                        last=tmp1->hash_prev;
                        last->hash_next=NULL;//spelnienie moich zalozen projektowych o NULLach
                    }
                    if(tmp1->hash_prev!=NULL)//poczatek nie ma poprzednika
                    {
                        tmp1->hash_prev->hash_next=tmp1->hash_next;
                    }
                    else
                    {
                        first=tmp1->hash_next;
                        first->hash_prev=NULL;
                    }
                    tmp1->user_next->user_prev=tmp1->user_prev;//na koncu jest straznik
                    if(tmp1->user_prev!=NULL)
                    {
                        tmp1->user_prev->user_next=tmp1->user_next;
                    }
                    else
                    {
                        user_first=tmp1->user_next;
                        user_first->user_prev=NULL;
                    }
                    delete tmp1;
                    //elementow--;
                    return 1;
                }
                tmp1=tmp1->hash_next;
            }
            return 0;
        };
        inline iterator erase(iterator i)
        {
            MyElement* el=i.node;
            if(el->hash_next!=NULL)
            {
                el->hash_next->hash_prev=el->hash_prev;
            }
            else
            {
                last=el->hash_prev;
                last->hash_next=NULL;
            }
            if(el->hash_prev!=NULL)//poczatek nie ma poprzednika
            {
                el->hash_prev->hash_next=el->hash_next;
            }
            else
            {
                first=el->hash_next;
                first->hash_prev=NULL;
            }
            el->user_next->user_prev=el->user_prev;//na koncu jest straznik
            if(el->user_prev!=NULL)
            {
                el->user_prev->user_next=el->user_next;
            }
            else
            {
                user_first=el->user_next;
                user_first->user_prev=NULL;
            }
            iterator nast=i;
            nast++;
            delete el;
            //elementow--;
            return nast;
        };
    };
    MyList tab[MY_HASH_NUMBER];
public:
    typedef K key_type;
    typedef V value_type;

    AISDIHashMap()
    {
DEBUG std::cout<<"AISDIHashMap()"<<std::endl;
DEBUG system("pause");
        //elementow=0;//zerowanie zabezpieczajace
        //tab=new MyList[MY_HASH_NUMBER];
        user_first=new MyElement(Data(static_cast<K>(""),static_cast<V>(0)), user_first, user_last);//sentinel, nie jest w tablicy
DEBUG std::cout<<"AISDIHashMap()"<<std::endl;
DEBUG system("pause");
        user_last=user_first;
DEBUG std::cout<<"AISDIHashMap()"<<std::endl;
DEBUG system("pause");
        //elementow=0;//nie licze straznika
    };
    ~AISDIHashMap()
    {
DEBUG std::cout<<"~AISDIHashMap()"<<std::endl;
DEBUG system("pause");
        delete[] tab;
DEBUG std::cout<<"~AISDIHashMap()"<<std::endl;
DEBUG system("pause");
        delete user_last;
    };

   /// Coping constructor.
    explicit AISDIHashMap(const AISDIHashMap<K, V, hashFunc, compFunc>& a);

   /// const_iterator.
    class const_iterator : public std::iterator<std::bidirectional_iterator_tag,
                                               std::pair<K, V> >
    {
    protected:
        MyElement* node;
        const_iterator(MyElement* x) : node(x) {};
        friend class AISDIHashMap;
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
        /*inline*/ const_iterator& operator++()
        {
            if(node->user_next!=NULL)
            {
                node=node->user_next;
            }
            return *this;
        };
        // postincrementacja
        /*inline*/ const_iterator operator++(int)
        {
            const_iterator i=*this;
            ++(*this);
            return i;
        };
        // predekrementacja
        /*inline*/ const_iterator& operator--()
        {
            if(node->prev!=NULL)
            {
                node=node->prev;
            }
            return *this;
        };
        // postdekrementacja
        /*inline*/ const_iterator operator--(int)
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
    private:
        MyElement* node;
        iterator(MyElement* x) : const_iterator(x) {};
        friend class AISDIHashMap;
    public:
        iterator() {};
        iterator(const iterator& a) { node = a.node; };
        inline Data& operator*() const
        {
            return node->data;
        };
        inline Data* operator->() const
        {
            return &(node->data);
        };
        iterator& operator++()
        {  // preincrementacja
            ++(*(const_iterator*)this);
            return (*this);
        };
        iterator operator++(int)
        {  // postincrementacja
            iterator temp = *this;
            ++*this;
            return temp;
        };
        iterator& operator--()
        {  // predekrementacja
            --(*(const_iterator*)this);
            return (*this);
        };
        iterator operator--(int)
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
        MyList tmp_hash=tab[hashFunc(entry.first)];
        iterator i;
        if(tmp_hash.find(entry.first)!=end())
        {
            i=tmp_hash.insert(entry);
            //elementow++;
            return std::make_pair(i, (bool)true);
        }
        return std::make_pair(end(), (bool)false);
    };

   /// Returns an iterator addressing the location of the entry in the map
   /// that has a key equivalent to the specified one or the location succeeding the
   /// last element in the map if there is no match for the key.
    iterator find(const K& k)
    {
        return tab[hashFunc(k)].find(k);
    };

   /// Returns an const iterator
    const_iterator find(const K& k) const
    {
        return tab[hashFunc(k)].find(k);
    };

   /// Inserts an element into a map with a specified key value
   /// if one with such a key value does not exist.
   /// @returns Reference to the value component of the element defined by the key.
    V& operator[](const K& k)
    {
        MyList tmp_hash=tab[hashFunc(k)];
        iterator i=tmp_hash.find(k);
        if(i!=end())
        {
            return i->first;
        }
        else
        {
            i=tmp_hash.insert(Data(k,NULL));
            //elementow++;
            return i->first;
        }
    };

   /// Tests if a map is empty.
    bool empty( ) const
    {
        if(begin()==end())
        {
            return true;
        }
        return false;
    };

   /// Returns the number of elements in the map.
    size_type size() const
    {
        return 0;
    };

   /// Returns the number of elements in a map whose key matches a parameter-specified key.
    size_type count(const K& k) const
    {
        if(tab[hashFunc(k)].find(k)==end())
        {
            return 1;
        }
        return 0;
    };

   /// Removes an element from the map.
   /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator i)
    {
        if(i==end())
        {
            return i;
        }
        return tab[hashFunc(i->first)].erase(i);//brzydki dostep, ale O(1)
    };

   /// Removes a range of elements from the map.
   /// The range is defined by the key values of the first and last iterators
   /// first is the first element removed and last is the element just beyond the last elemnt removed.
   /// @returns The iterator that designates the first element remaining beyond any elements removed.
    iterator erase(iterator first, iterator last)
    {//tak jest najszybciej napisac, ale dzialalo by minimalnie szybciej bez iteratorow
        for(iterator i=first;i!=last;i++)
        {
            erase(i);
        }
    };

   /// Removes an element from the map.
   /// @returns The number of elements that have been removed from the map.
   ///          Since this is not a multimap itshould be 1 or 0.
    size_type erase(const K& key)
    {
        return tab[hashFunc(key)].erase(key);
    };

   /// Erases all the elements of a map.
    void clear( )
    {
        erase(begin(), end());
    };


};
