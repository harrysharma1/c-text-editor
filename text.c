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
struct editor_config {
	struct termios origin_termios;
};

struct editor_config E;
/* Terminal  */
void killed(const char *s){
	perror(s);
	exit(1);
}


void raw_mode_disabled(){
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.origin_termios) == -1) killed("tcsetattr");
}

void raw_mode_enabled(){
	if (tcgetattr(STDIN_FILENO, &E.origin_termios) == -1) killed("tcgetattr");
	atexit(raw_mode_disabled);
	struct termios raw = E.origin_termios;
	raw.c_oflag &= ~(OPOST);
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
	raw.c_cflag |= (CS8);
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) killed("tcsetattr");
}


char editor_read_key(){
	int nread;
	char c;
	while ((nread = read(STDIN_FILENO, &c,1)) != 1){
		if (nread == -1 && errno != EAGAIN) killed("read");
	}

	return c;
}

/* Input  */
void editor_keypress_process(){
	char c = editor_read_key();

	switch (c) {
		case CTRL_KEY('q'):
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			exit(0);
			break;
	}
}

/* Output  */
void editor_draw_rows(){
	int i;
	for (i=0;i<24;i++){
		write(STDOUT_FILENO,">\r\n",3);
	}
}

void editor_refresh_screen(){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);

	editor_draw_rows();

	write(STDOUT_FILENO, "\x1b[H",3);
}



/* Init */
int main()
{
	raw_mode_enabled();
	char c;
	while (1) {
		editor_refresh_screen();
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
