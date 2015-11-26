#ifndef AES_H_
#define AES_H_

class AES {
private:
    typedef unsigned char byte;
    static const int KEY_SIZE = 16;    //    ��Կ����Ϊ128λ
    static const int N_ROUND = 11;
    byte plainText[17];    //    ����
    byte state[16];    //    ��ǰ���顣
    byte cipherKey[16];    //    ��Կ
    byte roundKey[N_ROUND][16];    //����Կ
    byte cipherText[17];    //����
    byte SBox[16][16];    //    S��
    byte InvSBox[16][16];    //    ��S��

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
    //Base64��������
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