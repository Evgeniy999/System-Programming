#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <errno.h>
#include <aio.h>
#include <stdlib.h>
#include <stdio.h>

#define sizeBuff 100

//.so - динамич библ, файл совместного использования 

void readLib(char fileName[sizeBuff], struct aiocb *aioHandler, char buffer[sizeBuff])
{
	int fd;
	int status;
	fd = open(fileName, O_RDONLY);
	aioHandler->aio_fildes = fd;// файловый дескриптор
	aioHandler->aio_buf = buffer;//расположение буфера
	aioHandler->aio_offset = 0;//смещение файла
	aioHandler->aio_nbytes = sizeBuff;//размер
	aioHandler->aio_lio_opcode = LIO_READ;//выполнение операции
	aioHandler->aio_sigevent.sigev_signo = SIGUSR1; //метод уведомления

	if(aio_read(aioHandler) == -1)
	{
		printf("AIO_READ ERROR at %s\n", strerror(errno));
		exit(1);
	}

	while ((status = aio_error(aioHandler)) == EINPROGRESS);
	status = aio_error(aioHandler);
	if(status != 0) {
		printf("error at %s aio_error : %s\n", fileName, strerror(status));
		close(fd);
		exit(2);
	}
	status = aio_return(aioHandler);
	if(status == 0) {
		printf("error at aio_return\n");
		close(fd);
		exit(2);
	}
}

void writeLib(char resultFileName[sizeBuff], struct aiocb *aioHandler, int current_bytes, int all_bytes, char buffer[sizeBuff])
{
	int fd = open(resultFileName, O_WRONLY | O_CREAT);
	int status;
	aioHandler->aio_fildes = fd;
	aioHandler->aio_buf = buffer;
	aioHandler->aio_offset = all_bytes;
	aioHandler->aio_nbytes = current_bytes;
	aioHandler->aio_lio_opcode = LIO_READ;

	if(aio_write(aioHandler) == -1) {
		printf("error at aio_write: %s\n", strerror(errno));
		close(fd);
		exit(2);
	}	

	while(aio_error(aioHandler) == EINPROGRESS);

	status = aio_error(aioHandler);
	if(status != 0) {
		printf("error at aio_error(write): %s\n", strerror(errno));
		close(fd);
		exit(2);
	}
}
