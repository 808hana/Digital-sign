#include "key_functions.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <string>
#include <sstream>
#include <chrono>
#include <QCryptographicHash>
#include <QList>
#include <bitset>


key_functions::key_functions() {

}

bool key_functions::isPrime(boost::multiprecision::cpp_int n, int k) {
    if (n <= 1 || n == 4) return false;
    if (n <= 3) return true;
    boost::multiprecision::cpp_int d = n - 1;
    while (d % 2 == 0)
        d /= 2;

    for (int i = 0; i < k; i++)
        if (!millerTest(d, n))
            return false;

    return true;
}

bool key_functions::millerTest(boost::multiprecision::cpp_int d, boost::multiprecision::cpp_int n) {
    boost::random::mt19937_64 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> dist(2, n - 2);
    boost::multiprecision::cpp_int a = dist(rng);

    boost::multiprecision::cpp_int x = boost::multiprecision::powm(a, d, n);

    if (x == 1 || x == n - 1)
        return true;

    while (d != n - 1) {
        x = (x * x) % n;
        d *= 2;

        if (x == 1) return false;
        if (x == n - 1) return true;
    }
    return false;
}


boost::multiprecision::cpp_int key_functions::prime(){
    unsigned seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    boost::random::mt19937_64 gen(seed);
    boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> distribution(100000000000000000, 1000000000000000000);
    boost::multiprecision::cpp_int prime_candidate = distribution(gen);
    prime_candidate |= 1; //has to be odd
    while (!isPrime(prime_candidate, 5)) {  // Using 5 iterations for the Miller-Rabin test
        prime_candidate = distribution(gen);
        prime_candidate |= 1;  // Ensure the number is odd
    }
    return prime_candidate;
}


boost::multiprecision::cpp_int key_functions::find_n(boost::multiprecision::cpp_int q, boost::multiprecision::cpp_int p){
    boost::multiprecision::cpp_int n = q*p;
    return n;
}

boost::multiprecision::cpp_int key_functions::find_fi(boost::multiprecision::cpp_int q, boost::multiprecision::cpp_int p){
    boost::multiprecision::cpp_int fi = (p-1)*(q-1);
    return fi;
}

boost::multiprecision::cpp_int key_functions::gcd(boost::multiprecision::cpp_int e, boost::multiprecision::cpp_int fi) {
    if (fi == 0) {
        return e;
    }
    return gcd(fi, e % fi);
}

boost::multiprecision::cpp_int key_functions::search_e(boost::multiprecision::cpp_int fi){
    boost::random::mt19937_64 gen;
    boost::random::uniform_int_distribution<boost::multiprecision::cpp_int> distribution(1, fi);
    boost::multiprecision::cpp_int e = distribution(gen);
    while (gcd(e,fi) != 1){
        e = distribution(gen);
    }
    return e;
}

boost::multiprecision::cpp_int key_functions::search_d(boost::multiprecision::cpp_int e, boost::multiprecision::cpp_int fi){
    boost::multiprecision::cpp_int d = boost::integer::mod_inverse(e, fi);
    return d;
}

std::string key_functions::base64(std::string input){
    using namespace boost::archive::iterators;
    using base64_enc = base64_from_binary<transform_width<std::string::const_iterator, 6, 8>>;
    std::stringstream ss;
    std::copy(base64_enc(input.begin()), base64_enc(input.end()), std::ostream_iterator<char>(ss));
    size_t num_equals = (3 - input.length() % 3) % 3;
    for (size_t i = 0; i < num_equals; i++) {
        ss.put('=');
    }
    return ss.str();

}

std::string key_functions::encode(boost::multiprecision::cpp_int number){
    std::stringstream ss;
    ss << number;
    std::string private_key = ss.str();
    std::string priv = base64(private_key);
    return priv;
}

std::string key_functions::encode_string(QString number){
    std::string stdNumber = number.toStdString();
    std::stringstream ss;
    ss << stdNumber;
    std::string private_key = ss.str();
    std::string priv = base64(private_key);
    return priv;
}

std::string key_functions::decode_base64(std::string input){
    using namespace boost::archive::iterators;
    typedef transform_width<binary_from_base64<std::string::const_iterator>, 8, 6> base64_dec;

    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
    input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());

    size_t padChars = count(input.begin(), input.end(), '=');
    std::string inputWithoutPadding = input.substr(0, input.length() - padChars);

    std::stringstream ss;
    std::copy(base64_dec(inputWithoutPadding.begin()), base64_dec(inputWithoutPadding.end()), std::ostream_iterator<char>(ss));
    return ss.str();
}

boost::multiprecision::cpp_int key_functions::decode(QString encoded_string){
    std::string decoded = decode_base64(encoded_string.toStdString());
    boost::multiprecision::cpp_int number;
    std::istringstream iss(decoded);
    iss >> number;
    return number;
}

QVector<boost::multiprecision::cpp_int> key_functions::decode_sign(QString encoded_string) {
    std::string decoded = decode_base64(encoded_string.toStdString());
    std::istringstream iss(decoded);
    QVector<boost::multiprecision::cpp_int> numbers;
    std::string number;

    while (iss >> number) {
        boost::multiprecision::cpp_int num(number);
        numbers.push_back(num);
    }

    return numbers;
}

QString key_functions::hash(QString data){
    QCryptographicHash hash(QCryptographicHash::Sha3_512);
    hash.addData(data.toUtf8());
    return hash.result().toHex();
}

QString key_functions::hexToPaddedBinary(QString hex){
    QByteArray byteArray = QByteArray::fromHex(hex.toUtf8());
    QString binStr;
    for (int i = 0; i < byteArray.size(); ++i) {
        binStr += QString::number(static_cast<quint8>(byteArray[i]), 2).rightJustified(8, '0');
    }
    int paddingLength = 40 - (binStr.length() % 40);
    if (paddingLength != 40) {
        binStr = binStr.rightJustified(binStr.length() + paddingLength, '0');
    }

    return binStr;
}

QList<QString> key_functions::splitToBlocks(QString binary_string){
    QList<QString> blocks;
    for (int i = 0; i < binary_string.length(); i += 40) {
        blocks.append(binary_string.mid(i, 40));
    }
    return blocks;
}

QVector<boost::multiprecision::cpp_int> key_functions::binToInt(QList<QString> binary_list){
    QList<boost::multiprecision::cpp_int> int_blocks;
    for (QString block : binary_list){
        std::string std_block = block.toStdString();
        boost::multiprecision::cpp_int number = std::bitset<40>(std_block).to_ullong();
        int_blocks.append(number);
    }
    return int_blocks;
}

QString key_functions::cppIntToBinaryString(boost::multiprecision::cpp_int number, int bitCount){
    QString binaryString;
    boost::multiprecision::cpp_int mask = 1;

    for (int i = 0; i < bitCount; ++i) {
        binaryString.prepend(((number & mask) != 0) ? '1' : '0');
        mask <<= 1;
    }

    return binaryString;
}

QVector<QString> key_functions::IntToBin(QVector<boost::multiprecision::cpp_int> cipher_list){
    QVector<QString> bin_block;
    for (boost::multiprecision::cpp_int num : cipher_list){
        QString binStr = cppIntToBinaryString(num, 40);
        bin_block.append(binStr);
    }
    return bin_block;
}

QVector<boost::multiprecision::cpp_int> key_functions::block_cipher(QVector<boost::multiprecision::cpp_int> block, boost::multiprecision::cpp_int decoded_priv_key, boost::multiprecision::cpp_int decoded_shared_key){
    QVector<boost::multiprecision::cpp_int> cipher;
    for (int i = 0; i < block.size(); ++i) {
        boost::multiprecision::cpp_int code = boost::multiprecision::powm(block[i], decoded_priv_key, decoded_shared_key);
        cipher.append(code);
    }
    return cipher;
}

QString key_functions::list_to_string(QVector<boost::multiprecision::cpp_int> ciphered_block){
    QString result;
    for (int i = 0; i < ciphered_block.size(); ++i) {
        if (i > 0) {
            result += " ";
        }
        result += QString::fromStdString(ciphered_block[i].str());
    }
    return result.trimmed();
}

boost::multiprecision::cpp_int key_functions::string_to_number(QString string){
    std::string stdStr = string.toStdString();
    boost::multiprecision::cpp_int number(stdStr);
    return number;
}

QString key_functions::concatenateBinaryBlocks(QVector<QString> bin_blok) {
    QString concatenatedString;

    for (const QString& block : bin_blok) {
        concatenatedString += block;
    }
    return concatenatedString;
}

QString key_functions::binToHex(QString str_bin){
    boost::multiprecision::cpp_int num(0);
    for (QChar ch : str_bin) {
        num <<= 1;
        if (ch == '1') {
            num += 1;
        }
    }
    std::string str_hex = num.str(0, std::ios_base::hex);
    return QString::fromStdString(str_hex).toLower();
}
