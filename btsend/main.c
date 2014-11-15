/**
 * @file main.c
 * @brief Start point for the program
 *
 * @author Brooks Kindle
 * @date Nov. 12th, 2014
 */

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <ncurses.h>

#define EXIT_KEY '\3'
#define NOOP_KEY ' '

int main(int argc, char **argv) {
	char ch = '\0';
	bool done = false;
	const char *devname = NULL;
	FILE *dev = NULL;

	if(argc != 2) {
		fprintf(stdout, "Usage: %s device_name\n", argv[0]);
		fprintf(stdout, "device_name, for example, can be /dev/tty1\n");
		return 1;
	}

	devname = argv[1];
	dev = fopen(devname, "w");
	if(!dev) { //unable to open terminal
		fprintf(stderr, "Fatal: unable to open %s for writing\n", devname);
		return 1;
	}
	////////////////
	//init ncurses//
	////////////////
	initscr();
	raw();
	noecho(); //prevent echoing user input
	timeout(250); //don't block on user input (wait 1/4 of a second for input)

	printw("Welcome to BTSEND...");
	printw("Press Ctrl-C to exit\n");
	refresh();
	sleep(1);
	while(!done) {
		ch = getch();
		clear();
		printw("Welcome to BTSEND...");
		printw("Press Ctrl-C to exit\n");
		printw("====================\n");
		refresh();
		switch(ch) {
			case EXIT_KEY:			//user wishes to quit
				done = true;
				break;
			case ERR:				//no key pressed (-1)
				ch = NOOP_KEY;		//send a predefined no-op key
				printw("No key pressed, sending no-op key\n");
			default:				//regular key pressed
				printw("Sending key '%c'\n", ch);
				fprintf(dev, "%c", ch); //send user input
				refresh();
				break;
		}
	}//end while
	clear();
	printw("Thank you for using btsend.\n");
	printw("Press <any> key to continue.\n");
	refresh();
	timeout(-1);
	getch();
	endwin();
	return 0;
}//end main
