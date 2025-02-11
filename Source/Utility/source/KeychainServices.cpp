// Copyright (C) 2014-2022 Michael Kazakov. Subject to GNU General Public License version 3.
#include <Security/Security.h>
#include "../include/Utility/KeychainServices.h"
#pragma clang diagnostic ignored "-Wdeprecated-declarations" // See issue #147

KeychainServices::KeychainServices()
{
}

KeychainServices &KeychainServices::Instance()
{
    static auto inst = new KeychainServices;
    return *inst;
}

bool KeychainServices::SetPassword(const std::string &_where,
                                   const std::string &_account,
                                   const std::string &_password)
{
    OSStatus result = SecKeychainAddInternetPassword(nullptr,
                                                     UInt32(_where.length()),
                                                     _where.c_str(),
                                                     0,
                                                     nullptr,
                                                     UInt32(_account.length()),
                                                     _account.c_str(),
                                                     0,
                                                     nullptr,
                                                     0,
                                                     kSecProtocolTypeAny,
                                                     kSecAuthenticationTypeDefault,
                                                     UInt32(_password.length()),
                                                     _password.c_str(),
                                                     nullptr);
    if( result == errSecDuplicateItem ) {
        SecKeychainItemRef item;
        result = SecKeychainFindInternetPassword(nullptr,
                                                 UInt32(_where.length()),
                                                 _where.c_str(),
                                                 0,
                                                 nullptr,
                                                 UInt32(_account.length()),
                                                 _account.c_str(),
                                                 0,
                                                 nullptr,
                                                 0,
                                                 kSecProtocolTypeAny,
                                                 kSecAuthenticationTypeDefault,
                                                 nullptr,
                                                 nullptr,
                                                 &item);
        if( result == 0 ) {
            result = SecKeychainItemModifyAttributesAndData(
                item, nullptr, UInt32(_password.length()), _password.c_str());
            CFRelease(item);
        }
    }

    return result == 0;
}

bool KeychainServices::GetPassword(const std::string &_where,
                                   const std::string &_account,
                                   std::string &_password)
{
    UInt32 len = 0;
    void *data = 0;
    OSStatus result = SecKeychainFindInternetPassword(nullptr,
                                                      UInt32(_where.length()),
                                                      _where.c_str(),
                                                      0,
                                                      nullptr,
                                                      UInt32(_account.length()),
                                                      _account.c_str(),
                                                      0,
                                                      nullptr,
                                                      0,
                                                      kSecProtocolTypeAny,
                                                      kSecAuthenticationTypeDefault,
                                                      &len,
                                                      &data,
                                                      nullptr);
    if( result == 0 ) {
        _password.assign(static_cast<char *>(data), len);
        SecKeychainItemFreeContent(nullptr, data);
        return true;
    }
    return false;
}

bool KeychainServices::ErasePassword(const std::string &_where, const std::string &_account)
{
    SecKeychainItemRef item;
    OSStatus result = SecKeychainFindInternetPassword(nullptr,
                                                      UInt32(_where.length()),
                                                      _where.c_str(),
                                                      0,
                                                      nullptr,
                                                      UInt32(_account.length()),
                                                      _account.c_str(),
                                                      0,
                                                      nullptr,
                                                      0,
                                                      kSecProtocolTypeAny,
                                                      kSecAuthenticationTypeDefault,
                                                      nullptr,
                                                      nullptr,
                                                      &item);
    if( result == 0 ) {
        result = SecKeychainItemDelete(item);
        CFRelease(item);
    }
    return result == 0;
}
