#ifndef AES_H_
#define AES_H_

class AES {
private:
    typedef unsigned char byte;
    static const int KEY_SIZE = 16;    //    密钥长度为128位
    static const int N_ROUND = 11;
    byte plainText[17];    //    明文
    byte state[16];    //    当前分组。
    byte cipherKey[16];    //    密钥
    byte roundKey[N_ROUND][16];    //轮密钥
    byte cipherText[17];    //密文
    byte SBox[16][16];    //    S盒
    byte InvSBox[16][16];    //    逆S盒

protected:
    void EncryptionProcess();
    void DecryptionProcess();
    void Round(const int& round);
    void InvRound(const int& round);
    void FinalRound();
    void InvFinalRound();
    void KeyExpansion();
    void AddRoundKey(const int& round);
    void SubBytes();
    void InvSubBytes();
    void ShiftRows();
    void InvShiftRows();
    void MixColumns();
    void InvMixColumns();
    void BuildSBox();
    void BuildInvSBox();
    void InitialState(const byte* text);
    void InitialCipherText();
    void InitialplainText();
    byte GFMultplyByte(const byte& left, const byte& right);
    const byte* GFMultplyBytesMatrix(const byte* left, const byte* right);


protected:
    //Base64编码解码表
    char* m_Base64_Table;
    void EncodeBase64(CString& strSource, CString& strOut);
    void DecodeBase64(CString& strSource, CString& strOut);

public:
    AES();
    const byte* Cipher(const byte* text, const int textLen, const byte* key, const int keyLen);
    const byte* InvCipher(const byte* text, const int textLen, const byte* key, const int keyLen);

    BOOL Encrypt(CString& strIn, CString& strOut);
    BOOL Decrypt(CString& strIn, CString& strOut);
};

#endif //AES_H_