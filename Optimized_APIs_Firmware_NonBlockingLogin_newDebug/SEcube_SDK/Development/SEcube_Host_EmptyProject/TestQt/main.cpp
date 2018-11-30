#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "V:\SECube\SEcube_Host_EmptyProject\secube-host\L1.h"

#pragma warning(disable : 4996)

void print_sn(uint8_t* v);
bool init_device(se3_device_info* devinfo);


static uint8_t pin_default[32] = {
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

static uint8_t pin_admin[32] = {
    'a','d','m','i', 'n',0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

static uint8_t pin_user[32] = {
    'u','s','e','r', 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

uint8_t my_sn[32] = {
    1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16,
    1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16
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

uint8_t data_key_nicola[32] = {
    0x6f,0xf6,0xe4,0x95,0x9d,0xde,0x3b,0xb0,
    0xbd,0x3a,0xfb,0xa1,0x7e,0xee,0x93,0xd9,
    0x52,0x56,0x79,0xa6,0x5c,0x4c,0xf9,0x97,
    0x1e,0xdf,0x1f,0x8d,0x2b,0x68,0xcf,0x25
};



/**
*  \brief Example for initialising a SEcube device
*
*  \return void
*
*  \details Note: ALL the SEcube devices found are initialised
*/
int main() {
    se3_disco_it it;

    printf("Looking for SEcube devices...\n");

    L0_discover_init(&it);

    while (L0_discover_next(&it)) {
        printf("SEcube found!\nInfo:\n");
        printf("Path:\t %ls\n", it.device_info.path);
        printf("Serial Number: ");
        print_sn(it.device_info.serialno);

        if (!init_device(&it.device_info)) {
            printf("Device already initialised\n");
        }
        else {
            printf("Initialisation successfull\n");
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
*  \brief Initialisation of the SEcube Device
*
*  \param [in] devinfo SEcube Device Information structure
*  \return true if succeeded; false otherwise

*  \details The PIN used are pin_admin for admin and pin_user for user.
*  These are defined at the beginning of this file.
*/
bool init_device(se3_device_info* devinfo) {
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

    /***** Initialisation *****/
    if (SE3_OK != L0_factoryinit(&dev, my_sn)) {
        goto cleanup;
    }
    /* */


    /***** Change user & admin PIN *****/
    // First time, login as admin with default PIN (all zeroes)
    if (SE3_OK != L1_login(&session, &dev, pin_default, SE3_ACCESS_ADMIN)) {
        goto cleanup;
    }
    logged_in = true;
    /* */


    /***** Change user & admin PIN *****/
    if (SE3_OK != L1_set_user_PIN(&session, pin_user)) {
        goto cleanup;
    }
    if (SE3_OK != L1_set_admin_PIN(&session, pin_admin)) {
        goto cleanup;
    }
    /* */


    /***** Add keys *****/
    // Key 1
    strcpy((char*) key.name, "TestKey_1");
    key.id = 1;
    key.name_size = strlen((char*) key.name);
    key.data_size = 32;
    key.data = data_key_nicola;
    key.validity = (uint32_t)time(0) + 365 * 24 * 3600;

    if (SE3_OK != L1_key_edit(&session, SE3_KEY_OP_UPSERT, &key)) {
        goto cleanup;
    }
    // --

/*
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
