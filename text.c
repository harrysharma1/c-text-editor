#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

struct termios originTermios;

void rawModeDisabled(){
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &originTermios);
}

void rawModeEnabled(){
	tcgetattr(STDIN_FILENO, &originTermios);
	atexit(rawModeDisabled);
	struct termios raw = originTermios;
	raw.c_lflag &= ~(ECHO);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main()
{
	rawModeEnabled();
	char c;
	while(read(STDIN_FILENO,&c,1)==1 && c != 'q');
	return 0;
}
