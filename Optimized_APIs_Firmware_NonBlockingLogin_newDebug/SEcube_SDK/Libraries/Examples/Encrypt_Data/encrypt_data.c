#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../secube-host/L1.h"


#pragma warning(disable : 4996)  


#define TEST_STRING   ("test_encryption")
#define TEST_SIZE     strlen(TEST_STRING)
#define ENC_SIZE(plain_size)    (((plain_size/SE3_L1_CRYPTOBLOCK_SIZE)+1)*SE3_L1_CRYPTOBLOCK_SIZE)


void print_sn(uint8_t* v);


static uint8_t pin_admin[32] = {
	'a','d','m','i', 'n',0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

static uint8_t pin_user[32] = {
	'u','s','e','r', 0,0,0,0, 0,0,0,0, 0,0,0,0,
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
*  \brief Example for encrypting data with a SEcube device
*
*  \return int
*
*/
int main() {
	se3_disco_it it;
	se3_device dev;
	se3_session s;

	uint32_t session_id = 0;
	
	uint8_t *buffer;
	uint8_t *enc_buffer = 0;
	uint8_t *dec_buffer;
	uint16_t buffer_len = 0;
	uint16_t enc_buffer_len = 0;
	uint16_t dec_buffer_len = 0;

	uint16_t return_value = 0;

	printf("Looking for SEcube devices...\n");
	Sleep(3000);

	/* Find SEcube devices */
	L0_discover_init(&it);
	while (L0_discover_next(&it)) {
		printf("SEcube found!\nInfo:\n");
		printf("Path:\t %ls\n", it.device_info.path);
		printf("Serial Number: ");
		print_sn(it.device_info.serialno);

		/* Open SEcube device */
		return_value = L0_open(&dev, &(it.device_info), SE3_TIMEOUT);
		if (SE3_OK != return_value) {
			printf("Failure to open device\n");
			return(!SE3_OK);
		}
		else {
			printf("Device is now open\n");
		}
		/* */


		/* Log in as user */
		return_value = L1_login(&s, &dev, pin_user, SE3_ACCESS_USER);
		if (SE3_OK != return_value) {
			printf("Failure to log in as user\n");
			return(!SE3_OK);
		}
		else {
			printf("Logged in as user\n");
		}
		
		printf("\n\n");
		/* */


		/* Set time for crypto functions */
		return_value = L1_crypto_set_time(&s, (uint32_t)time(0));
		if (SE3_OK != return_value) {
			printf("Failure to set time\n");
			L1_logout(&s);
			return(!SE3_OK);
		}
		else {
			printf("Time set\n");
		}
		/* */



/* Encrypt data */

		/* Initialize encryption */
		return_value = L1_crypto_init(&s, SE3_ALGO_AES, SE3_DIR_ENCRYPT | SE3_FEEDBACK_ECB, 1, &session_id);
		if (SE3_OK != return_value) {
			printf("Failure initialise crypto session\n");
			L1_logout(&s);
			return(!SE3_OK);
		}
		else {
			printf("Encryption Session initialised\n");
		}

		buffer_len = TEST_SIZE;
		buffer = (uint8_t*) calloc (ENC_SIZE(buffer_len), sizeof(uint8_t));
		strncpy(buffer, TEST_STRING, buffer_len);
		enc_buffer = (uint8_t*) calloc (ENC_SIZE(buffer_len), sizeof(uint8_t));
		/* */


		/* Encrypt and finalise encryption session */
		return_value = L1_crypto_update(&s, session_id, SE3_DIR_ENCRYPT | SE3_FEEDBACK_ECB | SE3_CRYPTO_FLAG_FINIT, 
									    0, NULL, ENC_SIZE(buffer_len), buffer, &enc_buffer_len, enc_buffer);
		if (SE3_OK != return_value) {
			printf("Failure encrypt data \n");
			L1_logout(&s);
			return(!SE3_OK);
		}
		else {
			printf("Data encrypted\n");
		}
		// SE3_CRYPTO_FLAG_FINIT is set because the data contained in buffer can be encrypted in one call
/* */


/* Decrypt data */

		/* Initialize Decryption */
		return_value = L1_crypto_init(&s, SE3_ALGO_AES, SE3_DIR_DECRYPT | SE3_FEEDBACK_ECB, 1, &session_id);
		if (SE3_OK != return_value) {
			printf("Failure initialise crypto session\n");
			L1_logout(&s);
			return(!SE3_OK);
		}
		else {
			printf("Decryption Session initialised\n");
		}

		dec_buffer = (uint8_t*) calloc (ENC_SIZE(buffer_len), sizeof(uint8_t));
		/* */

		/* Decrypt and finalise encryption session */
		return_value = L1_crypto_update(&s, session_id, SE3_DIR_DECRYPT | SE3_FEEDBACK_ECB | SE3_CRYPTO_FLAG_FINIT,
			0, NULL, enc_buffer_len, enc_buffer, &dec_buffer_len, dec_buffer);
		if (SE3_OK != return_value) {
			printf("Failure to decrypt data \n");
			L1_logout(&s);
			return(!SE3_OK);
		}
		else {
			printf("Data decrypted\n");
		}
		// SE3_CRYPTO_FLAG_FINIT is set because the data contained in buffer can be encrypted in one call

/* */

		/* Compare plain data with decrypted data */
		if (memcmp(buffer, dec_buffer, ENC_SIZE(buffer_len)) == 0) {
			printf("Data match\n");
		}
		else {
			printf("Data NOT match\n");
		}
		/* */


		/* Log out */
		return_value = L1_logout(&s);
		if (SE3_OK != return_value) {
			printf("Failure to log out\n");
			return(!SE3_OK);
		}
		else {
			printf("Logged out\n");
		}
		/* */
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