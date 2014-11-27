//
//                     **    UWAGA!   **
//
// Plik asd.cc przeznaczony jest tylko do wpisania wlasnych testow.
// Cala implementacja powinna znajdowac sie w pliku aisdihashmap.h

#include<iostream>
#include "aisdihashmap.h"



using namespace std;

int main()
{
   // Miejsce na testy
   cout << "Testy:" << endl;
   try{
   AISDIHashMap<string, int, hashF, _compFunc> m;
   cout<<"size: "<<m.size()<<endl;
   m.insert(make_pair("napis",1));
   cout<<"size: "<<m.size()<<endl;
   m.insert(make_pair("napis2",2));
   cout<<"size: "<<m.size()<<endl;
   m.insert(make_pair("napis3",3));
   cout<<"size: "<<m.size()<<endl;
   //system("pause");
    if(m.find("napis2")!=m.end())
        cout<<"find"<<endl;
    cout<<"size: "<<m.size()<<endl;
   //system("pause");
   cout<<m["napis3"]<<endl;
   cout<<"size: "<<m.size()<<endl;
   cout<<m["napis4"]<<endl;
   cout<<"size: "<<m.size()<<endl;
   m.print_data();
   m.erase("napis2");
   cout<<"size: "<<m.size()<<endl;
   m.print_data();
   cout<<"size: "<<m.size()<<endl;
   }
   catch(string s)
   {
       cout<<s<<endl;
   }
//   system("pause");
   return 0;
}
