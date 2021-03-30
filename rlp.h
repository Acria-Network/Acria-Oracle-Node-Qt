#ifndef RLP_h
#define RLP_h

#include <stdio.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <memory.h>

class Transaction
{
  public:
    std::string nonce;
    std::string gasPrice;
    std::string gasLimit;
    std::string to;
    std::string value;
    std::string data;
    int chainId;
    std::string v;
    std::string r;
    std::string s;
};

class RLP
{
  public:
    static std::string encode(std::string);
    static std::string encode(Transaction, bool);
    static std::string encodeLength(int, int);
    static std::string intToHex(int);
    static std::string bytesToHex(std::string);
    static std::string removeHexFormatting(std::string);
    static std::string hexToRlpEncode(std::string);
    static std::string hexToBytes(std::string);
    static int char2int(char);
    static void hex2bin(const char*, char*);
};

#endif
