#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios originTermios;

void killed(const char *s){
	perror(s);
	exit(1);
}


void rawModeDisabled(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &originTermios) == -1) killed("tcsetattr");
}

void rawModeEnabled(){
	if (tcgetattr(STDIN_FILENO, &originTermios) == -1) killed("tcgetattr");
	atexit(rawModeDisabled);
	struct termios raw = originTermios;
	raw.c_oflag &= ~(OPOST);
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) killed("tcsetattr");
}

int main()
{
	rawModeEnabled();
	char c;
	while (1) {
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) killed("read");
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			 printf("%d ('%c')\r\n", c, c);
		}
		if (c == 'q') break;
	}
	return 0;
}
