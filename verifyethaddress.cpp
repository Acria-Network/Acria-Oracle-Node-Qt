#include "verifyethaddress.h"
#include "sha3.h"
#include "keccak256.h"

#include <QRegularExpression>
#include <QDebug>

VerifyEthAddress::VerifyEthAddress()
{

}

bool VerifyEthAddress::is_valid_eth_address(QString address) {
    if (!QRegularExpression("^(0x)?[0-9a-fA-F]{40}$").match(address).hasMatch()) {
        // check if it has the basic requirements of an address
        return false;
    } else if (QRegularExpression("^(0x)?[0-9a-f]{40}$").match(address).hasMatch() || QRegularExpression("^(0x)?[0-9A-F]{40}$").match(address).hasMatch()) {
        // If it's all small caps or all all caps, return true
        return true;
    } else {
        // Otherwise check each case
        return is_checksum_address(address);
    }
};

bool VerifyEthAddress::is_checksum_address(QString address) {
    // Check each case
    Keccak keccak;
    address = address.replace("0x","");
    QString addressHash = QString::fromStdString(keccak(address.toLower().toStdString()));

    for (uint i = 0; i < 40; i++ ) {
        // the nth letter should be uppercase if the nth digit of casemap is 1
        if ((QString(addressHash.at(i)).toUInt(NULL, 16) > 7 && address.toUpper().at(i) != address.at(i)) || (QString(addressHash.at(i)).toUInt(NULL, 16) <= 7 && address.toLower().at(i) != address.at(i))) {
            return false;
        }
    }
    return true;
};
