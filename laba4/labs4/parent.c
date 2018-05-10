#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/sem.h>

using namespace std;
typedef unsigned char byte;
int main(int argc, char *argv[])
{
  	
	sem_t* workSemaphore = sem_open("/workSemaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);
 	if (workSemaphore == SEM_FAILED) {
   	cout << "Error with opening semaphore." << endl;
    	return 1;
  	}
	pid_t pid = fork();
	vector<char> arrChar;
	int x,x2;
	int pipefd[2];
	initscr();
  	noecho();
	clear();
	x = atoi(argv[1]);
	x2 = atoi(argv[2]);
	switch(pid)
	{
		case -1: {
				printw("Error.");
				break;
			}

		case 0: { 
			//cout<<"open child"<<endl;
			execlp("./child", "./child", "100","vitaminA", "1", NULL);
			break;
			}

		default: {	ifstream file;
				file.open("lol.txt", ios::in);
				int i=0;
				arrChar.push_back('S');
				arrChar.push_back('e');
				arrChar.push_back('l');
				arrChar.push_back('l');
				arrChar.push_back('!');
				arrChar.push_back(' ');
				while(i<x)
				{
					char ch = file.get();if( ch == '\n'){ ch = ' ';i++;}
			     	 	arrChar.push_back(ch);
				  	
				}
				file.close();	
     				sem_wait(workSemaphore);

 				if (write(x2, arrChar.data(), arrChar.size()+1) == -1){ cout<<"error pipe"<<endl;}
		}cout<<" ";
   	 }
 	sem_post(workSemaphore);
        close(x2);
  	wait(0);
	return 0;
} 
