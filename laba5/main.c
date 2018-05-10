#include <iostream>
#include <vector>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <aio.h>      // асинхронный ввод и вывод 
#include <sys/stat.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <gnu/lib-names.h>

#define sizeBuff 100

char buffer[sizeBuff];
std::string resultFileName("./result.txt");
std::vector<std::string> fileNames;

int current_bytes = 0;
int last_bytes = 0;
int all_bytes = 0;

aiocb aioRead; //Обзор асинхронного ввода-вывода
aiocb aioWrite;

pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exit_mutex = PTHREAD_MUTEX_INITIALIZER;

void (*s_read) (char f[sizeBuff], aiocb *aioHandler, char buffer[sizeBuff]);
void (*s_write) (char f[sizeBuff], aiocb *aioHandler, int current_bytes, int all_bytes, char buffer[sizeBuff]);

static void actionHandler(int sig, siginfo_t *sign, void *ucontext)
{
}

void *reader(void *args) 
{
	for(int i = 0; i < fileNames.size(); i++) 
	{
		pthread_mutex_lock(&write_mutex);
		s_read(const_cast<char*>(fileNames[i].c_str()), &aioRead, buffer);
		pthread_mutex_unlock(&read_mutex);
	}
	pthread_mutex_unlock(&exit_mutex); 
  	pthread_mutex_unlock(&read_mutex);
}

void *writer(void *args) 
{
	while(true) 
	{
		pthread_mutex_lock(&read_mutex);
		current_bytes = aio_return(&aioRead);
		buffer[current_bytes] = '\0';
		s_write(const_cast<char*>(resultFileName.c_str()), &aioWrite, current_bytes, all_bytes, buffer);
		last_bytes = current_bytes;
		all_bytes += last_bytes;
		if(pthread_mutex_trylock(&exit_mutex) == 0)
      		break; 
		pthread_mutex_unlock(&write_mutex);
	}
}

int main()
{
        struct sigaction sig;
        sig.sa_flags = SA_RESTART;
        sigemptyset(&sig.sa_mask);
        sig.sa_flags = SA_RESTART | SA_SIGINFO;
        sig.sa_sigaction = actionHandler;
        if(sigaction(SIGUSR1, &sig, NULL) == -1)
           exit(1);
        memset(&aioRead, 0, sizeof(struct aiocb));// заполение массива указанными символами
        memset(&aioWrite, 0, sizeof(struct aiocb));

	pthread_t read_thread;
	pthread_t write_thread;

        fileNames.push_back("./first.txt");
        fileNames.push_back("./second.txt");
        fileNames.push_back("./third.txt");

	void *lib = dlopen("./lib.so", RTLD_NOW);
	if(!lib) printf("error at dlopen\n");

	s_read = (void(*)(char f[sizeBuff], aiocb *aioHandler, char buffer[sizeBuff])) dlsym(lib,"readLib");
  	if(!s_read) printf("error at s_read\n");

  	char *error = dlerror();

    	if (error != NULL) {
    	   fprintf(stderr, "%s\n", error);
   	}

  	s_write = (void(*)(char f[sizeBuff], aiocb *aioHandler, int current_bytes, int all_bytes, char buffer[sizeBuff])) dlsym(lib,"writeLib"); 

  	if(!s_write) printf("error at s_write\n");

  	error = dlerror();//Возвращает читаемую пользователем строку, описывающую самую последнюю ошибку, которая произошла

   	if (error != NULL) {
    	    fprintf(stderr, "%s\n", error);
    	}
	pthread_mutex_lock(&read_mutex);
	pthread_mutex_lock(&exit_mutex);

	pthread_create(&read_thread, NULL, &reader, NULL);
  	pthread_create(&write_thread, NULL, &writer, NULL);

  	pthread_join(read_thread, NULL);
  	pthread_join(write_thread, NULL);

  	dlclose(lib);

	return 0;
}

