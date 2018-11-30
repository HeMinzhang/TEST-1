#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../secube-host/L1.h"

#pragma warning(disable : 4996)  

void print_sn(uint8_t* v);
bool init_device(se3_device_info* devinfo);


static uint8_t pin[32] = {
	't','e','s','t',  0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};



int main() {
	se3_disco_it it;

	printf("Looking for SEcube devices...\n");
	Sleep(3000);

	L0_discover_init(&it);

	while (L0_discover_next(&it)) {
		printf("SEcube found!\nInfo:\n");
		printf("Path:\t %ls\n", it.device_info.path);
		printf("Serial Number: ");
		print_sn(it.device_info.serialno);

		if (!init_device(&it.device_info)) {
			printf("Device already initialised\n");
			printf("PIN set to: test\n");
		}
		else {
			printf("Initialisation successfull\n");
		}
	}

	getchar();

	return (0);
}



void print_sn(uint8_t* v) {
	size_t i;
	for (i = 0; i < SE3_SERIAL_SIZE; i++) {
		printf("%u ", (unsigned)v[i]);
	}
	printf("\n");


	return;
}



bool init_device(se3_device_info* devinfo) {
	se3_device dev;
	se3_session session;
	se3_key key;
	bool success = false;
	bool logged_in = false;

	uint8_t test_sn[32] = {
		1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16,
		1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16
	};

	uint8_t test_data[32] = {
		1,2,3,4, 1,2,3,4, 1,2,3,4, 1,2,3,4,
		1,2,3,4, 1,2,3,4, 1,2,3,4, 1,2,3,4
	};

	uint8_t pin0[32] = {
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
	};


	/***** Open SEcube device *****/
	if (SE3_OK != L0_open(&dev, devinfo, 1000)) {
		return false;
	}
	/* */


	/***** Initialisation *****/
	if (SE3_OK != L0_factoryinit(&dev, test_sn)) {
		goto cleanup;
	}
	/* */


	/***** Change user & admin PIN *****/
	// First time, login as admin with default PIN (all zeroes)
	if (SE3_OK != L1_login(&session, &dev, pin0, SE3_ACCESS_ADMIN)) {
		goto cleanup;
	}
	logged_in = true;
	/* */


	/***** Change user & admin PIN *****/
	if (SE3_OK != L1_set_user_PIN(&session, pin)) {
		goto cleanup;
	}
	if (SE3_OK != L1_set_admin_PIN(&session, pin)) {
		goto cleanup;
	}
	/* */


	/***** Add a key *****/
	strcpy(key.name, "TestKey");
	key.id = 3;
	key.name_size = strlen(key.name);
	key.data_size = 32;
	key.data = test_data;
	key.validity = (uint32_t)time(0) + 365 * 24 * 3600;

	if (SE3_OK != L1_key_edit(&session, SE3_KEY_OP_UPSERT, &key)) {
		goto cleanup;
	}
	/* */


	/***** Clean and Logout *****/
	success = true;
cleanup:
	if (logged_in) {
		L1_logout(&session);
	}
	L0_close(&dev);
	return success;
	/* */
}