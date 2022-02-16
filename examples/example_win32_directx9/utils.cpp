#include "utils.h"

const std::string vformat(const char* const zcFormat, ...) {

    // initialize use of the variable argument array
    va_list vaArgs;
    va_start(vaArgs, zcFormat);

    // reliably acquire the size
    // from a copy of the variable argument array
    // and a functionally reliable call to mock the formatting
    va_list vaArgsCopy;
    va_copy(vaArgsCopy, vaArgs);
    const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
    va_end(vaArgsCopy);

    // return a formatted string without risking memory mismanagement
    // and without assuming any compiler or platform specific behavior
    std::vector<char> zc(iLen + 1);
    std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
    va_end(vaArgs);
    return std::string(zc.data(), iLen);
}
void info(std::string str) {
    //vector<int>::iteratork = vecInt.begin();

    //vecInt.erase(k);//ɾ����һ��Ԫ��
    if (gInfos.size()>21) {
        std::vector <std::string>::iterator iterat = gInfos.begin();
        gInfos.erase(iterat);
    }
    gInfos.push_back(str);
}
