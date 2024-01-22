#ifndef KEY_FUNCTIONS_H
#define KEY_FUNCTIONS_H

#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <QString>
#include <QList>

class key_functions
{
public:
    key_functions();
    bool isPrime(boost::multiprecision::cpp_int n, int k);
    bool millerTest(boost::multiprecision::cpp_int d, boost::multiprecision::cpp_int n);
    boost::multiprecision::cpp_int prime();
    boost::multiprecision::cpp_int find_n(boost::multiprecision::cpp_int q, boost::multiprecision::cpp_int p);
    boost::multiprecision::cpp_int find_fi(boost::multiprecision::cpp_int q, boost::multiprecision::cpp_int p);
    boost::multiprecision::cpp_int gcd(boost::multiprecision::cpp_int e, boost::multiprecision::cpp_int fi);
    boost::multiprecision::cpp_int search_e(boost::multiprecision::cpp_int fi);
    boost::multiprecision::cpp_int search_d(boost::multiprecision::cpp_int e, boost::multiprecision::cpp_int fi);

    std::string encode(boost::multiprecision::cpp_int number);
    std::string encode_string(QString number);
    std::string base64(std::string input);

    std::string decode_base64(std::string input);
    boost::multiprecision::cpp_int decode(QString encoded_string);
    QVector<boost::multiprecision::cpp_int> decode_sign(QString encoded_string);

    QString hash(QString data);
    QString hexToPaddedBinary(QString hex);
    QList<QString> splitToBlocks(QString binary_string);
    QVector<boost::multiprecision::cpp_int> binToInt(QList<QString> binary_list);
    QVector<QString> IntToBin(QVector<boost::multiprecision::cpp_int> cipher_list);
    QString cppIntToBinaryString(boost::multiprecision::cpp_int number, int bitCount);

    QVector<boost::multiprecision::cpp_int> block_cipher(QList<boost::multiprecision::cpp_int> block, boost::multiprecision::cpp_int decoded_priv_key, boost::multiprecision::cpp_int decoded_shared_key);
    QString list_to_string(QVector<boost::multiprecision::cpp_int> ciphered_block);
    boost::multiprecision::cpp_int string_to_number(QString string);
    QString concatenateBinaryBlocks(QVector<QString> bin_blok);
    QString binToHex(QString str_bin);

};

#endif // KEY_FUNCTIONS_H
