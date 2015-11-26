#include "StdAfx.h"
#include "AES.h"
#include <bitset>
#include <utility>

using namespace std;

static const unsigned char DEFAULT_KEY[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 'a', 'b', 'c', 'd', 'e', 'f'};

AES::AES() {
    BuildSBox();
    BuildInvSBox();
    m_Base64_Table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
}

void AES::EncryptionProcess() {
    //    加密过程
    InitialState(plainText);
    KeyExpansion();    //    密钥扩展
    AddRoundKey(0);    //    轮密钥加

    for (int i = 1; i < N_ROUND - 1; ++i) {
        Round(i);
    }

    FinalRound();
    InitialCipherText();
}

void AES::DecryptionProcess() {
    //    解密过程
    InitialState(cipherText);
    KeyExpansion();
    InvFinalRound();

    for (int i = N_ROUND - 2; i > 0 ; --i) {
        InvRound(i);
    }

    AddRoundKey(0);
    InitialplainText();
}

void AES::Round(const int& round) {
    //    正常轮
    SubBytes();
    ShiftRows();
    MixColumns();
    AddRoundKey(round);
}

void AES::InvRound(const int& round) {
    //    正常轮的逆
    AddRoundKey(round);
    InvMixColumns();
    InvShiftRows();
    InvSubBytes();
}

void AES::FinalRound() {
    //    最后轮
    SubBytes();
    ShiftRows();
    AddRoundKey(N_ROUND - 1);
}

void AES::InvFinalRound() {
    //    最后轮的逆
    AddRoundKey(N_ROUND - 1);
    InvShiftRows();
    InvSubBytes();
}

void AES::KeyExpansion() {
    //    密钥扩展
    static const byte rcon[N_ROUND][4] = {
        {0x00, 0x00, 0x00, 0x00},
        {0x01, 0x00, 0x00, 0x00},
        {0x02, 0x00, 0x00, 0x00},
        {0x04, 0x00, 0x00, 0x00},
        {0x08, 0x00, 0x00, 0x00},
        {0x10, 0x00, 0x00, 0x00},
        {0x20, 0x00, 0x00, 0x00},
        {0x40, 0x00, 0x00, 0x00},
        {0x80, 0x00, 0x00, 0x00},
        {0x1b, 0x00, 0x00, 0x00},
        {0x36, 0x00, 0x00, 0x00}
    };

    for (int i = 0; i < 16; ++i) {
        roundKey[0][i] = cipherKey[i];
    }

    for (int i = 0; i < 4; ++i) {
        //  roundKey[0][16]为cipherKey的转置矩阵
        for (int j = 0; j < 4; ++j) {
            roundKey[0][4 * i + j] = cipherKey[4 * j + i];
        }
    }

    for (int roundIndex = 1; roundIndex < N_ROUND; ++roundIndex) {
        byte rotWord[4] = {0x00};
        rotWord[0] = roundKey[roundIndex - 1][3];
        rotWord[1] = roundKey[roundIndex - 1][7];
        rotWord[2] = roundKey[roundIndex - 1][11];
        rotWord[3] = roundKey[roundIndex - 1][15];
        std::swap<byte>(rotWord[0], rotWord[1]);
        std::swap<byte>(rotWord[1], rotWord[2]);
        std::swap<byte>(rotWord[2], rotWord[3]);

        for (int i = 0; i < 4; ++i) {
            rotWord[i] = SBox[ rotWord[i] >> 4][ rotWord[i] & 0x0f ];
            roundKey[roundIndex][4 * i] = roundKey[roundIndex - 1][4 * i] ^ rotWord[i] ^ rcon[roundIndex][i];
        }

        for (int j = 1; j < 4; ++j) {
            for (int i = 0; i < 4; ++i) {
                roundKey[roundIndex][4 * i + j] = roundKey[roundIndex - 1][4 * i + j] ^ roundKey[roundIndex][4 * i +
                                                  j - 1];
            }
        }
    }
}

void AES::AddRoundKey(const int& round) {
    //    轮密钥加
    for (int i = 0; i < 16; ++i) {
        //    利用当前分组state和第round组扩展密钥进行按位异或
        state[i] ^= roundKey[round][i];
    }
}

void AES::SubBytes() {
    //    字节代换
    for (int i = 0; i < 16; ++i) {
        state[i] = SBox[ state[i] >> 4][ state[i] & 0x0f ];
    }
}

void AES::InvSubBytes() {
    //    逆字节代换
    for (int i = 0; i < 16; ++i) {
        state[i] = InvSBox[ state[i] >> 4][ state[i] & 0x0f ];
    }
}

void AES::ShiftRows() {
    //    行变换
    //state第一行保持不变
    // Do nothing.
    //state第二行循环左移一个字节
    std::swap<byte>(state[4], state[5]);
    std::swap<byte>(state[5], state[6]);
    std::swap<byte>(state[6], state[7]);
    //state第三行循环左移两个字节
    std::swap<byte>(state[8], state[10]);
    std::swap<byte>(state[9], state[11]);
    //state第三行循环左移三个字节
    std::swap<byte>(state[14], state[15]);
    std::swap<byte>(state[13], state[14]);
    std::swap<byte>(state[12], state[13]);
}

void AES::InvShiftRows() {
    //    行变换反演
    //state第一行保持不变
    // Do nothing.
    //state第二行循环右移一个字节
    std::swap<byte>(state[6], state[7]);
    std::swap<byte>(state[5], state[6]);
    std::swap<byte>(state[4], state[5]);
    //state第三行循环右移两个字节
    std::swap<byte>(state[9], state[11]);
    std::swap<byte>(state[8], state[10]);
    //state第三行循环右移三个字节
    std::swap<byte>(state[12], state[13]);
    std::swap<byte>(state[13], state[14]);
    std::swap<byte>(state[14], state[15]);
}

void AES::MixColumns() {
    //    列混淆
    static const byte matrix[4][4] = {
        {0x02, 0x03, 0x01, 0x01},
        {0x01, 0x02, 0x03, 0x01},
        {0x01, 0x01, 0x02, 0x03},
        {0x03, 0x01, 0x01, 0x02}
    };
    const byte* temp = GFMultplyBytesMatrix(&matrix[0][0], state);

    for (int i = 0; i < 16; ++i) {
        state[i] = temp[i];
    }

    delete[] temp;
}

void AES::InvMixColumns() {
    //    列混淆反演
    static const byte matrix[4][4] = {
        {0x0e, 0x0b, 0x0d, 0x09},
        {0x09, 0x0e, 0x0b, 0x0d},
        {0x0d, 0x09, 0x0e, 0x0b},
        {0x0b, 0x0d, 0x09, 0x0e}
    };
    const byte* temp = GFMultplyBytesMatrix(&matrix[0][0], state);

    for (int i = 0; i < 16; ++i) {
        state[i] = temp[i];
    }

    delete[] temp;
}

void AES::BuildSBox() {
    //    构建S盒
    static const byte box[16][16] = {
        /*        0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
        /*0*/  {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76},
        /*1*/  {0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0},
        /*2*/  {0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15},
        /*3*/  {0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75},
        /*4*/  {0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84},
        /*5*/  {0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf},
        /*6*/  {0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8},
        /*7*/  {0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2},
        /*8*/  {0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73},
        /*9*/  {0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb},
        /*a*/  {0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79},
        /*b*/  {0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08},
        /*c*/  {0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a},
        /*d*/  {0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e},
        /*e*/  {0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf},
        /*f*/  {0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16}
    };

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            SBox[i][j] = box[i][j];
        }
    }
}

void AES::BuildInvSBox() {
    //    构建逆S盒
    static const byte box[16][16] = {
        /*        0     1     2     3     4     5     6     7     8     9     a     b     c     d     e     f */
        /*0*/  {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb},
        /*1*/  {0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb},
        /*2*/  {0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e},
        /*3*/  {0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25},
        /*4*/  {0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92},
        /*5*/  {0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84},
        /*6*/  {0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06},
        /*7*/  {0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b},
        /*8*/  {0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73},
        /*9*/  {0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e},
        /*a*/  {0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b},
        /*b*/  {0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4},
        /*c*/  {0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f},
        /*d*/  {0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef},
        /*e*/  {0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61},
        /*f*/  {0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d}
    };

    for (int i = 0; i < 16; ++i) {
        for (int j = 0; j < 16; ++j) {
            InvSBox[i][j] = box[i][j];
        }
    }
}

void AES::InitialState(const byte* text) {
    //    state初始时候为明(密)文矩阵的转置矩阵
    for (int i = 0; i < 4; ++i) {
        //转置text存放在state中
        for (int j = 0; j < 4; ++j) {
            state[4 * i + j] = text[4 * j + i];
        }
    }
}

void AES::InitialCipherText() {
    //    state被复制到输出矩阵中
    for (int i = 0; i < 4; ++i) {
        //转置state存放在cipherText中
        for (int j = 0; j < 4; ++j) {
            cipherText[4 * i + j] = state[4 * j + i];
        }
    }

    cipherText[16] = 0;
}

void AES::InitialplainText() {
    //    state被复制到输入矩阵中
    for (int i = 0; i < 4; ++i) {
        //转置state存放在plainText中
        for (int j = 0; j < 4; ++j) {
            plainText[4 * i + j] = state[4 * j + i];
        }
    }

    plainText[16] = 0;
}

AES::byte AES::GFMultplyByte(const byte& left, const byte& right) {
    //有限域GF(2^8)上的乘法
    byte temp[8];
    bitset<8> bits((unsigned long)right);    //把right化为个二进制位存放在bits中
    temp[0] = left;

    for (int i = 1; i < 8; ++i) {
        if (temp[i - 1] >= 0x80) { //若(temp[i-1] 首位为"1"
            temp[i] = temp[i - 1] << 1;
            temp[i] = temp[i] ^ 0x1b;    //与(00011011)异或
        } else {
            temp[i] = temp[i - 1] << 1;
        }
    }

    byte result = 0x00;

    for (int i = 0; i < 8; ++i) {
        if (bits[i] == 1) {
            result ^= temp[i];
        }
    }

    return result;
}

const AES::byte* AES::GFMultplyBytesMatrix(const byte* left, const byte* right) {
    //有限域GF(2^8)上的矩阵(4*4)乘法
    AES::byte* result = new AES::byte[16];

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            result[4 * i + j] = GFMultplyByte(left[4 * i], right[j]);

            for (int k = 1; k < 4; ++k) {
                result[4 * i + j] ^= GFMultplyByte(left[4 * i + k], right[4 * k + j]);
            }
        }
    }

    return result;
}

const AES::byte* AES::Cipher(const byte* text, const int textLen, const byte* key,
                             const int keyLen) {
    memset(plainText, 0, sizeof(plainText));
    memset(cipherKey, 0, sizeof(cipherKey));

    if (textLen > 16) {
        return 0;
    }

    if (keyLen > 16) {
        return 0;
    }

    //    用key给text加密
    for (int i = 0; i < textLen; ++i) {
        plainText[i] = text[i];
    }

    for (int i = 0; i < keyLen; ++i) {
        cipherKey[i] = key[i];
    }

    EncryptionProcess();
    return cipherText;
}

const AES::byte* AES::InvCipher(const byte* text, const int textLen, const byte* key,
                                const int keyLen) {
    memset(cipherText, 0, sizeof(cipherText));
    memset(cipherKey, 0, sizeof(cipherKey));

    if (textLen > 16) {
        return 0;
    }

    if (keyLen > 16) {
        return 0;
    }

    //    用key给text解密
    for (int i = 0; i < textLen; ++i) {
        cipherText[i] = text[i];
    }

    for (int i = 0; i < keyLen; ++i) {
        cipherKey[i] = key[i];
    }

    DecryptionProcess();
    return plainText;
}


void AES::EncodeBase64(CString& strSource, CString& strOut) {
    /*
    * 以下是操作二进制数时用到的
    * 11111100   0xFC
    * 11000000   0x3
    * 11110000   0xF0
    * 00001111   0xF
    * 11000000   0xC0
    * 00111111   0x3F
    */

    CString strEncode;
    char cTemp[4];

    //行长度,MIME规定Base64编码行长为76字节
    int LineLength = 0;
    int sourceLen = strSource.GetLength();

    for (int i = 0; i < sourceLen; i += 3) {
        memset(cTemp, 0, 4);

        cTemp[0] = strSource[i];

        if (i + 1 < sourceLen) {
            cTemp[1] = strSource[i + 1];
        }

        if (i + 2 < sourceLen) {
            cTemp[2] = strSource[i + 2];
        }

        int len = strlen(cTemp);

        if (len == 3) {
            strEncode += m_Base64_Table[((int)cTemp[0] & 0xFC) >> 2];
            strEncode += m_Base64_Table[((int)cTemp[0] & 0x3) << 4 | ((int)cTemp[1] & 0xF0) >> 4];
            strEncode += m_Base64_Table[((int)cTemp[1] & 0xF) << 2 | ((int)cTemp[2] & 0xC0) >> 6];
            strEncode += m_Base64_Table[(int)cTemp[2] & 0x3F];

            if (LineLength += 4 >= 76) {
                strEncode += "\r\n";
            }
        } else if (len == 2) {
            strEncode += m_Base64_Table[((int)cTemp[0] & 0xFC) >> 2];
            strEncode += m_Base64_Table[((int)cTemp[0] & 0x3) << 4 | ((int)cTemp[1] & 0xF0) >> 4];
            strEncode += m_Base64_Table[((int)cTemp[1] & 0x0F) << 2];

            if (LineLength += 4 >= 76) {
                strEncode += "\r\n";
            }

            strEncode += "=";
        } else if (len == 1) {
            strEncode += m_Base64_Table[((int)cTemp[0] & 0xFC) >> 2];
            strEncode += m_Base64_Table[((int)cTemp[0] & 0x3) << 4];

            if (LineLength += 4 >= 76) {
                strEncode += "\r\n";
            }

            strEncode += "==";
        }
    }

    strOut = strEncode;
}

void AES::DecodeBase64(CString& strSource, CString& strOut) {
    //返回值
    CString strDecode;
    char cTemp[5];

    int sourceLen = strSource.GetLength();

    for (int i = 0; i < sourceLen; i += 4) {
        memset(cTemp, 0, 5);

        cTemp[0] = strSource[i];

        if (i + 1 < sourceLen) {
            cTemp[1] = strSource[i + 1];
        }

        if (i + 2 < sourceLen) {
            cTemp[2] = strSource[i + 2];
        }

        if (i + 3 < sourceLen) {
            cTemp[3] = strSource[i + 3];
        }

        int asc[4];

        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < (int)strlen(m_Base64_Table); k++) {
                if (cTemp[j] == m_Base64_Table[k]) {
                    asc[j] = k;
                }
            }
        }

        if ('=' == cTemp[2] && '=' == cTemp[3]) {
            strDecode += (char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
        } else if ('=' == cTemp[3]) {
            strDecode += (char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
            strDecode += (char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
        } else {
            strDecode += (char)(int)(asc[0] << 2 | asc[1] << 2 >> 6);
            strDecode += (char)(int)(asc[1] << 4 | asc[2] << 2 >> 4);
            strDecode += (char)(int)(asc[2] << 6 | asc[3] << 2 >> 2);
        }
    }

    strOut = strDecode;
}

BOOL AES::Encrypt(CString& strIn, CString& strOut) {

    CString strEnc;
    LPCSTR buf = (LPCSTR)strIn;
    int len = strIn.GetLength();


    while (len > 0) {
        strEnc += (LPCSTR)Cipher((byte*)buf, min(len, 16), DEFAULT_KEY, KEY_SIZE);
        len -= 16;
        buf += 16;
    }

    EncodeBase64(strEnc, strOut);

    return TRUE;
}

BOOL AES::Decrypt(CString& strIn, CString& strOut) {

    CString strDec;
    DecodeBase64(strIn, strDec);

    LPCSTR buf = (LPCSTR)strDec;
    int len = strDec.GetLength();

    strOut.Empty();

    while (len > 0) {
        strOut += (LPCSTR)InvCipher((byte*)buf, min(len, 16), DEFAULT_KEY, KEY_SIZE);
        len -= 16;
        buf += 16;
    }

    return TRUE;
}
