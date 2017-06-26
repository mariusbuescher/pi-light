# include <stdio.h>
# include <wiringPi.h>
# include <lcd.h>

# include<string.h>
# include<stdlib.h>
# include<errno.h>
# include<arpa/inet.h>
# include<sys/socket.h>

# define BUFLEN 1
# define PORT 8888
# define LED_PIN 15
# define BUTTON_PIN 8

# define LCD_ROWS 2
# define LCD_COLS 16
# define LCD_BITS 4

typedef int bool;
#define true 1
#define false 0

bool light_on = false;
int lcd_handle;

struct sockaddr_in si_me, si_other;
int s, i, slen = sizeof(si_other) , recv_len;
char buf[BUFLEN];

void die(char *s)
{
    perror(s);
    exit(1);
}

void turn_on_light() {
	digitalWrite(LED_PIN, 1);
	printf("Turn on the lights\n");
	lcdHome(lcd_handle);
	lcdClear(lcd_handle);
	lcdPrintf(lcd_handle, "Lights are on");
	light_on = true;
}

void turn_off_light() {
	digitalWrite(LED_PIN, 0);
	printf("Turn off the lights\n");
	lcdHome(lcd_handle);
	lcdClear(lcd_handle);
	lcdPrintf(lcd_handle, "Lights are off");
	light_on = false;
}

void toggle_light() {
	if (light_on == true)
	{
		turn_off_light();
	}
	else if (light_on == false)
	{
		turn_on_light();
	}
}

void send_status() {
	char buf[BUFLEN];

	if (light_on == true)
	{
		buf[0] = '1';
	}
	else
	{
		buf[0] = '0';
	}

	if (sendto(s, buf, sizeof(buf), 0, (struct sockaddr*) &si_other, slen) == -1)
	{
		die("sendto()");
	}
}

int main() {
	if (wiringPiSetup() == -1)
	    return 1;


	pinMode(LED_PIN, OUTPUT);

	if ( wiringPiISR(BUTTON_PIN, INT_EDGE_FALLING, &toggle_light) < 0 ) {
		fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
		die("interrupt()");
	}

	lcd_handle = lcdInit(LCD_ROWS, LCD_COLS, LCD_BITS, 11, 10, 0, 1, 2, 3, 0, 0, 0, 0);

	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
	    die("socket");
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s, (struct sockaddr*)&si_me, sizeof(si_me)) == -1)
	{
		die("bind");
	}

	digitalWrite(LED_PIN, 0);

	lcdHome(lcd_handle);
	lcdClear(lcd_handle);
	lcdPrintf(lcd_handle, "Waiting for data");

	while(1)
	{
		printf("Waiting for data\n");

		if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			die("recvfrom()");
		}
		
		char request = buf[0];

		if (request == '1')
		{
			 turn_on_light();
		}
		else if (request == 't')
		{
			toggle_light();
		}
		else if (request == '0')
		{
			turn_off_light();
		}
		
		send_status();

		fflush(stdout);
	}

	close(s);
	
	return 0;
}

