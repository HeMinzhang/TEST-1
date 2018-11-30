#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "../secube-host/L1.h"

static uint8_t pin[32] = {
	'a','b','c','d',  0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
static uint8_t pin0[32] = {
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
static uint8_t pin1[32] = {
	't','e','s','t',  0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
static uint8_t pin2[32] = {
	't','e','s','t',  0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};
static uint8_t pin_aaaa[32] = {
	'a','a','a','a', 0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

void print_sn(uint8_t* v);
void test_randbuf(size_t len, void** buf);
bool test_echo(se3_device* dev);
uint16_t test_login(se3_device *dev);
uint16_t test_algorithm(se3_device *dev);


int main() {
	se3_disco_it it;
	se3_device dev;
	uint16_t return_value = 0;


	printf("Looking for SEcube devices...\n");
	Sleep(3000);
	
	
	/* List all SEcube attached */
	L0_discover_init(&it);
	while (L0_discover_next(&it)) {
		printf("SEcube found!\nInfo:\n");
		printf("Path:\t %ls\n", it.device_info.path);
		printf("Serial Number: ");
		print_sn(it.device_info.serialno);
		printf("\n\n");
	}


	printf("TEST: Open Device\n");
	L0_discover_init(&it);
	if (!L0_discover_next(&it)) {
		return_value = L0_open(&dev, &(it.device_info), SE3_TIMEOUT);
	}
	if (return_value != SE3_OK) {
		printf("Error opening device\nAbort Tests");
		return(0);
	}
	else
	{
		printf("Open Device success\n");
	}
	printf("---END TEST: Open Device\n\n");


	printf("TEST: Echo\n");
	if (test_echo(&dev)) {
		printf("Echo success\n");
	}
	else {
		printf("Error Echo\n");
	}
	printf("---END TEST: Echo\n\n");


	printf("TEST: Login\n");
	return_value = test_login(&dev);
	if (return_value != SE3_OK) {
		printf("Error Login\n");
	}
	else {
		printf("Login success\n");
	}
	printf("---END TEST: Login\n\n");


	printf("TEST: Algorithms\n");
	return_value = test_algorithm(&dev);
	if (return_value != SE3_OK) {
		printf("Error Algorithms\n");
	}
	else {
		printf("Algorithm list success\n");
	}
	printf("---END TEST: Algorithms\n\n");


	getchar();

	return (0);
}




#define NRUN 10
bool test_echo(se3_device* dev) {
	enum {
		TEST_SIZE = 1 * 1024 * 1024
	};
	bool success = false;
	size_t rep;
	uint8_t* sendbuf, *recvbuf;
	size_t n, i;
	uint16_t r = SE3_OK;
	uint8_t* sp, *rp;
	

	test_randbuf(TEST_SIZE, &sendbuf);
	recvbuf = (uint8_t*)malloc(TEST_SIZE);
	
	for (rep = 0; rep < NRUN; rep++) {
		printf("Echo - Run %d... ", rep);
		sp = sendbuf;
		rp = recvbuf;
		n = TEST_SIZE / SE3_REQ_MAX_DATA;
		for (i = 0; i < n; i++) {
			r = L0_echo(dev, sp, SE3_REQ_MAX_DATA, rp);
			if (SE3_OK != r) goto cleanup;
			sp += SE3_REQ_MAX_DATA;
			rp += SE3_REQ_MAX_DATA;
		}
		n = TEST_SIZE%SE3_REQ_MAX_DATA;
		if (n > 0) {
			L0_echo(dev, sp, (uint16_t)n, rp);
			if (SE3_OK != r) goto cleanup;
		}

		if (memcmp(sendbuf, recvbuf, TEST_SIZE)) goto cleanup;
		printf("   -> OK\n");
	}

	printf("\n");


	success = true;
cleanup:
	free(sendbuf);
	free(recvbuf);
	return success;
}



void test_randbuf(size_t len, void** buf) {
	*buf = malloc(len);
	se3c_rand(len, *buf);
}



void print_sn(uint8_t* v) {
	size_t i;
	for (i = 0; i < SE3_SERIAL_SIZE; i++) {
		printf("%u ", (unsigned)v[i]);
	}
	printf("\n");


	return;
}



#define MAX_INIT_PIN 32
uint16_t test_login(se3_device *dev) {

	uint16_t return_value = 0;
	uint8_t init_pin[MAX_INIT_PIN];
	se3_session s;
	memset(init_pin, 0, MAX_INIT_PIN);

	// Log in with default pin (all 0) - It should fail when initialized
	printf("Logging in as user with default PIN...\t");
	return_value = L1_login(&s, dev, init_pin, SE3_ACCESS_USER);
	if (SE3_OK == return_value) {
		printf("failure\n");
		L1_logout(&s);
		return(!SE3_OK);
	}
	else {
		printf("success\n");
	}

	// Log in as User
	printf("Logging in as user...\t");
	return_value = L1_login(&s, dev, pin, SE3_ACCESS_USER);
	if (SE3_OK == return_value){ 
		printf("success\n");
		L1_logout(&s);
	}
	else {
		printf("failure\n");
	}

	// Log in as Admin
	printf("Logging in as admin...\t");
	return_value = L1_login(&s, dev, pin, SE3_ACCESS_ADMIN);
	if (SE3_OK == return_value) {
		printf("success\n");
		L1_logout(&s);
	}
	else {
		printf("failure\n");
	}

	// Log in as User with WRONG pin
	printf("Logging in as user with WRONG PIN...\t");
	return_value = L1_login(&s, dev, pin_aaaa, SE3_ACCESS_USER);
	if (SE3_OK == return_value) {
		printf("failure\n");
		L1_logout(&s);
		return(!SE3_OK);
	}
	else {
		printf("success\n");
	}


	return return_value ? 0 : 1;
}



#define MAX_ALG 10
uint16_t test_algorithm(se3_device *dev) {
	uint16_t return_value = SE3_OK, count = 0;
	se3_session s;
	se3_algo alg_array[MAX_ALG];
	int i = 0;

	// Login as User
	return_value = L1_login(&s, dev, pin, SE3_ACCESS_USER);	

	printf("Retrieving algorithm list...\n");
	return_value = L1_get_algorithms(&s, 0, MAX_ALG, alg_array, &count);
	if (return_value != SE3_OK) {
		printf("Error retrieving algorithm list");
		return(!SE3_OK);
	}

	char buff_name[SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE + 1];
	buff_name[SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE] = 0;

	for (i = 0; i < count; i++) {
		memcpy(buff_name, alg_array[i].name, SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE);
		printf("Algorithm name: %s\n", buff_name);
		printf("Algorithm type: %u\n", (unsigned int)alg_array[i].type);
		printf("Algorithm block size: %u\n", (unsigned int)alg_array[i].block_size);
		printf("Algorithm key size: %u\n\n\n", (unsigned int)alg_array[i].key_size);
	}

	//Logout
	return_value = L1_logout(&s);									

	return return_value;
}

