
#include <stdlib.h>
#include <stdio.h>   /* Стандартные объявления ввода/вывода */
#include <string.h>  /* Объявления строковых функций */
#include <unistd.h>  /* Объявления стандартных функций UNIX */
#include <fcntl.h>   /* Объявления управления файлами */
#include <errno.h>   /* Объявления кодов ошибок */
#include <termios.h> /* Объявления управления POSIX-терминалом */
#include <unistd.h>  /* Для usleep */
#include <iconv.h>


// Объявление функций
int serialport_init(const char *serialport);

// отправка СМС
int write_serialport(int fd);

// Очистка порта
void clear_port(int fd);

// Чтения порта и поиск строк "OK" и ">"
int read_port(int fd);


//char *phone[] = {"89209656669","89605720332","89206372888",NULL};
//char *phone[] = {"89209656669","89206372888",NULL};
//char *phone[] = {"89209656669",NULL};

char *message = "041F04400438043204350442002100210021"; //Привет!!!
char *phone = "9702696566F9"; // Убрать +, Добавить F в конец и поменять соседние цифры местами 
char *port = "/dev/ttyUSB1";

int main(int argc, char *argv[])
{
	int fd, byte;
	
	fd = serialport_init(port);
	if(fd == -1)
	{
	  printf("Не могу инициализировать port\n");
	  return -1;
	}
	clear_port(fd);



	char buf[512];
	char text[512];
//	char enter = 13; 	// Enter
	char ctrl_z = 26; 	// ctrl-Z
	size_t lenMsg = strlen(message)/2;
	sprintf(text,"0001000B91%s0008%02X%s",phone,(int)lenMsg, message);
	
	size_t lenBuf = (strlen(text))/2-1;
	printf("Длина: %d\nMsg: %s\n",(int)lenBuf,text);
	
	// Передаем режим работы
	sprintf(buf,"AT+CMGF=0\r");	// режим работы: 0-цифровой или 1-текстовый
	byte = write(fd, &buf, strlen(buf)); 				// записываем данные
	printf("Передано %d байт TEXT: %s\n", byte,buf);

	usleep(1000000); // 0,5 сек
	if(read_port(fd))
	{
		printf("Не могу прочитать port\n");
		return -1;
	}
	// Передаем размер сообщения
	sprintf(buf,"AT+CMGS=%d\r",(int)lenBuf);					// режим работы: 0-цифровой или 1-текстовый
	byte = write(fd, &buf, strlen(buf)); 				// записываем данные
	printf("Передано %d байт TEXT: %s\n", byte,buf);

	usleep(2000000); // 0,5 сек
	if(read_port(fd))
	{
		printf("Не могу прочитать port\n");
		return -1;
	}
	// Передаем текст сообщения
	sprintf(buf,"%s%c",text,ctrl_z); 	// Номер телефона
	byte = write(fd, &buf, strlen(buf)); 				// записываем данные
	printf("Передано %d байт TEXT: %s\n", byte,buf);

	usleep(2000000); // 0,5 сек
	if(read_port(fd))
	{
		printf("Не могу прочитать port\n");
		return -1;
	}

	
	close(fd);

	return 0;
}

// Чтения порта и поиск строк "OK" и ">"
int read_port(int fd)
{
	int byte = 0;
	char buf[512];

	while(byte != -1)
	{
		byte = read(fd, buf, 512);
	}
	printf("Прочитано TEXT: %s\n", buf);

	//Поиск строки
	if(!((strstr (buf,"OK")) || (strstr (buf,">"))))
	{
		printf("Ошибка при отправке смс...\n");
		return -1;
	}
	return 0;
}

// функция настройки serial port
int serialport_init(const char* serialport)
{

    struct termios options;
    int fd;

    fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);

    usleep(2000000); // задержка 2 сек

    if (fd == -1)  {
        perror("Unable to open port");
        return -1;
    }

    // Настройка serial порта

    fcntl(fd, F_SETFL, FNDELAY);

    // Получение текущих опций для порта...

     tcgetattr(fd, &options);

    //Установки скорости

//    cfsetispeed(&options, B9600);
//    cfsetospeed(&options, B9600);

		cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    //Отсутствие проверки на четность 8N1
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Установка аппаратного управления потоком передаваемых данных

    options.c_cflag &= ~CRTSCTS; //Отключено

    options.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    options.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    options.c_oflag &= ~OPOST; // make raw

    options.c_cc[VMIN]  = 0;
    options.c_cc[VTIME] = 15;



    if( tcsetattr(fd, TCSANOW, &options) < 0)
    {
        perror("Couldn't set term attributes");
        return -1;
    }

    return fd;
}


// Очистка порта
void clear_port(int fd)
{
	char buffer;
	int n, i=1;
	while(n != -1)
	{
		printf("очистка serial port %d\n", i++);
		n = read(fd, &buffer, 1);
	}
}





