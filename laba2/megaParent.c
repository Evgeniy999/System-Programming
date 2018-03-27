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
#include <cstring>
#include <signal.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <iterator>
#include <algorithm>
#include <csignal>
#include <sys/sem.h>

using namespace std;

vector<int> arrPid;
sem_t* workSemaphore;
struct sigaction chld_struct;

vector<int> rep(vector<int> rewrite)
{
	return rewrite;
}

void chld_handler(int iSignal)
{
    int index = distance(arrPid.begin(), std::find(arrPid.begin(), arrPid.end(), wait(0)));
    if (index == -1) {
     cout << "Unknown signal" << endl;
    return;
    }
    arrPid.at(index);
    arrPid.erase(arrPid.begin()+index);
    arrPid = rep(arrPid);
    int value;
    sem_getvalue(workSemaphore, &value);// возр текущ знач семафора
    if (arrPid.empty() && !value) {
    sem_post(workSemaphore);
  }
}

pid_t create_child_proc(int processNumber) {
  char buff[20];
  snprintf(buff, sizeof(buff),"%d",processNumber);
  pid_t pid = fork();
  switch (pid) {
    case -1:
      exit(1);
    case 0:
	if(execlp("./parent", "./parent", buff, NULL) == -1){printw("Error 2");exit(2);}
      return 0;
    default:
      return pid;
  }
}

int main(int argc, char* argv[]) {

  
  chld_struct.sa_handler = chld_handler;
  if(sigaction(SIGCHLD, &chld_struct, NULL) == -1)
    {  
	cout<<"Error Sigation"<<endl;
    }
	
  workSemaphore = sem_open("/workSemaphore", O_CREAT, S_IRUSR | S_IWUSR, 1);

  if (workSemaphore == SEM_FAILED) {
    cout << "Error creating semaphore" << endl;return 1;
  }
  initscr();
  noecho();
  refresh();
  move(0,0);
  int count = 1;
  while (true) {
    switch (getch()) {
      case '+':
        arrPid.push_back(create_child_proc(count));
        count++;
        break;
      case '-':
        if (!arrPid.empty()) {
          kill(arrPid.back(), SIGTERM);
        }
        if (count > 1) {
          count--;
        }
        break;
      case 'q':
        endwin();
        sem_close(workSemaphore);
        sem_unlink("/workSemaphore");
        signal(SIGCHLD, SIG_DFL);
        kill(-getpid(), SIGTERM);
    }
  }
}






