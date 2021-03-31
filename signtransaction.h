#ifndef SIGNTRANSACTION_H
#define SIGNTRANSACTION_H

#include <iostream>
#include "rlp.h"
#include <string>
#include "keccak256.h"
#include <memory.h>
#include <stdlib.h>
#include "ecdsa.h"


#define EIP_155_OFFSET 35


class SignTransaction
{
private:
    static char* uintToCharArr(uint8_t src[], int len);
    static void hexstrToByteArray(const char hexstr[], size_t hexstr_size, uint8_t **pp_arr_out);
    static void reverse(uint8_t *p_arr, size_t len);

public:
    static std::string fixHexValue(std::string hex);
    static std::string sign_transaction(Transaction tx, std::string privkey);
};

#endif // SIGNTRANSACTION_H
