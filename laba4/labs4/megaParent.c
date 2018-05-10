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
#include <pthread.h>
 #include <sys/time.h>
using namespace std;

vector<pthread_t> arr;
int outputTime;
pthread_mutex_t mutex;

void* func_pthread(void* arg);
int chld_handler();
void createFlow(int iSignal);

int main(int argc, char* argv[]) {

  initscr();
  noecho();
  scrollok(stdscr, true);

  if (argc < 3){ printf("no arguments\n"); }
  if (chld_handler() == -1) {printf("Error sigation\n");}
  if (pthread_mutex_init(&mutex, NULL) != 0) { printf("Error mutex\n");}

  outputTime = atoi(argv[1]);//интервал с которым буду выводится имена потоков
  time_t autoCreate = atoi(argv[2]);// автоматически

  itimerval timerValue;
  timerValue.it_interval.tv_usec = 0;
  timerValue.it_interval.tv_sec = autoCreate;
  timerValue.it_value.tv_usec = 0;
  timerValue.it_value.tv_sec = autoCreate;

  if (setitimer(ITIMER_REAL, &timerValue, 0)== -1){printf("error settime\n");}//уменьшается и подает сигнал сигаларм
  while (true) {
    switch (getch()) {
      case '+':
	createFlow(SIGVTALRM);
        break;
      case '-':
        if (!arr.empty()) {
	 printf("Finished\n\r");
          pthread_cancel(arr.back());
	  arr.pop_back();
        }else  printf("That all!!!\n\r");
        break;
      case 'q':
       endwin();
  	if (!pthread_mutex_trylock(&mutex)) {
   	  pthread_mutex_unlock(&mutex);
   	  pthread_mutex_destroy(&mutex);
 	}
 	signal(SIGALRM, SIG_DFL);
        return 0; 
    }
  }
}

void* func_pthread(void* arg) {
  int sleepTime = *(int*)arg;
  pthread_t tid = pthread_self();
  while (true) {
    pthread_testcancel();//создает точку отмены
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);//удаление разрещено
    pthread_mutex_lock(&mutex);
    printw("Thread id: %d\n", tid);
    refresh();
    pthread_mutex_unlock(&mutex);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);//удаление запрещено
    sleep(sleepTime);
  }
}
void createFlow(int iSignal) {
  pthread_t tid;
  pthread_attr_t attr;
  pthread_attr_init(&attr);//создание объекта по умолчанию 
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);//статус выхода и поток сохр после завершения потока
  pthread_create(&tid, &attr, func_pthread, &outputTime);//создание потока
  arr.push_back(tid);
}

int chld_handler() {
  struct sigaction alrmStruct;
  alrmStruct.sa_handler = createFlow;
  return sigaction(SIGALRM, &alrmStruct, NULL);
}



