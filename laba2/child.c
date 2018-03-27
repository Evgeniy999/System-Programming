#include <sys/types.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

//тут он работает и записывает инфу в файл ост как есть ток без времени
class Data
{
public:
    int sale;
    string name;
    int count;
    bool recept;
    Data(){ sale = 0; name ="unknow"; count = 0; recept = false;}
    Data(int Sale, string Name,int Count, int Recept):sale(Sale),name(Name),count(Count),recept(Recept){}
    friend ostream &operator<<(ostream &stream, Data &a)
    {
        stream << a.sale <<' ';
        stream << a.name << ' ';
        stream << a.count <<' ';
	stream << a.recept << endl;
        return stream;
    }
};

	
bool find(vector<Data> arr, char *argv[])
{
	bool flag = false;
	for(int i = 0;i<arr.size()-1;i++)
	{	
		if(arr[i].name == argv[2])
		{
			if (arr[i].count != 0) flag = true;
			else false;
		}
	} 
   	return flag;	
}

vector<Data> find_take(vector<Data> arr, char *argv[])
{	
	int n,b;
	n = atoi(argv[1]);
	b = atoi(argv[3]);
	for(int i = 0;i<arr.size()-1;i++)
	{
	        if(arr[i].sale >= n)
		{
			if(arr[i].name == argv[2])
			{
				if(arr[i].count >= b)
				{	
					arr[i].count = arr[i].count-b;
					if(arr[i].count < b){ printw("No more!\n");break;}
					/*printw("%d ",arr[i].sale);
	  				printw("%s ",arr[i].name.c_str());
	  				printw("%d\n",arr[i].count);*/
					//if(arr[i].recept) printw("With recept!!!\n");
					//printw("You bought a medicine!\n");
				}
			}
		}
	}
	return arr; 
}

int main(int argc, char *argv[])
{
	time_t ltime;
	int c, sale, count;
	string name;bool recept;
        Data data;
	vector<Data> arr;
        ifstream file;
	ofstream file_out;
        file.open("lol.txt", ios::in); 
        while(!file.eof())
	{
   	file>>sale;
	file>>name;
	file>>count;
	file>>recept;
	arr.push_back(Data(sale,name,count,recept));
	}
	file.close();
        //cout <<endl<< argv[0] << " "<< argv[1] <<" "<< argv[2] <<" "<< argv[3] <<" "<< argv[4] ; 
	
        int tap;
	bool check; 
	check = find(arr,argv);
	if(check)
	{
		arr = find_take(arr,argv);
	}
	refresh();
	file_out.open("lol.txt",ios::out);
	for(int i =0; i<arr.size()-1; i++)
	{
		file_out << arr[i]; 
	}
	file_out.close();
	return 0;
}  
























