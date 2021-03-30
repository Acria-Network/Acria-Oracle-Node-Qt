#include "rlp.h"
#include <QDebug>

std::string RLP::encode(std::string s)
{
    if (s.size()==0)
        return hexToBytes("80");
    else if(s.size()==1 && (unsigned char)s[0]<128)
        return s;
    else
        return encodeLength(s.size(), 128)+s;
}

std::string RLP::encode(Transaction tx, bool toSign)
{
    std::string serialized = hexToRlpEncode(tx.nonce)+hexToRlpEncode(tx.gasPrice)+hexToRlpEncode(tx.gasLimit)+hexToRlpEncode(tx.to)+hexToRlpEncode(tx.value)+hexToRlpEncode(tx.data);

    if(!toSign)
          serialized += hexToRlpEncode(tx.v)+hexToRlpEncode(tx.r)+hexToRlpEncode(tx.s);
    qDebug() <<QString::fromStdString(bytesToHex(encodeLength(serialized.length(),192)));
    qDebug() << QString::fromStdString(bytesToHex(encodeLength(serialized.length(),192)+serialized));
    //return hexToBytes(encodeLength(serialized.length(),192))+serialized;
    return encodeLength(serialized.length(),192)+serialized;
}

int char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, char* target)
{
  while(*src && src[1])
  {
    *(target++) = char2int(*src)*16 + char2int(src[1]);
    src += 2;
  }
}

std::string RLP::hexToBytes(std::string s){
    if (s.size() == 0)
        return "";
    char inp [s.length()];
    memset( inp, 0, s.length()*sizeof(char) );
    memcpy(inp,s.c_str(),s.length());
    char dest [sizeof(inp)/2];
    memset( dest, 0, s.length()/2*sizeof(char) );
    hex2bin(inp,dest);
    return std::string(dest,sizeof(dest));
}

std::string RLP::hexToRlpEncode(std::string s){
    s = removeHexFormatting(s);
    return encode(hexToBytes(s));
}

std::string RLP::removeHexFormatting(std::string s){
    if(s[0]=='0'&&s[1]=='x')
        return s.substr(2,s.length()-2);
    return s;
}

std::string RLP::encodeLength(int len, int offset)
{
    std::string temp;
    if(len<56){
        temp=(char)(len+offset);
        return temp;
    }
    else{
        std::string hexLength = intToHex(len);
        int	lLength = hexLength.length()/2;
        std::string fByte = intToHex(offset+55+lLength);
        qDebug() << QString::fromStdString(fByte);
        qDebug() << QString::fromStdString(hexLength);
        return hexToBytes(fByte+hexLength);
    }
}

std::string RLP::intToHex(int n){
    std::stringstream stream;
    stream << std::hex << n;
    std::string result( stream.str() );
    if(result.size() % 2)
        result = "0"+result;
    return result;
}

std::string RLP::bytesToHex(std::string input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}

int RLP::char2int(char input)
{
  if(input >= '0' && input <= '9')
    return input - '0';
  else if(input >= 'A' && input <= 'F')
    return input - 'A' + 10;
  else if(input >= 'a' && input <= 'f')
    return input - 'a' + 10;
  else
    return 0;
}

void RLP::hex2bin(const char* src, char* target)
{
  while(*src)
  {
      if (src[1])
        *(target++) = char2int(*src)*16 + char2int(src[1]);
      else
        *(target++) = char2int(*src);
    src += 2;
  }
}
