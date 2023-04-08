#pragma once
#ifndef CLIENT_REQUEST
#define CLIENT_REQUEST

#include <sys/socket.h>
#include <map>
#include <string>

#define CR '\r'
#define LF '\n'
#define BUF_SIZE 2048
struct StringBuffer {
    char* begin = NULL;//�ַ�����ʼλ��
    char* end = NULL;//�ַ�������λ��

    operator std::string() const {
        return std::string(begin, end);
    }
};

/**
 * Http�����е�״̬
 */
enum class HttpRequestDecodeState {
    INVALID,//��Ч
    INVALID_METHOD,//��Ч���󷽷�
    INVALID_URI,//��Ч������·��
    INVALID_VERSION,//��Ч��Э��汾��
    INVALID_HEADER,//��Ч����ͷ


    START,//�����п�ʼ
    METHOD,//���󷽷�

    BEFORE_URI,//��������ǰ��״̬����Ҫ'/'��ͷ
    IN_URI,//url����
    BEFORE_URI_PARAM_KEY,//URL���������֮ǰ
    URI_PARAM_KEY,//URL���������
    BEFORE_URI_PARAM_VALUE,//URL�Ĳ���ֵ֮ǰ
    URI_PARAM_VALUE,//URL�������ֵ

    BEFORE_PROTOCOL,//Э�����֮ǰ
    PROTOCOL,//Э��

    BEFORE_VERSION,//�汾��ʼǰ
    VERSION_SPLIT,//�汾�ָ��� '.'
    VERSION,//�汾

    HEADER_KEY,

    HEADER_BEFORE_COLON,//ð��֮ǰ
    HEADER_AFTER_COLON,//ð��
    HEADER_VALUE,//ֵ

    WHEN_CR,//����һ���س�֮��

    CR_LF,//�س�����

    CR_LF_CR,//�س�����֮���״̬


    BODY,//������

    COMPLETE,//���
};

/**
 * http��������
 */
class HttpParsing
{    /**
     * ����httpЭ��
     * @param buf
     * @return
     */
public:
    HttpParsing();
    ~HttpParsing();
    void init(int connfd);
    
    void tryDecode(const std::string& buf);

    const std::string& getMethod() const;

    const std::string& getUrl() const;

    const std::map<std::string, std::string>& getRequestParams() const;

    const std::string& getProtocol() const;

    const std::string& getVersion() const;

    const std::map<std::string, std::string>& getHeaders() const;

    const std::string& getBody() const;

    const int& getLen() const;
    const bool getError()const;

private:

    void parseInternal(const char* buf, int size);

private:
    char buf[BUF_SIZE];//���ջ�����

    std::string content;

    int _reclen;

    bool iserror;
	
    std::string _method;//���󷽷�

    std::string _url;//����·��[�������������]

    std::map<std::string, std::string> _requestParams;//�������

    std::string _protocol;//Э��
    std::string _version;//�汾

    std::map<std::string, std::string> _headers;//���е�����ͷ

    std::string _body;//������

    int _nextPos = 0;//��һ��λ�õ�

    HttpRequestDecodeState _decodeState = HttpRequestDecodeState::START;//����״̬
};


#endif // !CLIENT_REQUEST
