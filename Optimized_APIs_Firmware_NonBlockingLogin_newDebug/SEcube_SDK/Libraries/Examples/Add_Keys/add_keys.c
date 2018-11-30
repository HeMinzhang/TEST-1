#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../secube-host/L1.h"


#pragma warning(disable : 4996)  


void print_sn(uint8_t* v);
bool add_keys(se3_device_info* devinfo);


static uint8_t pin_admin[32] = {
	'a','d','m','i', 'n',0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

uint8_t data_key_1[32] = {
	1,2,3,4, 1,2,3,4, 1,2,3,4, 1,2,3,4,
	1,2,3,4, 1,2,3,4, 1,2,3,4, 1,2,3,4
};

uint8_t data_key_2[32] = {
	5,6,7,8, 5,6,7,8, 5,6,7,8, 5,6,7,8,
	5,6,7,8, 5,6,7,8, 5,6,7,8, 5,6,7,8
};

uint8_t data_key_3[32] = {
	0,9,1,8, 0,9,1,8, 0,9,1,8, 0,9,1,8,
	0,9,1,8, 0,9,1,8, 0,9,1,8, 0,9,1,8
};


/**
*  \brief Example for adding keys to a SEcube device
*
*  \return void
*
*  \details Note: The keys will be inserted in ALL the SEcube devices found
*/
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

		if (add_keys(&it.device_info)) {
			printf("Keys Added\n");
		}
		else {
			printf("Failure\n");
		}
	}

	getchar();

	return (0);
}


/**
*  \brief Print Serial Number in readable format
*
*  \param [in] v Array containing serial number
*  \return void
*
*/
void print_sn(uint8_t* v) {
	size_t i;
	for (i = 0; i < SE3_SERIAL_SIZE; i++) {
		printf("%u ", (unsigned)v[i]);
	}
	printf("\n");


	return;
}



/**
*  \brief Example: Add keys to SEcube Device
*
*  \param [in] devinfo SEcube Device Information structure
*  \return true if succeeded; false otherwise
*
*/
bool add_keys(se3_device_info* devinfo) {
	se3_device dev;
	se3_session session;
	se3_key key;
	bool success = false;
	bool logged_in = false;

	/***** Open SEcube device *****/
	if (SE3_OK != L0_open(&dev, devinfo, 1000)) {
		return false;
	}
	/* */


	/***** Change user & admin PIN *****/
	// First time, login as admin with default PIN (all zeroes)
	if (SE3_OK != L1_login(&session, &dev, pin_admin, SE3_ACCESS_ADMIN)) {
		goto cleanup;
	}
	logged_in = true;
	/* */


	/***** Add keys *****/
	// Key 1
	strcpy(key.name, "TestKey_1");
	key.id = 1;
	key.name_size = strlen(key.name);
	key.data_size = 32;
	key.data = data_key_1;
	key.validity = (uint32_t)time(0) + 365 * 24 * 3600;

	if (SE3_OK != L1_key_edit(&session, SE3_KEY_OP_UPSERT, &key)) {
		goto cleanup;
	}
	// --

	// Key 2
	strcpy(key.name, "TestKey_2");
	key.id = 2;
	key.name_size = strlen(key.name);
	key.data_size = 32;
	key.data = data_key_2;
	key.validity = (uint32_t)time(0) + 365 * 24 * 3600;

	if (SE3_OK != L1_key_edit(&session, SE3_KEY_OP_UPSERT, &key)) {
		goto cleanup;
	}
	// --

	// Key 3
	strcpy(key.name, "TestKey_3");
	key.id = 3;
	key.name_size = strlen(key.name);
	key.data_size = 32;
	key.data = data_key_3;
	key.validity = (uint32_t)time(0) + 365 * 24 * 3600;

	if (SE3_OK != L1_key_edit(&session, SE3_KEY_OP_UPSERT, &key)) {
		goto cleanup;
	}
	// --
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