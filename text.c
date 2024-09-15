/* Include */
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

/* Definition */
#define CTRL_KEY(k) ((k) & 0x1f)

/* Data  */
struct termios originTermios;

/* Terminal  */
void killed(const char *s){
	perror(s);
	exit(1);
}


void raw_mode_disabled(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &originTermios) == -1) killed("tcsetattr");
}

void raw_mode_enabled(){
	if (tcgetattr(STDIN_FILENO, &originTermios) == -1) killed("tcgetattr");
	atexit(raw_mode_disabled);
	struct termios raw = originTermios;
	raw.c_oflag &= ~(OPOST);
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) killed("tcsetattr");
}


char editor_read_key() {
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c,1)) != 1){
		if (nread == -1 && errno != EAGAIN) killed("read");
	}

	return c;
}

void editor_keypress_process(){
	char c = editor_read_key();

	switch (c) {
		case CTRL_KEY('q'):
			exit(0);
			break;
	}
}

/* Init */
int main()
{
	raw_mode_enabled();
	char c;
	while (1) {
		editor_keypress_process();
		char c = '\0';
		if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) killed("read");
		if (iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			 printf("%d ('%c')\r\n", c, c);
		}
		if (c == CTRL_KEY('q')) break;
	}
	return 0;
}
