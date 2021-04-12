#include "signtransaction.h"
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
    /*
    Transaction tx;
    tx.nonce=fixHexValue("0x5");
    tx.gasPrice=fixHexValue("0x210000");
    tx.gasLimit=fixHexValue("0x100000");
    tx.to=fixHexValue("0x0000000000000000000000000000000000000000");
    tx.value=fixHexValue("0x10000000");
    tx.data=fixHexValue("0x7f7465737432000000000000000000000000000000000000000000000000000000600057");
    tx.chainId = 15;
    tx.v=fixHexValue(RLP::intToHex(tx.chainId));//as per EIP 155
*/
    //std::string privkey = "440ec6f6dc87dda4d2918a07d115ae4d290e290bd92e70e851eeb23de59d955a";
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

    return RLP::bytesToHex(RLP::encode(tx,false));
}
