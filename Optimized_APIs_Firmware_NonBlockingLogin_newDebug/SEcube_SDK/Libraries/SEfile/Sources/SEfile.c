/** \file SEfile.c
 *  \brief In this file you will find the implementation
 *         of the public function already described in
 *         \ref SEfile.h
 *   \authors Francesco Giavatto, Nicolò Maunero, Giulio Scalia
 *   \date 17/09/2016
 */

#ifdef __linux__
#define _GNU_SOURCE
#endif

#include "SEfile.h"

#define SEFILE_NONCE_LEN 32
/**
 * @brief The SEFILE_HANDLE struct
 *
 * This abstract data type is used to hide from higher level
 * of abstraction its implementation. The data stored in here
 * are the current physical file pointer position and the
 * file descriptor OS-dependent data type.
 */
struct SEFILE_HANDLE {
    uint32_t log_offset;
#if defined(__linux__) || defined(__APPLE__)
    int32_t fd;
#elif _WIN32
    HANDLE fd;
#endif
    uint8_t nonce_ctr[16];
    uint8_t nonce_pbkdf2[SEFILE_NONCE_LEN];
};

/**
 * @defgroup SectorStruct
 * @{
 */

#pragma pack(push,1)
/**
 * @brief The SEFILE_HEADER struct
 *
 * This data struct is used to define a 31 bytes field inside a sector
 * while taking care of its inner composition.
 */
///@{
typedef struct {
    uint8_t nonce_pbkdf2[SEFILE_NONCE_LEN];	/**< 32 random bytes storing the IV for generating a different key*/
    uint8_t nonce_ctr[16];		/**< 16 random bytes storing the IV for next sectors*/
    int32_t magic;				/**< 4 bytes used to represent file type (not used yet)*/
    int16_t ver;				/**< 2 bytes used to represent current filesystem version (not used yet)*/
    int32_t uid;				/**< 4 bytes not used yet*/
    int32_t uid_cnt;			/**< 4 bytes not used yet*/
    uint8_t fname_len;			/**< 1 byte to express how long is the filename.*/
} SEFILE_HEADER;
///@}
#pragma pack(pop)

#pragma pack(push,1)
/**
 * @brief The SEFILE_SECTOR struct
 * This data struct is the actual sector organization.
 * The total size should ALWAYS be equal to \ref SEFILE_SECTOR_SIZE.
 * The first sector is used to hold ONLY the header.
 * Thanks to the union data type, the developer can simply declare
 * a sector and then choose if it is the header sector or not.
 */
///@{
typedef struct {
    union {
        SEFILE_HEADER header;               /**< See \ref SEFILE_HEADER .*/
        uint8_t data[SEFILE_LOGIC_DATA];   /**< In here it will be written the actual data.
         *   Since it is inside a union data type, the
         *   filename will be written from 32nd byte.
         */
    };
    uint16_t len;                      /**< How many bytes are actually stored in this sector.*/
    uint8_t signature[32];
    /*uint32_t signature;                /**< Here we store the first 4 bytes computed by some
     *   digest algorithm.
     */
} SEFILE_SECTOR;

///@}
/** @}*/
#pragma pack(pop)
#define POS_TO_CIPHER_BLOCK(current_position) ((current_position / SEFILE_SECTOR_SIZE) - 1)*(SEFILE_SECTOR_DATA_SIZE / SEFILE_BLOCK_SIZE)
/**
 * @defgroup EnvironmentalVars
 * @{
 */
/** \name Environmental Variables
 * This static variables will store some data useful
 * during the active session.
 */
///@{
static se3_session *EnvSession=NULL;			/**< Which session we want to use*/
static int32_t *EnvKeyID=NULL;             		/**< Which KeyID we want to use*/
static uint16_t *EnvCrypto=NULL;	/**< Which cipher algorithm and mode we want to use*/
///@}
/** @}*/
/**
 * @brief This function extract the filename pointed by
 *        \ref path.
 * @param [in] path It can be both an absolute or relative
 *        path. No encrypted directory are allowed inside the path.
 * @param [out] file_name A preallocated string where to store
 *        the filename.
 */
void get_filename(char *path, char *file_name);
/**
 * @brief This function extract the path where the file is.
 * @param [in] full_path It can be both an absolute or relative
 *        path. No encrypted directory are allowed inside the path.
 * @param [out] path A preallocated string where to store
 *        the path
 */
void get_path(char *full_path, char *path);
/**
 * @brief This function check if the environmental variables
 *        are correctly initialized and set.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t check_env();
/**
 *  \brief This function encrypts the buff_decrypt data by exploiting the
 *        functions provided by \ref L1.h.
 *
 *  \param [in] buff_decrypt The plaintext data to be encrypted
 *  \param [out] buff_crypt  The preallocated buffer where to store the
 *        encrypted data.
 *  \param [in] datain_len Specify how many data we want to encrypt.
 *  \param [in] current_offset Current position inside the file expressed
 *  			as number of cipher blocks
 *  \param [in] nonce Initialization vector, see \ref SEFILE_HEADER
 *  \return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 *
 */
uint16_t crypt_sectors(void *buff_decrypt, void *buff_crypt, size_t datain_len, size_t current_offset, uint8_t* nonce_ctr, uint8_t* nonce_pbkdf2);


/**
 *  \brief This function encrypts a header buffer by exploiting the
 *        functions provided by \ref L1.h.
 *
 *  \param [in] buff1 Pointer to the header we want to encrypt/decrypt.
 *  \param [out] buff2 Pointer to an allocated header where to store the
 *  			result.
 *  \param [in] datain_len How big is the amount of data.
 *  \param [in] direction See \ref SE3_DIR.
 *  \return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 *
 */
uint16_t crypt_header(void *buff1, void *buff2, size_t datain_len, uint16_t direction);

//uint16_t decrypt_sectors(void *buff_crypt, void *buff_decrypt, uint32_t size);
/**
 *  \brief This function decrypts the buff_crypt data by exploiting the
 *        functions provided by \ref L1.h.
 *
 *  \param [in] buff_crypt The cipher text data to be decrypted
 *  \param [out] buff_decrypt  The preallocated buffer where to store the
 *        decrypted data.
 *  \param [in] datain_len Specify how many data we want to decrypt.
 *  \param [in] current_offset Current position inside the file expressed
 *  			as number of cipher blocks
 *  \param [in] nonce Initialization vector, see \ref SEFILE_HEADER
 *  \return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 *
 */
uint16_t decrypt_sectors(void *buff_crypt, void *buff_decrypt, size_t datain_len, size_t current_offset, uint8_t* nonce_ctr, uint8_t* nonce_pbkdf2);
/**
 * @brief This function is used to compute the total logic size of an open
 *        file handle.
 * @param [in] hFile Open file handle whose size shall be computed.
 * @param [out] length Pointer to a preallocated variable where to store
 *        the logic size.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t get_filesize(SEFILE_FHANDLE *hFile, uint32_t * length);
/**
 * @brief This function is used to compute the plaintext of a encrypted
 *        filename stored in path.
 * @param [in] path Where the encrypted file is stored, it can be an absolute
 *        or relative path. No encrypted directory are allowed inside the path.
 * @param [out] filename  A preallocated string where to store the plaintext
 *        filename
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t decrypt_filename(char *path, char *filename);
/**
 * @brief This function is used to compute the plaintext of a encrypted
 *        filename stored in an already open hFile header.
 * @param [in] hFile Already opened file handle to be read in order to obtain
 *        its plaintext filename.
 * @param [out] filename A preallocated string where to store the plaintext
 *        filename.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t decrypt_filehandle(SEFILE_FHANDLE *hFile, char *filename);
/**
 * @brief This function is used to compute the ciphertext of a directory
 *        name stored in dirname.
 * @param [in] dirpath Path to the directory whose name has to be encrypted.
 *        No encrypted directory are allowed inside the path.
 * @param [out] encDirname A preallocated string where to store the encrypted
 *        directory name
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t crypt_dirname(char *dirpath, char *encDirname);
/**
 * @brief This function is used to compute the plaintext of a encrypted
 *        directory name stored in dirname.
 * @param [in] dirpath Path to the directory whose name has to be decrypted.
 *        No encrypted directory are allowed inside the path.
 * @param [out] decDirname A preallocated string where to store the decrypted
 *        directory name
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t decrypt_dirname(char *dirpath, char *decDirname);

/**
 * @brief This function checks if the given name can be a valid encrypted
 *        filename/directory name.
 * @param [in] Name of the file/directory.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t valid_name(char *name);

uint16_t secure_init(se3_session *s, uint32_t keyID, uint16_t crypto){
    int i = 0, j = 0, count = 0;
    se3_key keyTable;
    se3_algo algTable[SE3_ALGO_MAX];
    uint16_t ret = 0;
    bool found = false;

    if(EnvSession != NULL || EnvKeyID != NULL || EnvCrypto != NULL ){
        return SEFILE_ENV_INIT_ERROR;
    }

    if(s==NULL){
        return SEFILE_ENV_INIT_ERROR;
    }

    if(!s->logged_in){
        return SEFILE_ENV_INIT_ERROR;
    }

    EnvSession=(se3_session *)malloc(sizeof(se3_session));
    EnvKeyID=(int32_t *)malloc(sizeof(int32_t));
    EnvCrypto = (uint16_t *)malloc(sizeof(uint16_t));
    *EnvCrypto = SE3_ALGO_MAX + 1; //No valid algorithm still set

    if(EnvSession == NULL || EnvKeyID == NULL || EnvCrypto == NULL ){
        return SEFILE_ENV_INIT_ERROR;
    }

    memcpy(EnvSession, s, sizeof(se3_session));
    if (keyID == -1){ //use first available KeyID
        i=0;
        do{
            if(L1_key_list(s, i, 1, &keyTable, &count)){
                secure_finit();
                return SEFILE_ENV_INIT_ERROR;
            }

            if(count == 0){
                secure_finit();
                return SEFILE_KEYID_NOT_PRESENT;
            }
            if(keyTable.validity>time(0)){
                *EnvKeyID=keyTable.id;
                found = true;
                break;
            }
            i++;
        }while(found == false);

        if (found == false){
            secure_finit();
            return SEFILE_KEYID_NOT_PRESENT;
        }
    }else{
        if(!L1_find_key(EnvSession, keyID)){
            secure_finit();
            return SEFILE_ENV_INIT_ERROR;
        }
        *EnvKeyID = keyID;
    }

    if(crypto!=(SE3_ALGO_MAX + 1)){
        if ((ret = L1_get_algorithms(EnvSession, 0, SE3_ALGO_MAX, algTable, &count)) == 0 && count > 0 && algTable[crypto].type == SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH){
            *EnvCrypto = crypto;
        }else{
            secure_finit();
            return SEFILE_ENV_INIT_ERROR;
        }
    } else {
        if ((ret = L1_get_algorithms(EnvSession, 0, SE3_ALGO_MAX, algTable, &count)) == 0 && count > 0){
            for (i = 0; i < count; i++){
                if (algTable[i].type == SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH) {
                    *EnvCrypto = i;
                    break;
                }
            }
            if (*EnvCrypto == SE3_ALGO_MAX + 1){
                secure_finit();
                return SEFILE_ENV_INIT_ERROR;
            }
        }else{
            secure_finit();
            return SEFILE_ENV_INIT_ERROR;
        }
    }

    return 0;
}

uint16_t secure_update(se3_session *s, int32_t keyID, uint16_t crypto){
    uint16_t count = 0, ret = 0;
    se3_key keyTable;
    se3_algo algTable[SE3_ALGO_MAX];

    if(check_env()){
        return SEFILE_ENV_UPDATE_ERROR;
    }

    if(s!=NULL){
        if(!s->logged_in){
            memcpy(EnvSession, s, sizeof(*s));
        }else{
            return SEFILE_ENV_UPDATE_ERROR;
        }
    }

    if(keyID!=-1){
        if(!L1_find_key(EnvSession, keyID)){
            return SEFILE_ENV_UPDATE_ERROR;
        }
        *EnvKeyID=keyID;
    }

    if (crypto != (SE3_ALGO_MAX + 1)){
        if ((ret = L1_get_algorithms(EnvSession, 0, SE3_ALGO_MAX, algTable, &count)) == 0 && count > 0 && algTable[crypto].type == SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH){
            *EnvCrypto = crypto;
        }else{
            return SEFILE_ENV_UPDATE_ERROR;
        }
    }

    return 0;
}

uint16_t secure_finit(){
    if(EnvSession!=NULL){
        free(EnvSession);
    }
    if(EnvKeyID!=NULL){
        free(EnvKeyID);
    }

    if(EnvCrypto!=NULL){
        free(EnvCrypto);
    }

    return 0;
}

uint16_t secure_open(char *path, SEFILE_FHANDLE *hFile, int32_t mode, int32_t creation){

    uint16_t commandError=0;
    char enc_filename[MAX_PATHNAME];
    uint16_t lenc=0;
    SEFILE_FHANDLE hTmp=NULL;
    SEFILE_SECTOR buffEnc, buffDec;
#ifdef _WIN32
    DWORD nBytesRead = 0;
#endif

    if(check_env()){
        return SEFILE_OPEN_ERROR;
    }

    memset(enc_filename, 0, MAX_PATHNAME*sizeof(char));

    if(creation==(SEFILE_NEWFILE)){
        if((commandError=secure_create(path, &hTmp, mode))!=0){
            return SEFILE_OPEN_ERROR;
        }
        memcpy(hFile, &hTmp, sizeof(hTmp));
        return 0;
    }

    if(crypto_filename(path, enc_filename, &lenc)){
        return SEFILE_OPEN_ERROR;
    }
    hTmp=(SEFILE_FHANDLE)malloc(sizeof(struct SEFILE_HANDLE));
    if(hTmp==NULL){

        return SEFILE_OPEN_ERROR;
    }
    /* open phase start */
#if defined(__linux__) || defined(__APPLE__)
    if((hTmp->fd = open(enc_filename, mode | creation, S_IRWXU)) == -1 ){
        commandError=SEFILE_OPEN_ERROR;
    }
#elif _WIN32

    hTmp->fd = CreateFile(
                enc_filename,		              				// file to open
                mode,   	     								// open for reading and writing
                FILE_SHARE_READ | FILE_SHARE_WRITE,       		// share
                NULL,                  							// default security
                creation,	      								// existing file only
                FILE_ATTRIBUTE_NORMAL,                          // normal file
                NULL);                 							// no attr. template

    if (hTmp->fd == INVALID_HANDLE_VALUE){
        DWORD errValue = GetLastError();

        commandError=SEFILE_OPEN_ERROR;
    }

#endif
    /* open phase end */

    /* move pointer after the first sector*/
#if defined(__linux__) || defined(__APPLE__)
    /*lseek(hTmp->fd, SEFILE_SECTOR_SIZE, SEEK_SET);*/
    if (read(hTmp->fd, &buffEnc, sizeof(SEFILE_SECTOR)) != sizeof(SEFILE_SECTOR)){
        commandError = SEFILE_OPEN_ERROR;
    }
    hTmp->log_offset = lseek(hTmp->fd, 0, SEEK_CUR);
#elif _WIN32
    /*SetFilePointer(hTmp->fd, SEFILE_SECTOR_SIZE, NULL, FILE_BEGIN);*/
    if (ReadFile(hTmp->fd, &buffEnc, sizeof(SEFILE_SECTOR), &nBytesRead, NULL) == FALSE && nBytesRead != sizeof(SEFILE_SECTOR)){
        commandError = SEFILE_OPEN_ERROR;
    }
    hTmp->log_offset = SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT);
#endif
    if (crypt_header(&buffEnc, &buffDec, SEFILE_SECTOR_DATA_SIZE, SE3_DIR_DECRYPT)){
        commandError = SEFILE_OPEN_ERROR;
    }

    memcpy(hTmp->nonce_ctr, buffDec.header.nonce_ctr, 16);
    memcpy(hTmp->nonce_pbkdf2, buffDec.header.nonce_pbkdf2, SEFILE_NONCE_LEN);

    memcpy(hFile, &hTmp, sizeof(hTmp));
    return commandError;
}

uint16_t secure_create(char *path, SEFILE_FHANDLE *hFile, int mode){
    uint16_t commandError=0;
    char enc_filename[MAX_PATHNAME], *filename=NULL;
    uint16_t lenc=0;
    SEFILE_SECTOR *buff=NULL, *buffEnc=NULL;
    SEFILE_FHANDLE hTmp=NULL;
    size_t random_padding = 0;
    uint8_t *padding_ptr = NULL;
#ifdef _WIN32
    DWORD nBytesWritten = 0;
#endif
    if(check_env()){
        return SEFILE_CREATE_ERROR;
    }
    memset(enc_filename, 0, MAX_PATHNAME*sizeof(char));
    if(crypto_filename(path, enc_filename, ((uint16_t*)&lenc))){
        return SEFILE_CREATE_ERROR;
    }

    hTmp=(SEFILE_FHANDLE)malloc(sizeof(struct SEFILE_HANDLE));
    if(hTmp==NULL){

        return SEFILE_CREATE_ERROR;
    }

    /* create phase start */
#if defined(__linux__) || defined(__APPLE__)
    if((hTmp->fd = open(enc_filename, mode | O_CREAT | O_TRUNC , S_IRWXU)) == -1 ){

        commandError=SEFILE_CREATE_ERROR;
    }
#elif _WIN32

    hTmp->fd = CreateFile(
                enc_filename,		          					// file to open
                mode,     	     								// open for reading and writing
                FILE_SHARE_READ | FILE_SHARE_WRITE,             // share
                NULL,                  							// default security
                CREATE_ALWAYS,	      							// existing file only
                FILE_ATTRIBUTE_NORMAL,                          // normal file
                NULL);                 							// no attr. template

    if (hTmp->fd == INVALID_HANDLE_VALUE){

        commandError=SEFILE_CREATE_ERROR;
    }

#endif
    /* create phase end */

    /* create new header sector start ****************************************/
    buff=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    buffEnc=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    if(buff==NULL || buffEnc==NULL){

        return SEFILE_CREATE_ERROR;
    }
    //TODO Populate header and write to file.
    //buff->header.nonce_ctr.high=0; //discover what to put.
    //buff->header.nonce_ctr.low=0;
    //memset(buff->header.nonce_ctr, 0, 16);
    se3c_rand(16, buff->header.nonce_ctr);
    memcpy(hTmp->nonce_ctr, buff->header.nonce_ctr, 16);
    se3c_rand(SEFILE_NONCE_LEN, buff->header.nonce_pbkdf2);
    memcpy(hTmp->nonce_pbkdf2, buff->header.nonce_pbkdf2, SEFILE_NONCE_LEN);
    buff->header.uid=0;
    buff->header.uid_cnt=0;
    buff->header.ver=0;
    buff->header.magic=0;

    filename=strrchr(path, '/');
    if(filename==NULL){
        filename=strrchr(path, '\\');
        if(filename==NULL) filename=path;
        else filename++;
    }else{
        filename++;
    }
    buff->header.fname_len=strlen(filename);
    buff->len=0;
    //copy filename in header
    memcpy(buff->data+sizeof(SEFILE_HEADER), filename, buff->header.fname_len);
    //TODO add random padding!
    padding_ptr = (buff->data + sizeof(SEFILE_HEADER) + buff->header.fname_len);
    random_padding = (buff->data+SEFILE_LOGIC_DATA) - padding_ptr;
    se3c_rand(random_padding, padding_ptr);


    if (crypt_header(buff, buffEnc, SEFILE_SECTOR_DATA_SIZE, SE3_DIR_ENCRYPT)){
        free(buff);
        free(buffEnc);
        return SEFILE_CREATE_ERROR;
    }



#if defined(__linux__) || defined(__APPLE__)
    if(write(hTmp->fd, buffEnc, sizeof(SEFILE_SECTOR)) != sizeof(SEFILE_SECTOR) ){
        commandError=SEFILE_CREATE_ERROR;
    }
#elif _WIN32
    if (WriteFile(hTmp->fd, buffEnc, sizeof(SEFILE_SECTOR), &nBytesWritten, NULL) == FALSE)
        commandError=SEFILE_CREATE_ERROR;
#endif
    /* create new header sector end **************************************/

    /* move pointer after the first sector */
#if defined(__linux__) || defined(__APPLE__)
    hTmp->log_offset=lseek(hTmp->fd, 0, SEEK_CUR);
#elif _WIN32
    hTmp->log_offset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT);
#endif
    free(buff);
    free(buffEnc);
    memcpy(hFile, &hTmp, sizeof(hTmp));
    return commandError;
}

uint16_t secure_write(SEFILE_FHANDLE *hFile, uint8_t * dataIn, uint32_t dataIn_len){
    SEFILE_FHANDLE hTmp=NULL;
    int32_t absOffset=0, sectOffset=0;
    SEFILE_SECTOR *cryptBuff=NULL, *decryptBuff=NULL;
    int length = 0;
    size_t current_position = SEFILE_SECTOR_SIZE;
    size_t random_padding = 0;
    uint8_t *padding_ptr = NULL;
#if defined(__linux__) || defined(__APPLE__)
    int nBytesRead=0;
#elif _WIN32
    DWORD nBytesWritten=0, nBytesRead=0;
#endif

    if(check_env() || hFile==NULL){
        return SEFILE_WRITE_ERROR;
    }
    hTmp=*hFile;
    if (dataIn_len == 0){
        return 0;
    }
    //    if (secure_sync(hFile)){
    //        return SEFILE_WRITE_ERROR;
    //    }
    //move the pointer to the begin of the sector
#if defined(__linux__) || defined(__APPLE__)
    if((absOffset=lseek(hTmp->fd, 0, SEEK_CUR))<0 || absOffset!=hTmp->log_offset){

        return SEFILE_WRITE_ERROR;
    }
    current_position = lseek(hTmp->fd,((int32_t)(absOffset/SEFILE_SECTOR_SIZE))*SEFILE_SECTOR_SIZE, SEEK_SET);
#elif _WIN32
    if((absOffset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT))<0 || absOffset!=hTmp->log_offset){

        return SEFILE_WRITE_ERROR;
    }
    current_position = SetFilePointer(hTmp->fd,((int32_t)(absOffset/SEFILE_SECTOR_SIZE))*SEFILE_SECTOR_SIZE, NULL, FILE_BEGIN);
#endif

    cryptBuff=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    decryptBuff=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    if(cryptBuff==NULL || decryptBuff==NULL){

        return SEFILE_WRITE_ERROR;
    }
    //save the relative position inside the sector
    sectOffset=absOffset%SEFILE_SECTOR_SIZE;
    //read the whole sector and move back the pointer
#if defined(__linux__) || defined(__APPLE__)
    nBytesRead=read(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE);
#elif _WIN32
    ReadFile(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE, &nBytesRead, NULL);
#endif
    if(nBytesRead>0){

        if (decrypt_sectors(cryptBuff, decryptBuff, SEFILE_SECTOR_DATA_SIZE, POS_TO_CIPHER_BLOCK(current_position), hTmp->nonce_ctr, hTmp->nonce_pbkdf2)){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_WRITE_ERROR;
        }
        //sector integrity check
        if (memcmp(cryptBuff->signature, decryptBuff->signature, B5_SHA256_DIGEST_SIZE)){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_SIGNATURE_MISMATCH;
        }



#if defined(__linux__) || defined(__APPLE__)
        lseek(hTmp->fd, (-1)*SEFILE_SECTOR_SIZE, SEEK_CUR);
#elif _WIN32
        SetFilePointer(hTmp->fd, (-1)*SEFILE_SECTOR_SIZE, NULL, FILE_CURRENT);
#endif

    }else{
        //sector empty
        decryptBuff->len=0;
    }
    do{
        //fill the sector with input data until datain are over or the sector is full
        length = dataIn_len < SEFILE_LOGIC_DATA-sectOffset? dataIn_len : SEFILE_LOGIC_DATA-sectOffset;
        memcpy(decryptBuff->data+sectOffset, dataIn, length);

        //update sector data length if needed
        if( (length + (sectOffset)) > decryptBuff->len){
            decryptBuff->len = length + sectOffset;
        }
        /*Padding must be random! (known plaintext attack)*/
        padding_ptr = decryptBuff->data + decryptBuff->len;
        random_padding = decryptBuff->data + SEFILE_LOGIC_DATA - padding_ptr;
        se3c_rand(random_padding, padding_ptr);


        //encrypt sector
        if (crypt_sectors(decryptBuff, cryptBuff, SEFILE_SECTOR_DATA_SIZE, POS_TO_CIPHER_BLOCK(current_position), hTmp->nonce_ctr, hTmp->nonce_pbkdf2)){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_WRITE_ERROR;
        }


        /* writeback sector into file phase start */
#if defined(__linux__) || defined (__APPLE__)

        if(write(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE) != SEFILE_SECTOR_SIZE){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_WRITE_ERROR;
        }

#elif _WIN32

        if (WriteFile(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE, &nBytesWritten, NULL) == FALSE){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_WRITE_ERROR;
        }

        if(nBytesWritten != (DWORD) SEFILE_SECTOR_SIZE){
            free(cryptBuff);
            free(decryptBuff);
            return SEFILE_WRITE_ERROR;
        }

#endif
        /* writeback sector into file phase end */
        current_position += SEFILE_SECTOR_SIZE;
        dataIn_len-=length;
        dataIn+=length;
        sectOffset=(sectOffset+length)%(SEFILE_LOGIC_DATA);
        decryptBuff->len=0;

    }while(dataIn_len>0); //cycles unless all dataIn are processed

    //move the pointer inside the last sector written
    if(sectOffset!=0){
#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset=lseek(hTmp->fd, (sectOffset - SEFILE_SECTOR_SIZE), SEEK_CUR);
#elif _WIN32
        hTmp->log_offset = SetFilePointer(hTmp->fd, (LONG)(sectOffset - SEFILE_SECTOR_SIZE), NULL, FILE_CURRENT);
#endif
    }else{
#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset=lseek(hTmp->fd, 0, SEEK_CUR);
#elif _WIN32
        hTmp->log_offset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT);
#endif
    }
    free(cryptBuff);
    free(decryptBuff);
    return 0;
}



uint16_t secure_read(SEFILE_FHANDLE *hFile,  uint8_t * dataOut, uint32_t dataOut_len, uint32_t * bytesRead){
    SEFILE_FHANDLE hTmp=NULL;
    int32_t absOffset=0, sectOffset=0;
    uint32_t dataRead=0;
    SEFILE_SECTOR *cryptBuff=NULL, *decryptBuff=NULL;
    int length = 0;
    size_t current_position = SEFILE_SECTOR_SIZE;
    int32_t data_remaining = 0;
#if defined(__linux__) || defined(__APPLE__)
    int nBytesRead=0;
#elif _WIN32
    DWORD nBytesRead=0;
#endif

    if(check_env() || hFile==NULL){
        return SEFILE_READ_ERROR;
    }
    if (dataOut_len == 0){
        if (bytesRead != NULL) *bytesRead = 0;
        return 0;
    }
    hTmp=*hFile;
    //    if (secure_sync(hFile)){
    //        return SEFILE_WRITE_ERROR;
    //    }
    //move the pointer to the begin of the sector
#if defined(__linux__) || defined(__APPLE__)
    if((absOffset=lseek(hTmp->fd, 0, SEEK_CUR))<0 || absOffset!=hTmp->log_offset){

        return SEFILE_READ_ERROR;
    }
    current_position = lseek(hTmp->fd,((int32_t)(absOffset/SEFILE_SECTOR_SIZE))*SEFILE_SECTOR_SIZE, SEEK_SET);
#elif _WIN32
    if((absOffset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT))<0 || absOffset!=hTmp->log_offset){

        return SEFILE_READ_ERROR;
    }
    current_position = SetFilePointer(hTmp->fd, ((int32_t)(absOffset / SEFILE_SECTOR_SIZE))*SEFILE_SECTOR_SIZE, NULL, FILE_BEGIN);
#endif

    cryptBuff=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    decryptBuff=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    if(cryptBuff==NULL || decryptBuff==NULL){
        return SEFILE_READ_ERROR;
    }
    //save the relative position inside the sector
    sectOffset=absOffset%SEFILE_SECTOR_SIZE;

    do{
        //read the whole sector
#if defined(__linux__) || defined(__APPLE__)
        nBytesRead=read(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE);
#elif _WIN32
        ReadFile(hTmp->fd, cryptBuff, SEFILE_SECTOR_SIZE, &nBytesRead, NULL);
#endif
        if(nBytesRead>0){
            
            if (decrypt_sectors(cryptBuff, decryptBuff, SEFILE_SECTOR_DATA_SIZE, POS_TO_CIPHER_BLOCK(current_position), hTmp->nonce_ctr, hTmp->nonce_pbkdf2)){
                free(cryptBuff);
                free(decryptBuff);
                return SEFILE_READ_ERROR;
            }
            //sector integrity check
            if (memcmp(cryptBuff->signature, decryptBuff->signature, B5_SHA256_DIGEST_SIZE)){
                free(cryptBuff);
                free(decryptBuff);
                return SEFILE_SIGNATURE_MISMATCH;
            }

        }else{
            break;
        }

        data_remaining = (decryptBuff->len) - sectOffset; //remaining data in THIS sector
        length = dataOut_len < SEFILE_LOGIC_DATA-sectOffset? dataOut_len : SEFILE_LOGIC_DATA-sectOffset;

        if(data_remaining<length)
            length = data_remaining;

        memcpy(dataOut+dataRead, decryptBuff->data+sectOffset, length);
        current_position += SEFILE_SECTOR_SIZE;
        dataOut_len-=length;
        dataRead+=length;
        sectOffset=(sectOffset+length)%SEFILE_LOGIC_DATA;
    }while(dataOut_len>0); //cycles unless all data requested are read

    //move the pointer inside the last sector read
    if(sectOffset!=0){
#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset=lseek(hTmp->fd, (-1)*(SEFILE_SECTOR_SIZE-sectOffset), SEEK_CUR);//todo check sectoffset
#elif _WIN32
        hTmp->log_offset=SetFilePointer(hTmp->fd,(-1)*(SEFILE_SECTOR_SIZE-sectOffset), NULL, FILE_CURRENT);
#endif
    }else{
#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset=lseek(hTmp->fd, 0, SEEK_CUR);//todo check sectoffset
#elif _WIN32
        hTmp->log_offset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT);
#endif
    }
    *bytesRead=dataRead;
    free(cryptBuff);
    free(decryptBuff);
    return 0;
}

uint16_t secure_seek(SEFILE_FHANDLE *hFile, int32_t offset, int32_t *position, uint8_t whence){
    int32_t dest=0, tmp=0, buffer_size=0;
    int32_t overhead=0, absOffset=0, sectOffset=0;
    uint8_t * buffer=NULL;
    uint32_t file_length=0;
    SEFILE_FHANDLE hTmp=NULL;
    if(check_env() || hFile==NULL){
        return SEFILE_SEEK_ERROR;
    }
    hTmp=*hFile;
    /*	dest contains the ABSOLUTE final position (comprehensive of header and overhead
     * overhead represent the signature and length byte of each sector "jumped"
     * position contains the position as the number of user data byte from the begin of the file
     */
    //    if (secure_sync(hFile)){
    //        return SEFILE_WRITE_ERROR;
    //    }
#if defined(__linux__) || defined(__APPLE__)
    if((absOffset=lseek(hTmp->fd, 0, SEEK_CUR))<0 || absOffset!=hTmp->log_offset){
        return SEFILE_SEEK_ERROR;
    }
#elif _WIN32
    if((absOffset=SetFilePointer(hTmp->fd, 0, NULL, FILE_CURRENT))<0 || absOffset!=hTmp->log_offset){

        return SEFILE_SEEK_ERROR;
    }
#endif
    if(get_filesize(&hTmp, &file_length)){
        return SEFILE_SEEK_ERROR;
    }

    sectOffset=absOffset%SEFILE_SECTOR_SIZE;

    if(whence==SEFILE_BEGIN){
        if(offset<0){			//backward jump not allowed from the file begin
            *position=-1;
            return SEFILE_SEEK_ERROR;
        }else{
            overhead=(offset/SEFILE_LOGIC_DATA)*SEFILE_SECTOR_OVERHEAD;
            dest=offset+overhead+SEFILE_SECTOR_SIZE;
        }
    } else if(whence==SEFILE_CURRENT){
        if(offset<0){			//backward jump
            tmp=(offset + sectOffset);
            if(tmp>=0){			//inside the sector
                dest=absOffset+offset;
            } else{				//outside the current sector (need to add overheads)
                tmp *= (-1);
                overhead=((tmp)/(SEFILE_LOGIC_DATA))* SEFILE_SECTOR_OVERHEAD;
                if((tmp)%SEFILE_LOGIC_DATA) overhead+=SEFILE_SECTOR_OVERHEAD;
                dest=absOffset + offset - overhead;
            }

        }else {					//forward jump
            overhead=((offset+sectOffset)/SEFILE_LOGIC_DATA)*SEFILE_SECTOR_OVERHEAD;
            dest=absOffset+overhead+offset;
        }

    } else if(whence==SEFILE_END){
        sectOffset=(file_length%SEFILE_LOGIC_DATA);
        absOffset=((file_length/SEFILE_LOGIC_DATA)+1)*SEFILE_SECTOR_SIZE+sectOffset;

        if(offset<0){			//backward jump
            tmp=(offset + sectOffset);
            if(tmp>=0){			//inside the sector
                dest=absOffset+offset;
            } else{		//outside the current sector (need to add overheads)
                tmp *= (-1);
                overhead=((tmp)/(SEFILE_LOGIC_DATA))* SEFILE_SECTOR_OVERHEAD;
                if((tmp)%SEFILE_LOGIC_DATA) overhead+=SEFILE_SECTOR_OVERHEAD;
                dest=absOffset + offset - overhead;
            }
        } else {				//forward jump
            overhead=((offset+sectOffset)/SEFILE_LOGIC_DATA)*SEFILE_SECTOR_OVERHEAD;
            dest=absOffset+offset+overhead;
        }
    }

    if(dest<SEFILE_SECTOR_SIZE){	//pointer inside the header sector is not allowed
        *position=-1;
        return SEFILE_ILLEGAL_SEEK;
    }

    *position=(dest%SEFILE_SECTOR_SIZE)+(((dest/SEFILE_SECTOR_SIZE)-1)*SEFILE_LOGIC_DATA);
    buffer_size=*position-file_length;

    if(buffer_size>0){ 			//if destination exceed the end of the file, empty sectors are inserted at the end of the file to keep the file consistency
        buffer=calloc(buffer_size, sizeof(uint8_t));
        if(buffer==NULL){
            return SEFILE_SEEK_ERROR;
        }
        if((file_length%SEFILE_LOGIC_DATA)){
#if defined(__linux__) || defined(__APPLE__)
            hTmp->log_offset=lseek(hTmp->fd, ((file_length%SEFILE_LOGIC_DATA)-SEFILE_SECTOR_SIZE), SEEK_END);
#elif _WIN32
            hTmp->log_offset=SetFilePointer(hTmp->fd, ((file_length%SEFILE_LOGIC_DATA)-SEFILE_SECTOR_SIZE), NULL, FILE_END);
#endif
        }
        if(secure_write(&hTmp, buffer, buffer_size)){
            free(buffer);
            return SEFILE_SEEK_ERROR;
        }
        free(buffer);
    } else {

#if defined(__linux__) || defined(__APPLE__)

        tmp=lseek(hTmp->fd, (off_t) dest, SEEK_SET);
        if(( tmp == -1)){

            return SEFILE_SEEK_ERROR;
        }

#elif _WIN32

        tmp = SetFilePointer( hTmp->fd, dest, NULL, FILE_BEGIN);

        if ( tmp == INVALID_SET_FILE_POINTER){

            return SEFILE_SEEK_ERROR;
        }
#endif

        hTmp->log_offset=tmp;
    }

    return 0;
}

uint16_t secure_truncate(SEFILE_FHANDLE *hFile, uint32_t size){

    SEFILE_FHANDLE hTmp = *hFile;
    uint32_t fPosition = 0;
    int rOffset = 0, nSector = 0; //New Relative offset & new number of sectors
    uint8_t *buffer = NULL;
    uint32_t aSize = 0, bytesRead = 0;

    //    if (secure_sync(hFile)){
    //        return SEFILE_WRITE_ERROR;
    //    }

    if(get_filesize(hFile, &aSize)){
        return SEFILE_TRUNCATE_ERROR;
    }

    if(aSize < size){ //File should be enlarged
        if (secure_seek(hFile, (size-aSize), (int32_t*)&fPosition, SEFILE_END) || fPosition != size+aSize){
            return SEFILE_TRUNCATE_ERROR;
        }
    }else{

        rOffset = size % SEFILE_LOGIC_DATA; //Relative offset inside a sector
        nSector = (size / SEFILE_LOGIC_DATA) + 1; //Number of sectors in a file (including header)

#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset = lseek(hTmp->fd, nSector*SEFILE_SECTOR_SIZE, SEEK_SET);
        if(hTmp->log_offset < 0){

            return SEFILE_TRUNCATE_ERROR;
        }
#elif _WIN32
        hTmp->log_offset = SetFilePointer(hTmp->fd, nSector*SEFILE_SECTOR_SIZE, NULL, FILE_BEGIN);
        if(hTmp->log_offset == INVALID_SET_FILE_POINTER){

            return SEFILE_TRUNCATE_ERROR;
        }
#endif

        buffer=(uint8_t *)malloc(rOffset*(sizeof(uint8_t)));
        if(buffer == NULL){

            return SEFILE_TRUNCATE_ERROR;
        }
        if(secure_read(&hTmp, buffer, rOffset, &bytesRead)){
            return SEFILE_TRUNCATE_ERROR;
        }
#if defined(__linux__) || defined(__APPLE__)
        hTmp->log_offset = lseek(hTmp->fd, nSector*SEFILE_SECTOR_SIZE, SEEK_SET);
        if(hTmp->log_offset < 0){

            return SEFILE_TRUNCATE_ERROR;
        }
        if(ftruncate(hTmp->fd, nSector*SEFILE_SECTOR_SIZE)){	//truncate

            return SEFILE_TRUNCATE_ERROR;
        }
#elif _WIN32
        hTmp->log_offset = SetFilePointer(hTmp->fd, nSector*SEFILE_SECTOR_SIZE, NULL, FILE_BEGIN);
        if(hTmp->log_offset == INVALID_SET_FILE_POINTER){

            return SEFILE_TRUNCATE_ERROR;
        }
        if(!SetEndOfFile(hTmp->fd)){	//truncate

            return SEFILE_TRUNCATE_ERROR;
        }
#endif

        if(secure_write(&hTmp, buffer, rOffset)){
            return SEFILE_TRUNCATE_ERROR;
        }
        free(buffer);
    }
    return 0;
}

uint16_t secure_close(SEFILE_FHANDLE *hFile){
    SEFILE_FHANDLE hTmp=*hFile;
    if(check_env()){
        return SEFILE_CLOSE_HANDLE_ERR;
    }
    //    if (secure_sync(hFile)){
    //        return SEFILE_WRITE_ERROR;
    //    }
#if defined(__linux__) || defined(__APPLE__)
    if(hFile!=NULL){
        if(close(hTmp->fd) == -1 ){

            free(hTmp);
            return SEFILE_CLOSE_HANDLE_ERR;
        }
        free(hTmp);
    }else{

        return SEFILE_CLOSE_HANDLE_ERR;
    }
#elif _WIN32
    if(hFile!=NULL){
        if ( CloseHandle(hTmp->fd) == 0){

            free(hTmp);
            return SEFILE_CLOSE_HANDLE_ERR;
        }
        free(hTmp);
    }else{

        return SEFILE_CLOSE_HANDLE_ERR;
    }
#endif

    return 0;
}

uint16_t secure_ls(char *path, char *list, uint32_t * list_length){
    uint16_t commandError=0;
    char bufferEnc[MAX_PATHNAME], bufferDec[MAX_PATHNAME], *pFile=NULL;
    uint16_t encoded_length=0;
    char cwd[MAX_PATHNAME];
#ifdef _WIN32
    HANDLE hDir;
    WIN32_FIND_DATA dDir;
#endif
    memset(bufferEnc, 0, MAX_PATHNAME*sizeof(char));
    memset(bufferDec, 0, MAX_PATHNAME*sizeof(char));
    memset(cwd, 0, MAX_PATHNAME*sizeof(char));
    if(check_env() || list == NULL){
        return SEFILE_LS_ERROR;
    }
    *list_length=0;
#if defined(__linux__) || defined(__APPLE__)
    DIR *hDir=NULL;
    struct dirent *dDir;
    getcwd(cwd, MAX_PATHNAME);
    chdir(path);
    pFile=list;
    hDir=opendir(".");
    while((dDir=readdir(hDir))!=NULL){
        if((!strcmp(dDir->d_name,"."))||(!strcmp(dDir->d_name,".."))){
            continue;
        }else if(dDir->d_type==DT_DIR){
            if(valid_name(dDir->d_name)) continue;
            commandError=decrypt_dirname(dDir->d_name, bufferDec);
            if(commandError==SEFILE_USER_NOT_ALLOWED){
                continue;
            }else if(commandError==0){
                strcpy(pFile, bufferDec);
                pFile+=(1+strlen(bufferDec));
                *list_length+=(1+strlen(bufferDec));
                memset(bufferEnc, 0, MAX_PATHNAME*sizeof(char));
                memset(bufferDec, 0, MAX_PATHNAME*sizeof(char));
            }else {
                return SEFILE_LS_ERROR;
            }
        }else if(dDir->d_type==DT_REG){
            if(valid_name(dDir->d_name) || decrypt_filename(dDir->d_name, bufferDec)){
                continue;
            }
            if(crypto_filename(bufferDec, bufferEnc, &encoded_length)){
                return SEFILE_LS_ERROR;
            }
            if(!strncmp(bufferEnc, dDir->d_name,encoded_length)){//user allowed
                strcpy(pFile, bufferDec);
                pFile+=(1+strlen(bufferDec));
                *list_length+=(1+strlen(bufferDec));
                memset(bufferEnc, 0, MAX_PATHNAME*sizeof(char));
                memset(bufferDec, 0, MAX_PATHNAME*sizeof(char));
            }
        }

    }
    closedir(hDir);
    chdir(cwd);
#elif _WIN32

    GetCurrentDirectory(MAX_PATHNAME, cwd);
    SetCurrentDirectory(path);

    pFile=list;

    hDir=FindFirstFile("*", &dDir);
    do{
        if((!strcmp(dDir.cFileName,"."))||(!strcmp(dDir.cFileName,".."))){
            continue;
        }else if(dDir.dwFileAttributes==FILE_ATTRIBUTE_DIRECTORY){
            if(valid_name(dDir.cFileName)) continue;
            commandError=decrypt_dirname(dDir.cFileName, bufferDec);
            if(commandError==SEFILE_USER_NOT_ALLOWED){
                continue;
            }else if(commandError==0){
                strcpy(pFile, bufferDec);
                pFile+=(1+strlen(bufferDec));
                *list_length+=(1+strlen(bufferDec));
            }else {
                return SEFILE_LS_ERROR;
            }
        }else{
            if(valid_name(dDir.cFileName) || decrypt_filename(dDir.cFileName, bufferDec)){
                continue;
            }
            if(crypto_filename(bufferDec, bufferEnc, &encoded_length)){
                return SEFILE_LS_ERROR;
            }
            if(!strncmp(bufferEnc, dDir.cFileName,encoded_length)){//user allowed
                strcpy(pFile, bufferDec);
                pFile+=(1+strlen(bufferDec));
                *list_length+=(1+strlen(bufferDec));
            }
        }

    }while(FindNextFile(hDir, &dDir));
    FindClose(hDir);
    SetCurrentDirectory(cwd);

#endif


    return 0;
}

uint16_t secure_getfilesize(char *path, uint32_t * position){
    uint16_t ret = SE3_OK;
    SEFILE_FHANDLE hFile=NULL;

    if(check_env()){
        return SEFILE_FILESIZE_ERROR;
    }

    if (secure_open(path, &hFile, SEFILE_READ, SEFILE_OPEN )){
        return SEFILE_FILESIZE_ERROR;
    }

    ret = get_filesize(&hFile, position);

    if(secure_close(&hFile)){
        return SEFILE_FILESIZE_ERROR;
    }
    return ret;
}

uint16_t secure_mkdir(char *path){
    char encDirname[MAX_PATHNAME];
#ifdef _WIN32
    DWORD errValue = 0;
#else
    int errValue = 0;
#endif
    memset(encDirname, 0, MAX_PATHNAME*sizeof(char));
    if(check_env()){
        return SEFILE_MKDIR_ERROR;
    }

    if(crypt_dirname(path, encDirname)){
        return SEFILE_MKDIR_ERROR;
    }
#if defined(__linux__)||defined(__APPLE__)
    if(mkdir(encDirname, S_IRWXU | S_IRWXG)){
        if ((errValue = errno) == EEXIST){
            return 0;
        }
        return SEFILE_MKDIR_ERROR;
    }
#elif _WIN32
    if(!CreateDirectory(encDirname, NULL)){
        if ((errValue = GetLastError()) == ERROR_ALREADY_EXISTS){
            return 0;
        }
        return SEFILE_MKDIR_ERROR;
    }
#endif
    return 0;
}

void get_filename(char *path, char *file_name){

    char *f_name;

    f_name=strrchr(path, '/');
    if(f_name==NULL){
        f_name=strrchr(path, '\\');
        if(f_name==NULL){
            strcpy(file_name, path);
        }
        else{
            strcpy(file_name, f_name+1);
        }
    }else{
        strcpy(file_name, f_name+1);
    }
}
void get_path(char *full_path, char *path){

    char *p_name;

    p_name=strrchr(full_path, '/');
    if(p_name==NULL){
        p_name=strrchr(full_path, '\\');
        if(p_name==NULL) {
            path[0]='\0';
        }
        else{
            memcpy(path, full_path, p_name-full_path+1);
            path[p_name-full_path+1]='\0';
        }
    }else{
        memcpy(path, full_path, p_name-full_path+1);
        path[p_name-full_path+1]='\0';
    }
}
uint16_t crypto_filename(char *path, char *enc_name, uint16_t *encoded_length){
    uint16_t commandError=0;
    uint8_t orig_filename[MAX_PATHNAME];
    uint8_t bufferName[MAX_PATHNAME], SHAName[MAX_PATHNAME];
    uint8_t finalPath[MAX_PATHNAME];
    uint16_t lorig=0, i=0;
    B5_tSha256Ctx ctx;
    int finalLen = 0;
    memset(orig_filename, 0, MAX_PATHNAME*sizeof(char));
    memset(bufferName, 0, MAX_PATHNAME*sizeof(char));
    memset(SHAName, 0, MAX_PATHNAME*sizeof(char));
    memset(finalPath, 0, MAX_PATHNAME*sizeof(char));
    get_filename(path, orig_filename);

    get_path(path, finalPath);

    lorig=strlen(orig_filename);

    if((commandError = B5_Sha256_Init(&ctx))){

        return commandError;
    }
    if((commandError = B5_Sha256_Update(&ctx, (uint8_t *)orig_filename, lorig*sizeof(uint8_t)))){

        return commandError;
    }
    if((commandError = B5_Sha256_Finit(&ctx, (uint8_t *)bufferName))){

        return commandError;
    }
    if (strlen(finalPath)>MAX_PATHNAME-B5_SHA256_DIGEST_SIZE){

        return SEFILE_FILENAME_ENC_ERROR;
    }
    for(i=0; i<B5_SHA256_DIGEST_SIZE; i++){
        sprintf(&(SHAName[i*2]), "%02x", (uint8_t)bufferName[i]);
    }
    finalLen=strlen(finalPath);

    if(encoded_length != NULL)
        *encoded_length=i*2+finalLen;

    if(finalPath[0])
        memcpy(enc_name, finalPath, finalLen);
    memcpy(enc_name+finalLen, SHAName, i*2);
    return 0;
}

uint16_t check_env(){
    if(EnvSession == NULL || EnvKeyID == NULL || EnvCrypto == NULL ){
        return SEFILE_ENV_NOT_SET;
    }
    return 0;
}

uint16_t crypt_header(void *buff1, void *buff2, size_t datain_len, uint16_t direction){
    enum {
        MAX_DATA_IN = SE3_CRYPTO_MAX_DATAIN - (SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA + SEFILE_BLOCK_SIZE) - SEFILE_NONCE_LEN
    };
    uint8_t* sp = (uint8_t*)buff1, *rp = (uint8_t*)buff2;
    uint16_t error = SE3_OK;
    uint16_t curr_len = 0;
    uint32_t enc_sess_id = 0;
    size_t curr_chunk = 0;
    uint16_t flag_reset_auth = datain_len < MAX_DATA_IN ? SE3_CRYPTO_FLAG_AUTH : SE3_CRYPTO_FLAG_AUTH;
    uint8_t *nonce_pbkdf2 = buff1;
    if (datain_len < 0 || buff2 == NULL)
        return(SE3_ERR_PARAMS);

    datain_len -= SEFILE_NONCE_LEN;
    curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;


    error = L1_crypto_init(EnvSession, *EnvCrypto, SE3_FEEDBACK_ECB | direction, *EnvKeyID, &enc_sess_id);
    if (error != SE3_OK) {
        return error;
    }
    error = L1_crypto_update(EnvSession, enc_sess_id, SE3_CRYPTO_FLAG_SETNONCE, SEFILE_NONCE_LEN, nonce_pbkdf2, 0, NULL, NULL, NULL);
    if (error != SE3_OK) {
        return error;
    }
    do {
        if (datain_len - curr_chunk)
            error = L1_crypto_update(EnvSession, enc_sess_id, SE3_FEEDBACK_ECB | direction, 0, NULL, curr_chunk, sp + SEFILE_NONCE_LEN, &curr_len, rp + SEFILE_NONCE_LEN);
        else
            error = L1_crypto_update(EnvSession, enc_sess_id, flag_reset_auth | SE3_CRYPTO_FLAG_FINIT, 0, NULL, curr_chunk, sp + SEFILE_NONCE_LEN, &curr_len, rp + SEFILE_NONCE_LEN);

        if(error) break;
        datain_len -= curr_chunk;
        sp += curr_chunk;
        rp += curr_chunk;
        curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;
    } while (datain_len > 0);

    memcpy(buff2, buff1, SEFILE_NONCE_LEN);

    return(error);
}
void compute_blk_offset(size_t current_offset, uint8_t* nonce){
    uint8_t i = 15, old_v;
    uint16_t cb;
    do{
        old_v = nonce[i];
        nonce[i] += (uint8_t)(current_offset & 0xFF);
        current_offset = (current_offset>>8);
        cb = nonce[i] < old_v;
        if (cb) nonce[i - 1]++;
    } while (i-- && current_offset > 0);
}
uint16_t crypt_sectors(void *buff_decrypt, void *buff_crypt, size_t datain_len, size_t current_offset, uint8_t* nonce_ctr, uint8_t* nonce_pbkdf2){

    enum {
        MAX_DATA_IN = SE3_CRYPTO_MAX_DATAIN - (SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA + SEFILE_BLOCK_SIZE) - B5_SHA256_DIGEST_SIZE
    };
    uint8_t* sp = buff_decrypt, *rp = buff_crypt;
    uint16_t error = SE3_OK;
    uint16_t curr_len = 0;
    uint32_t enc_sess_id = 0;
    size_t curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;
    uint8_t nonce_local[16];
    uint16_t flag_reset_auth = datain_len < MAX_DATA_IN ? SE3_CRYPTO_FLAG_RESET | SE3_CRYPTO_FLAG_AUTH : SE3_CRYPTO_FLAG_AUTH;

    if (datain_len < 0 || buff_crypt == NULL)
        return(SE3_ERR_PARAMS);

    error = L1_crypto_init(EnvSession, *EnvCrypto, SE3_FEEDBACK_CTR | SE3_DIR_ENCRYPT, *EnvKeyID, &enc_sess_id);
    if (error != SE3_OK) {
        return error;
    }
    error = L1_crypto_update(EnvSession, enc_sess_id, SE3_CRYPTO_FLAG_SETNONCE, SEFILE_NONCE_LEN, nonce_pbkdf2, 0, NULL, NULL, NULL);
    if (error != SE3_OK) {
        return error;
    }
    memcpy(nonce_local, nonce_ctr, 16);
    compute_blk_offset(current_offset, nonce_local);
    do {

        if (datain_len - curr_chunk)
            error = L1_crypto_update(EnvSession, enc_sess_id, SE3_CRYPTO_FLAG_RESET | SE3_FEEDBACK_CTR | SE3_DIR_ENCRYPT, SEFILE_BLOCK_SIZE, nonce_local, curr_chunk, sp, &curr_len, rp);
        else
            error = L1_crypto_update(EnvSession, enc_sess_id, flag_reset_auth | SE3_CRYPTO_FLAG_FINIT, SEFILE_BLOCK_SIZE, nonce_local, curr_chunk, sp, &curr_len, rp);

        if(error) break;
        compute_blk_offset(((curr_len - B5_SHA256_DIGEST_SIZE) / SEFILE_BLOCK_SIZE), nonce_local);
        datain_len -= curr_chunk;
        sp += curr_chunk;
        rp += curr_chunk;
        curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;
    } while (datain_len > 0);


    return(error);
}
uint16_t decrypt_sectors(void *buff_crypt, void *buff_decrypt, size_t datain_len, size_t current_offset, uint8_t* nonce_ctr, uint8_t* nonce_pbkdf2){

    enum {
        MAX_DATA_IN = SE3_CRYPTO_MAX_DATAIN - (SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA + SEFILE_BLOCK_SIZE) - B5_SHA256_DIGEST_SIZE
    };
    uint8_t* sp = buff_crypt, *rp = buff_decrypt;
    uint16_t error = SE3_OK;
    uint16_t curr_len = 0;
    uint32_t enc_sess_id = 0;
    size_t curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;
    uint8_t nonce_local[16];
    uint16_t flag_reset_auth = datain_len < MAX_DATA_IN ? SE3_CRYPTO_FLAG_RESET | SE3_CRYPTO_FLAG_AUTH : SE3_CRYPTO_FLAG_AUTH;

    if (datain_len < 0 || buff_crypt == NULL)
        return(SE3_ERR_PARAMS);


    error = L1_crypto_init(EnvSession, *EnvCrypto, SE3_FEEDBACK_CTR | SE3_DIR_DECRYPT, *EnvKeyID, &enc_sess_id);
    if (error != SE3_OK) {
        return error;
    }
    error = L1_crypto_update(EnvSession, enc_sess_id, SE3_CRYPTO_FLAG_SETNONCE, SEFILE_NONCE_LEN, nonce_pbkdf2, 0, NULL, NULL, NULL);
    if (error != SE3_OK) {
        return error;
    }
    memcpy(nonce_local, nonce_ctr, 16);
    compute_blk_offset(current_offset, nonce_local);
    do {

        if (datain_len - curr_chunk)
            error = L1_crypto_update(EnvSession, enc_sess_id, SE3_CRYPTO_FLAG_RESET | SE3_FEEDBACK_CTR | SE3_DIR_DECRYPT, SEFILE_BLOCK_SIZE, nonce_local, curr_chunk, sp, &curr_len, rp);
        else
            error = L1_crypto_update(EnvSession, enc_sess_id, flag_reset_auth | SE3_CRYPTO_FLAG_FINIT, SEFILE_BLOCK_SIZE, nonce_local, curr_chunk, sp, &curr_len, rp);

        if(error) break;
        compute_blk_offset(((curr_len-B5_SHA256_DIGEST_SIZE) / SEFILE_BLOCK_SIZE), nonce_local);
        datain_len -= curr_chunk;
        sp += curr_chunk;
        rp += curr_chunk;
        curr_chunk = datain_len < MAX_DATA_IN ? datain_len : MAX_DATA_IN;
    } while (datain_len > 0);


    return(error);
}

uint16_t get_filesize(SEFILE_FHANDLE *hFile, uint32_t * length){
    SEFILE_SECTOR *crypt_buffer=NULL, *decrypt_buffer=NULL;
    int32_t total_size=0;
    SEFILE_FHANDLE hTmp=NULL;
#if defined(__linux__) || defined(__APPLE__)
    off_t orig_off;
    size_t BytesRead = 0;
#elif _WIN32
    DWORD orig_off;
    DWORD BytesRead = 0;
#endif
    if(hFile==NULL){

        return SEFILE_FILESIZE_ERROR;
    }
    crypt_buffer=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    decrypt_buffer=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    if(crypt_buffer==NULL || decrypt_buffer==NULL){

        return SEFILE_FILESIZE_ERROR;
    }
    hTmp=*hFile;

#if defined(__linux__) || defined(__APPLE__)
    orig_off=lseek(hTmp->fd, 0, SEEK_CUR);
    total_size=lseek(hTmp->fd, (-1)*(SEFILE_SECTOR_SIZE), SEEK_END);

    if(orig_off==-1 || total_size==-1){
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_SEEK_ERROR;
    }
    if(!total_size) {
        lseek(hTmp->fd, orig_off, SEEK_SET);
        *length=0;
        free(crypt_buffer);
        free(decrypt_buffer);
        return 0;
    }
    if((BytesRead = read(hTmp->fd, crypt_buffer, SEFILE_SECTOR_SIZE))!= SEFILE_SECTOR_SIZE){
        lseek(hTmp->fd, orig_off, SEEK_SET);
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_READ_ERROR;
    }

    if((lseek(hTmp->fd, orig_off, SEEK_SET))==-1){
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_SEEK_ERROR;
    }
#elif _WIN32
    orig_off=SetFilePointer(hTmp->fd, 0, 0, FILE_CURRENT);
    total_size=SetFilePointer(hTmp->fd, (-1)*(SEFILE_SECTOR_SIZE), NULL, FILE_END);

    if(orig_off==INVALID_SET_FILE_POINTER || total_size==INVALID_SET_FILE_POINTER){
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_SEEK_ERROR;
    }
    if(!total_size) {
        SetFilePointer(hTmp->fd, orig_off, NULL, FILE_BEGIN);
        *length=0;
        free(crypt_buffer);
        free(decrypt_buffer);
        return 0;
    }
    if ((ReadFile(hTmp->fd, crypt_buffer, SEFILE_SECTOR_SIZE, &BytesRead, NULL))==0 || BytesRead!=SEFILE_SECTOR_SIZE){
        SetFilePointer(hTmp->fd, orig_off, NULL, FILE_BEGIN);
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_READ_ERROR;
    }

    if((SetFilePointer(hTmp->fd, orig_off, NULL, FILE_BEGIN))==INVALID_SET_FILE_POINTER){
        free(crypt_buffer);
        free(decrypt_buffer);

        return SEFILE_SEEK_ERROR;
    }

#endif

    if (decrypt_sectors(crypt_buffer, decrypt_buffer, SEFILE_SECTOR_DATA_SIZE, POS_TO_CIPHER_BLOCK(total_size), hTmp->nonce_ctr, hTmp->nonce_pbkdf2)){
        free(crypt_buffer);
        free(decrypt_buffer);
        return SEFILE_FILESIZE_ERROR;
    }
    if (memcmp(crypt_buffer->signature, decrypt_buffer->signature, B5_SHA256_DIGEST_SIZE)){
        free(crypt_buffer);
        free(decrypt_buffer);
        return SEFILE_SIGNATURE_MISMATCH;
    }

    *length=((total_size/SEFILE_SECTOR_SIZE)-1)*SEFILE_LOGIC_DATA + decrypt_buffer->len;
    free(crypt_buffer);
    free(decrypt_buffer);
    return 0;

}
uint16_t decrypt_filename(char *path, char *filename){
    SEFILE_FHANDLE hFile=NULL;

    hFile=(SEFILE_FHANDLE)malloc(sizeof(struct SEFILE_HANDLE));
    if(hFile==NULL){

        return SEFILE_FILENAME_DEC_ERROR;
    }
#if defined(__linux__) || defined(__APPLE__)
    if((hFile->fd = open(path, O_RDONLY)) == -1 ){

        return SEFILE_FILENAME_DEC_ERROR;
    }
#elif _WIN32
    hFile->fd = CreateFile(
                path,			              				// file to open
                GENERIC_READ,     								// open for reading and writing
                FILE_SHARE_READ,       							// share
                NULL,                  							// default security
                OPEN_EXISTING,									// existing file only
                FILE_ATTRIBUTE_NORMAL,							// normal file
                NULL);                 							// no attr. template

    if (hFile->fd == INVALID_HANDLE_VALUE){

        return SEFILE_FILENAME_DEC_ERROR;
    }
#endif
    if(decrypt_filehandle(&hFile, filename)){
        return SEFILE_FILENAME_DEC_ERROR;
    }

    if(secure_close(&hFile)){
        return SEFILE_FILENAME_DEC_ERROR;
    }

    return 0;
}

uint16_t decrypt_filehandle(SEFILE_FHANDLE *hFile, char *filename){
    SEFILE_SECTOR *header_buffer=NULL, *bufferDec=NULL;
    uint8_t length=0;
    int orig_off=0;
    SEFILE_FHANDLE hTmp=NULL;
#ifdef _WIN32
    DWORD BytesRead;
#endif
    if(hFile==NULL){

        return SEFILE_CREATE_ERROR;
    }
    hTmp=*hFile;
    header_buffer=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    bufferDec=(SEFILE_SECTOR *)calloc(1, sizeof(SEFILE_SECTOR));
    if(header_buffer==NULL || bufferDec==NULL){

        return SEFILE_FILESIZE_ERROR;
    }


#if defined(__linux__) || defined(__APPLE__)
    orig_off=lseek(hTmp->fd, 0, SEEK_CUR);

    if(orig_off==-1){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }

    if (lseek(hTmp->fd, 0, SEEK_SET)==-1){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }

    if(read(hTmp->fd, header_buffer, SEFILE_SECTOR_SIZE)!=SEFILE_SECTOR_SIZE){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }
    if (lseek(hTmp->fd, orig_off, SEEK_SET)==-1){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }
#elif _WIN32
    orig_off=SetFilePointer(hTmp->fd, 0, 0, FILE_CURRENT);

    if(orig_off==INVALID_SET_FILE_POINTER){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }

    if(SetFilePointer(hTmp->fd, 0, NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }
    if(ReadFile(hTmp->fd, header_buffer, SEFILE_SECTOR_SIZE, &BytesRead, NULL)==0 || BytesRead!=SEFILE_SECTOR_SIZE){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }
    if(SetFilePointer(hTmp->fd, orig_off, NULL, FILE_BEGIN)==INVALID_SET_FILE_POINTER){
        free(header_buffer);
        free(bufferDec);

        return SEFILE_FILENAME_DEC_ERROR;
    }
#endif

    if (crypt_header(header_buffer, bufferDec, SEFILE_SECTOR_DATA_SIZE, SE3_DIR_DECRYPT)){
        free(header_buffer);
        free(bufferDec);
        return SEFILE_FILENAME_DEC_ERROR;
    }

    if (memcmp(header_buffer->signature, bufferDec->signature, B5_SHA256_DIGEST_SIZE)){
        free(header_buffer);
        free(bufferDec);
        return SEFILE_SIGNATURE_MISMATCH;
    }


    length=bufferDec->header.fname_len;
    memcpy(filename, bufferDec->data+sizeof(SEFILE_HEADER), length);
    free(header_buffer);
    free(bufferDec);
    filename[length]='\0';

    return 0;
}

uint16_t encrypt_name(void* buff1, void* buff2, size_t size, uint16_t direction){
    return L1_encrypt(EnvSession, *EnvCrypto, SE3_FEEDBACK_ECB | direction, *EnvKeyID, size, buff1, NULL, buff2);
}

uint16_t crypt_dirname(char *dirpath, char *encDirname){
    uint16_t commandError=0;
    uint8_t *pDir=NULL;
    int32_t maxLen = 0;
    char *filename=NULL;
    uint8_t *buffDec=NULL, *buffEnc=NULL;
    int i=0;

    filename=strrchr(dirpath, '/');
    if(filename==NULL){
        filename=strrchr(dirpath, '\\');
        if(filename==NULL) filename=dirpath;
        else filename++;
    }else{
        filename++;
    }
    maxLen=(((strlen(filename)/SEFILE_BLOCK_SIZE)+1)*SEFILE_BLOCK_SIZE);
    if(strlen(filename)>(MAX_PATHNAME-2)/2){

        return SEFILE_DIRNAME_ENC_ERROR;
    }
    if((filename-dirpath)>MAX_PATHNAME-maxLen){
        return SEFILE_DIRNAME_ENC_ERROR;
    }
    buffDec=calloc(maxLen+1, sizeof(uint8_t));
    buffEnc=calloc(maxLen+1, sizeof(uint8_t));
    if(buffDec==NULL || buffEnc==NULL){

        return SEFILE_DIRNAME_ENC_ERROR;
    }
    memcpy(buffDec, filename, strlen(filename));

    if((commandError = encrypt_name(buffDec, buffEnc, maxLen, SE3_DIR_ENCRYPT))){
        free(buffDec);
        free(buffEnc);
        return commandError;
    }
    memcpy(encDirname, dirpath, filename-dirpath);
    pDir=encDirname+(filename-dirpath);
    sprintf(pDir, "%02x", (uint8_t)*EnvKeyID);
    pDir+=2;
    for(i=0;i<maxLen;i++){
        sprintf(pDir+(i*2), "%02x", (uint8_t)buffEnc[i]);
    }
    pDir[i*2]='\0';
    free(buffDec);
    free(buffEnc);
    return 0;
}

uint16_t decrypt_dirname(char *dirpath, char *decDirname){
    uint16_t commandError=0;
    int32_t maxLen = 0;
    int32_t key=0;
    char *filename=NULL, *buffEnc=NULL;
    char tmp[3]={0,0,0};
    uint32_t i=0;

    filename=strrchr(dirpath, '/');
    if(filename==NULL){
        filename=strrchr(dirpath, '\\');
        if(filename==NULL) filename=dirpath;
        else filename++;
    }else{
        filename++;
    }

    maxLen=(strlen(filename)-2);
    buffEnc=calloc((maxLen/2)+1+1, sizeof(char));
    if(buffEnc==NULL){

        return SEFILE_DIRNAME_ENC_ERROR;
    }
    //Conversion of 2 chars at a time from Hex to Integer

    for(i=0;i<(maxLen/2)+1;i++){
        memcpy(tmp,&filename[i*2],2);
        buffEnc[i]=strtol(tmp, NULL, 16);
    }
    key=buffEnc[0];

    if(*EnvKeyID==key){
        if((commandError = encrypt_name((buffEnc+1), decDirname, maxLen, SE3_DIR_DECRYPT))){
            free(buffEnc);
            return commandError;
        }
    }else{
        return SEFILE_USER_NOT_ALLOWED;
    }
    free(buffEnc);
    return 0;
}

uint16_t valid_name(char *name){
    int i=0;
    for(i=0;name[i]!='\0';i++){
        if(!isalnum(name[i])) return SEFILE_NAME_NOT_VALID;
        if(name[i] >= '0' && name[i] <= '9') continue;
        if(name[i] >= 'a' && name[i] <= 'f') continue;
        return SEFILE_NAME_NOT_VALID;
    }
    return 0;
}

uint16_t secure_sync(SEFILE_FHANDLE *hFile){
    SEFILE_FHANDLE hTmp;
    uint16_t ret = SE3_OK;

    if(check_env() || hFile == NULL){
        return SEFILE_SYNC_ERR;
    }
    hTmp = *hFile;
#if defined(__linux__) || defined(__APPLE__)
    if(fsync(hTmp->fd)){
        ret = SEFILE_SYNC_ERR;
    }
#elif _WIN32
    if (!FlushFileBuffers(hTmp->fd)){
        ret = SEFILE_SYNC_ERR;
    }
#endif
    return ret;
}
