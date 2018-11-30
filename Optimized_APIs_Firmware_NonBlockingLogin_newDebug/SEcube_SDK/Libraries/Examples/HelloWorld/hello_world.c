#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../secube-host/L1.h"

static uint8_t pin_admin[32] = {
	'a','d','m','i',  'n',0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

int main() {
	se3_disco_it it;
	se3_device dev;
	se3_session s;
	uint16_t return_value = 0;
	bool logged_in = false;

	printf("Welcome to SEcube Hello World!");
	printf("\n\n\n");
	Sleep(1000);

	printf("Looking for SEcube devices...\n");
	Sleep(3000);

	L0_discover_init(&it);
	if (L0_discover_next(&it)) {
		printf("SEcube device found!");
		return_value = L0_open(&dev, &(it.device_info), SE3_TIMEOUT);
	}
	if (return_value != SE3_OK) {
		printf("Error opening device\nPlease check board connection\n\n");
		return(!SE3_OK);
	}
	else {
		printf("Open Device success\n");
	}

	/* Log in */
	printf("Logging in as admin...\n");
	return_value = L1_login(&s, &dev, pin_admin, SE3_ACCESS_ADMIN);
	if (return_value != SE3_OK) {
		printf("Error Login\n");
		return(!SE3_OK);
	}
	else {
		printf("Login success\n");
		logged_in = true;
	}
	/* */


	if (logged_in) printf("You are logged in SEcube device! Hello World!!!");


	/* Log out */
	printf("Trying to log out\n");
	return_value = L1_logout(&s);
	if (return_value != SE3_OK) {
		printf("Error Logout\n");
		return(!SE3_OK);
	}
	else {
		printf("Logout success\n");
		printf("\n\nConnection to SEcube device was successfull\n");
		printf("Press [ENTER] to finish\n");
	}
	/* */

	getchar();

	return (0);
}
