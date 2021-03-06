/*
 * OovString.cpp
 *
 *  Created on: Oct 6, 2013
 *  \copyright 2013 DCBlaha.  Distributed under the GPL.
 */
#include "OovString.h"
#include <memory.h>
#include <string.h>
#include <stdio.h>
#include <algorithm>

OovString OovStringVec::getStr(size_t index)
    {
    OovString str;
    if(index < size())
        str = at(index);
    return str;
    }

void OovStringVec::deleteEmptyStrings()
    {
    erase(std::remove_if(begin(), end(),
            [](const OovString &str){ return(str.length() == 0); }),
            end());
    }

static bool isAsciiLenOk(OovStringRef const buf, size_t maxLen)
    {
    bool success = false;
    for(size_t i=0; i<maxLen; i++)
        {
        if(buf[static_cast<int>(i)] == '\0')
            {
            success = true;
            break;
            }
        }
    return success;
    }

// srcBytes must include null.
static bool copyString(char * const dstStr, size_t maxDstBytes,
    OovStringRef const srcStr, size_t srcBytes)
    {
    size_t len = std::min(maxDstBytes, srcBytes);
    memcpy(dstStr, srcStr, len);
    dstStr[maxDstBytes-1] = '\0';
    return(srcBytes > maxDstBytes);
    }

bool StringToFloat(char const * const mbStr, float min, float max, float &val)
    {
    bool success = false;
    float valNum = 0;
    char dummyc = 0;
    int ret = sscanf(mbStr, "%f%c", &valNum, &dummyc);
    if(ret == 1 && (valNum >= min && valNum <= max))
        {
        val = valNum;
        success = true;
        }
    return success;
    }

bool StringToInt(char const * const mbStr, int min, int max, int &val)
    {
    bool success = false;
    int valNum = 0;
    char dummyc = 0;
    int ret = sscanf(mbStr, "%d%c", &valNum, &dummyc);
    if(ret == 1 && (valNum >= min && valNum <= max))
        {
        val = valNum;
        success = true;
        }
    return success;
    }

bool StringToUnsignedInt(char const * const mbStr, unsigned int min, unsigned int max,
        unsigned int &val)
    {
    bool success = false;
    unsigned int valNum = 0;
    char dummyc = 0;
    int ret = sscanf(mbStr, "%u%c", &valNum, &dummyc);
    if(ret == 1 && (valNum >= min && valNum <= max))
        {
        val = valNum;
        success = true;
        }
    return success;
    }

void StringToLower(OovString &str)
    {
    for(size_t i=0; i<str.length(); ++i)
        {
        str.at(i) = static_cast<char>(tolower(str.at(i)));
        }
    }

int StringCompareNoCase(char const * const str1, char const * const str2)
    {
    char const * p1 = str1;
    char const * p2 = str2;
    while(*p1)
        {
        if(tolower(*p1) != tolower(*p2))
            break;
        p1++;
        p2++;
        }
    return(*p1-*p2);
    }

int StringCompareNoCaseNumCharsMatch(char const * const str1, char const * const str2)
    {
    char const * p1 = str1;
    char const * p2 = str2;
    while(*p1)
        {
        if(tolower(*p1) != tolower(*p2))
            break;
        p1++;
        p2++;
        }
    return(p1-str1);
    }

// minDigits 0 doesn't print zero, 1 is normal minimum
static bool IntToAsciiString(int value, char * const buffer, size_t dstSizeInBytes,
    int radix, int minSpaces=0, int minDigits=1)
    {
    bool success = false;
    if(dstSizeInBytes > 0)
        {
        const size_t numBufSize = 30;
        char numBuf[numBufSize];
        if(radix == 10)
            snprintf(numBuf, sizeof(numBuf), "%*.*d", minSpaces, minDigits, value);
        else
            snprintf(numBuf, sizeof(numBuf), "%*.*x", minSpaces, minDigits, value);
        if(isAsciiLenOk(numBuf, dstSizeInBytes))
            {
            copyString(buffer, dstSizeInBytes, numBuf, numBufSize);
            success = true;
            }
        else if(dstSizeInBytes >= 2)
            {
            buffer[0] = '*';
            buffer[1] = '\0';
            }
        else
            buffer[0] = '\0';
        }
    return success;
    }

static bool FloatToAsciiString(float value, char * const buffer, size_t dstSizeInBytes,
        int precision)
    {
    bool success = false;
    if(dstSizeInBytes > 0)
        {
        const size_t numBufSize = 30;
        char numBuf[numBufSize];
        snprintf(numBuf, sizeof(numBuf), "%.*f", precision, value);
        if(isAsciiLenOk(numBuf, dstSizeInBytes))
            {
            copyString(buffer, dstSizeInBytes, numBuf, numBufSize);
            success = true;
            }
        else if(dstSizeInBytes >= 2)
            {
            buffer[0] = '*';
            buffer[1] = '\0';
            }
        else
            buffer[0] = '\0';
        }
    return success;
    }

// This can count the number of multibyte or ASCII characters.
// Count all first-bytes (the ones that don't match 10xxxxxx).
size_t StringNumChars(char const * const str)
    {
    size_t len = 0;
    char const *p = str;
    while(*p)
            {
            len += ((*p++ & 0xc0) != 0x80);
            }
    return len;
    }

size_t StringNumBytes(char const * const str)
    {
    return strlen(str);
    }

bool StringIsAscii(char const * const str)
    {
    char const *p = str;
    while(*p)
            {
            if(*p & 0x80)       // Stop on any character that is not ASCII
                    break;
            p++;
            }
    return *p == '\0';  // If at end, there is no non-ASCII character
    }

size_t StringFindSpace(char const * const str, size_t startPos)
    {
    char const * p = str + startPos;
    while(*p)
        {
        if(isspace(*p))
            break;
        else
            p++;
        }
    if(*p == '\0')
        return std::string::npos;
    else
        return static_cast<size_t>(p - str);
    }

size_t StringFindNonSpace(char const * const str, size_t startPos)
    {
    char const * p = str + startPos;
    while(*p)
        {
        if(!isspace(*p))
            break;
        else
            p++;
        }
    if(*p == '\0')
        return std::string::npos;
    else
        return static_cast<size_t>(p - str);
    }

OovString StringMakeXml(char const * const text)
    {
    OovString outStr;
    char const *p = text;
    while(*p)
        {
        switch(*p)
            {
            case '>':   outStr += "&gt;";       break;
            case '<':   outStr += "&lt;";       break;
            case '&':   outStr += "&amp;";      break;
            case '\'':  outStr += "&apos;";     break;
            case '\"':  outStr += "&quot;";     break;
            default:    outStr += *p;           break;
            }
        p++;
        }
    return outStr;
    }

OovString StringTrim(char const * const text)
    {
    OovString outStr;
    char const * p = text;
    while(*p)
        {
        if(!isspace(*p))
            {
            break;
            }
        p++;
        }
    char const *endp = text + strlen(text)-1;
    while(*endp >= 0)
        {
        if(!isspace(*endp))
            {
            break;
            }
        endp--;
        }
    while(p <= endp)
        {
        outStr += *p;
        p++;
        }
    return outStr;
    }

OovStringVec StringSplit(char const * const str, char delimiter)
    {
    return StringSplit(str, std::string(1, delimiter).c_str());
    }

OovStringVec StringSplit(char const * const str, char const * const delimiterStr)
{
    OovString tempStr = str;
    OovStringVec tokens;
    size_t start = 0;
    size_t end = 0;
    const size_t delimLen = strlen(delimiterStr);
    while(end != std::string::npos)
        {
        end = tempStr.find(delimiterStr, start);
        size_t len = (end == std::string::npos) ? std::string::npos : end - start;
        OovString splitStr = tempStr.substr(start, len);
        tokens.push_back(splitStr);
        start = (( end > (std::string::npos - delimLen)) ?
                std::string::npos : end + delimLen);
        }
    return tokens;
    }

OovStringVec StringSplit(char const * const str, OovStringVec const &delimiters,
    bool keepZeroLengthStrings)
    {
    OovString tempStr = str;
    OovStringVec tokens;
    size_t start = 0;
    size_t end = 0;
    size_t lowestEnd = 0;
    while(end != std::string::npos)
        {
        lowestEnd = std::string::npos;
        size_t delimLen = std::string::npos;
        for(auto const &delim : delimiters)
            {
            end = tempStr.find(delim, start);
            if(end < lowestEnd)
                {
                delimLen = strlen(delim.getStr());
                lowestEnd = end;
                }
            }
        end = lowestEnd;
        /// Don't add zero length strings when two delimiters are next to each other.
        size_t len = (end == std::string::npos) ? std::string::npos : end - start;
        if(keepZeroLengthStrings || len > 0)
            {
            OovString splitStr = tempStr.substr(start, len);
            if(splitStr.length() > 0)
                {
                tokens.push_back(splitStr);
                }
            }
        start = (( end > (std::string::npos - delimLen)) ?
                std::string::npos : end + delimLen);
        }
    return tokens;
    }

OovString StringJoin(OovStringVec const &tokens, char delimiter)
    {
    OovString str;
    for(size_t i=0; i<tokens.size(); i++)
        {
        str += tokens[i];
        if(i < tokens.size()-1)
            {
            str += delimiter;
            }
        }
    return str;
    }

//////////////

void OovString::setUpperCase(OovStringRef const str)
    {
    char const *p = str;
    while(*p)
        {
        *this += static_cast<char>(toupper(*p));
        p++;
        }
    }

void OovString::setLowerCase(OovStringRef const str)
    {
    *this = str;
    StringToLower(*this);
    }

void OovString::appendInt(int val, int radix, int minSpaces, int minDigits)
    {
    char buf[30];
    IntToAsciiString(val, buf, sizeof(buf), radix, minSpaces, minDigits);
    append(buf);
    }

void OovString::appendFloat(float val, int precision)
    {
    char buf[30];
    FloatToAsciiString(val, buf, sizeof(buf), precision);
    append(buf);
    }

bool OovString::replaceStrs(OovStringRef const srchStr, OovStringRef const repStr)
    {
    size_t starti = 0;
    bool didReplace = false;
    do
        {
        starti = find(srchStr, starti);
        if(starti != OovString::npos)
            {
            didReplace = true;
            std::string::replace(starti, strlen(srchStr), repStr);
            starti+=strlen(repStr.getStr());
            }
        } while(starti != OovString::npos);
    return didReplace;
    }
