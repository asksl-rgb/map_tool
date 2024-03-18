#ifndef UTF8TOGBEX_H
#define UTF8TOGBEX_H

#include <string>
#include <QString>
#include <QTextCodec>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

/*  跨平台用UTF8ToGBEx, Windows平台用UTF8ToGBK */

inline std::string UTF8ToGBEx(const char *utf8)
{
    #ifdef _WIN32

        if (!utf8 || utf8[0] == '\0')
            return "";

        std::string result;
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (len > 0)
        {
            wchar_t *wstr = new wchar_t[len];
            memset(wstr, 0, len * sizeof(wchar_t));
            if (MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len) > 0)
            {
                len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
                if (len > 0)
                {
                    char *str = new char[len];
                    memset(str, 0, len);
                    if (WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL) > 0)
                    {
                        result = str;
                    }
                    delete[] str;
                }
            }
            delete[] wstr;
        }
        return result;

    #else

        return utf8;

    #endif // _WIN32
}

inline std::string UTF8ToGBEx(const std::string& _utf8)
{
    #ifdef _WIN32

        const char *utf8 = _utf8.c_str();

        if (!utf8 || utf8[0] == '\0')
            return "";

        std::string result;
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (len > 0)
        {
            wchar_t *wstr = new wchar_t[len];
            memset(wstr, 0, len * sizeof(wchar_t));
            if (MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len) > 0)
            {
                len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
                if (len > 0)
                {
                    char *str = new char[len];
                    memset(str, 0, len);
                    if (WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL) > 0)
                    {
                        result = str;
                    }
                    delete[] str;
                }
            }
            delete[] wstr;
        }
        return result;

    #else

        return _utf8;

    #endif // _WIN32
}

inline std::string UTF8ToGBEx(const QString& __utf8)
{
    #ifdef _WIN32

        std::string utf8Str = __utf8.toStdString();
        const char *utf8 = utf8Str.c_str();

        if (!utf8 || utf8[0] == '\0')
            return "";

        std::string result;
        int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
        if (len > 0)
        {
            wchar_t *wstr = new wchar_t[len];
            memset(wstr, 0, len * sizeof(wchar_t));
            if (MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len) > 0)
            {
                len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
                if (len > 0)
                {
                    char *str = new char[len];
                    memset(str, 0, len);
                    if (WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL) > 0)
                    {
                        result = str;
                    }
                    delete[] str;
                }
            }
            delete[] wstr;
        }
        return result;

    #else

        return __utf8.toStdString();

    #endif // _WIN32
}

inline std::string UTF8ToGBK(const std::string& _utf8)
{
    QTextCodec* codec = QTextCodec::codecForName("GBK");
    QByteArray gbkBytes = codec->fromUnicode(QString::fromUtf8(_utf8.c_str()));
    std::string gbkString = gbkBytes.constData();
    return gbkString;
}

inline std::string UTF8ToGBK(const QString& __utf8)
{
    QTextCodec* codec = QTextCodec::codecForName("gbk");
    std::string gbkString = codec->fromUnicode(__utf8).data();    //注意QString是无法保存gbk编码的，需要直接转换成std::string
    return gbkString;
}

#endif // UTF8TOGBEX_H
