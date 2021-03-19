#ifndef VERIFYETHADDRESS_H
#define VERIFYETHADDRESS_H

#include <QString>

class VerifyEthAddress
{
public:
    VerifyEthAddress();
    static bool is_valid_eth_address(QString address);
    static bool is_checksum_address(QString address);
};

#endif // VERIFYETHADDRESS_H
