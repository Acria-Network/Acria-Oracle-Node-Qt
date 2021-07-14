#include "signtransaction.h"
#include "util.h"

#include <QDebug>


char* SignTransaction::uintToCharArr(uint8_t src[], int len){
    char* dest = (char*) calloc(len,1);
    for(int i=0;i<len;i++){
        dest[i] = src[i];
    }
    return dest;
}

void SignTransaction::hexstrToByteArray(const char hexstr[], size_t hexstr_size, uint8_t **pp_arr_out) {
    const char *pos = hexstr;
    size_t arr_size = hexstr_size/2;
    uint8_t *p_arr = (uint8_t *) malloc(arr_size);
    memset(p_arr,0,arr_size);

    for(size_t i = 0; i<arr_size;++i) {
        sscanf(pos, "%2hhx", &p_arr[i]);
        pos+=2;
    }
    *pp_arr_out = p_arr;
}

void SignTransaction::reverse(uint8_t *p_arr, size_t len) {
    for(size_t i = 0; i<len/2; ++i) {
        uint8_t tmp = p_arr[i];
        p_arr[i] = p_arr[len-1-i];
        p_arr[len-1-i] = tmp;
    }
}

std::string SignTransaction::fixHexValue(std::string hex) {
    if (hex.size() % 2 != 0) {
        std::string tmp = RLP::removeHexFormatting(hex);
        tmp = "0x0" + tmp;
        hex = tmp;
    }
    return hex;
}

std::string SignTransaction::sign_transaction(Transaction tx, std::string privkey) {
    uint8_t *privkeyBytes;
    hexstrToByteArray(privkey.c_str(), 64, &privkeyBytes);
    char inp [] = {};
    memset( inp, 0, privkey.length() );
    memcpy(inp,privkey.c_str(),privkey.length());
    char dest [sizeof(inp)/2] = {};
    RLP::hex2bin(inp,dest);

    std::string txRLP = RLP::encode(tx,false);
    Keccak k;
    std::string myHash  = k(txRLP);
    uint8_t *hashBytes;
    hexstrToByteArray(myHash.c_str(), 64, &hashBytes);

    uint8_t r[32];
    uint8_t s[64];
    uint8_t recId;

    ecdsaSign((BigNum256)r, (BigNum256)s, (BigNum256)hashBytes, (BigNum256)privkeyBytes, &recId);
    reverse(r, 32);
    reverse(s, 32);
    char* rCharArr = uintToCharArr(r,32);
    char* sCharArr = uintToCharArr(s,32);
    int vInt = EIP_155_OFFSET + (int) recId + 2 * tx.chainId;
    char *vChar = (char *) calloc(50,1);
    snprintf(vChar,50,"%x",vInt);

    tx.v = std::string(vChar);
    tx.r = RLP::bytesToHex(std::string(rCharArr,32));
    tx.s = RLP::bytesToHex(std::string(sCharArr,32));
    free(rCharArr);
    free(sCharArr);
    free(vChar);

    return RLP::bytesToHex(RLP::encode(tx,false));
}

std::string SignTransaction::sign_message(QString message, std::string privkey) {
    privkey = RLP::removeHexFormatting(privkey);

    uint8_t *privkeyBytes;
    hexstrToByteArray(privkey.c_str(), 64, &privkeyBytes);
    char inp [] = {};
    memset( inp, 0, privkey.length() );
    memcpy(inp,privkey.c_str(),privkey.length());
    char dest [sizeof(inp)/2] = {};
    RLP::hex2bin(inp,dest);

    message = "Ethereum Signed Message:\n" + QString::number(message.size()) + message;
    std::string txRLP = RLP::hexToBytes("19" + Util::str2bytes(message).toStdString());
    Keccak k;
    std::string myHash  = k(txRLP);
    uint8_t *hashBytes;
    hexstrToByteArray(myHash.c_str(), 64, &hashBytes);

    uint8_t r[32];
    uint8_t s[64];
    uint8_t recId;

    ecdsaSign((BigNum256)r, (BigNum256)s, (BigNum256)hashBytes, (BigNum256)privkeyBytes, &recId);
    reverse(r, 32);
    reverse(s, 32);
    char* rCharArr = uintToCharArr(r,32);
    char* sCharArr = uintToCharArr(s,32);
    int vInt = 25 + (int) recId + 2;
    char *vChar = (char *) calloc(50,1);
    snprintf(vChar,50,"%x",vInt);

    Transaction tx;
    tx.v = std::string(vChar);
    tx.r = RLP::bytesToHex(std::string(rCharArr,32));
    tx.s = RLP::bytesToHex(std::string(sCharArr,32));

    free(rCharArr);
    free(sCharArr);
    free(vChar);

    return tx.r+tx.s+tx.v;
}
