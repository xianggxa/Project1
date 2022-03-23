#include "httpparsing.h"

#include <string.h>
#include <vector>

//Э�����
void HttpParsing::tryDecode(const std::string& buf) {
    this->parseInternal(buf.c_str(), buf.size());
}
HttpParsing::HttpParsing(){}

HttpParsing::~HttpParsing(){}

void HttpParsing::init(int connfd)
{
    _reclen = recv(connfd, buf, BUF_SIZE, 0);
    printf("http %d\n", _reclen);
    
    parseInternal(buf, _reclen);
}

//����������
void  HttpParsing::parseInternal(const char* buf, int size) {

    StringBuffer method;
    StringBuffer url;

    StringBuffer requestParamKey;
    StringBuffer requestParamValue;

    StringBuffer protocol;
    StringBuffer version;

    StringBuffer headerKey;
    StringBuffer headerValue;

    int bodyLength = 0;
    _nextPos = 0;
    _decodeState = HttpRequestDecodeState::START;
    //��ʼ��
    char* p0 = const_cast<char*>(buf + _nextPos);//ȥ��const����

    while (_decodeState != HttpRequestDecodeState::INVALID
        && _decodeState != HttpRequestDecodeState::INVALID_METHOD
        && _decodeState != HttpRequestDecodeState::INVALID_URI
        && _decodeState != HttpRequestDecodeState::INVALID_VERSION
        && _decodeState != HttpRequestDecodeState::INVALID_HEADER
        && _decodeState != HttpRequestDecodeState::COMPLETE
        && _nextPos < size) {


        char ch = *p0;//��ǰ���ַ�
        printf("%c", ch);
        char* p = p0++;//ָ��ƫ��
        int scanPos = _nextPos++;//��һ��ָ������ƫ��
        switch (_decodeState) {
        case HttpRequestDecodeState::START: {
            //�ո� ���� �س�������
            if (ch == CR || ch == LF || isblank(ch)) {
                //do nothing
            }
            else if (isupper(ch)) {//�ж��ǲ��Ǵ�д�ַ������ǵĻ�����Ч��
                method.begin = p;//��������ʼ��
                _decodeState = HttpRequestDecodeState::METHOD;//���������һ���ַ�����ʼ��������
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID;
            }
            break;
        }
        case HttpRequestDecodeState::METHOD: {
            //������Ҫ��д��ĸ����д��ĸ�ͼ���
            if (isupper(ch)) {
                //do nothing
            }
            else if (isblank(ch)) {//�ո�˵������������������һ����ʼ����URI
                method.end = p;//������������
                _method = method;
                _decodeState = HttpRequestDecodeState::BEFORE_URI;
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID_METHOD;//�����������Ч�����󷽷�
            }
            break;
        }
        case HttpRequestDecodeState::BEFORE_URI: {
            //��������ǰ�Ĵ�����Ҫ'/'��ͷ
            if (ch == '/') {
                url.begin = p;
                _decodeState = HttpRequestDecodeState::IN_URI;//��һ�����ǿ�ʼ��������
            }
            else if (isblank(ch)) {
                //do nothing
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID;//��Ч��
            }
            break;
        }
        case HttpRequestDecodeState::IN_URI: {
            //��ʼ��������·�����ַ���
            if (ch == '?') {//תΪ���������keyֵ
                url.end = p;
                _url = url;
                _decodeState = HttpRequestDecodeState::BEFORE_URI_PARAM_KEY;
            }
            else if (isblank(ch)) {//�����ո�����·��������ɣ���ʼ����Э��
                url.end = p;
                _url = url;
                _decodeState = HttpRequestDecodeState::BEFORE_PROTOCOL;
            }
            else {
                //do nothing
            }
            break;
        }
        case HttpRequestDecodeState::BEFORE_URI_PARAM_KEY: {
            if (isblank(ch) || ch == LF || ch == CR) {
                _decodeState = HttpRequestDecodeState::INVALID_URI;
            }
            else {
                requestParamKey.begin = p;
                _decodeState = HttpRequestDecodeState::URI_PARAM_KEY;
            }
            break;
        }
        case HttpRequestDecodeState::URI_PARAM_KEY: {
            if (ch == '=') {
                requestParamKey.end = p;
                _decodeState = HttpRequestDecodeState::BEFORE_URI_PARAM_VALUE;//��ʼ��������ֵ
            }
            else if (isblank(ch)) {
                _decodeState = HttpRequestDecodeState::INVALID_URI;//��Ч���������
            }
            else {
                //do nothing
            }
            break;
        }
        case HttpRequestDecodeState::BEFORE_URI_PARAM_VALUE: {
            if (isblank(ch) || ch == LF || ch == CR) {
                _decodeState = HttpRequestDecodeState::INVALID_URI;
            }
            else {
                requestParamValue.begin = p;
                _decodeState = HttpRequestDecodeState::URI_PARAM_VALUE;
            }
            break;
        }
        case HttpRequestDecodeState::URI_PARAM_VALUE: {
            if (ch == '&') {
                requestParamValue.end = p;
                //�ջ�һ���������
                _requestParams.insert({ requestParamKey, requestParamValue });
                _decodeState = HttpRequestDecodeState::BEFORE_URI_PARAM_KEY;
            }
            else if (isblank(ch)) {
                requestParamValue.end = p;
                //�ո�Ҳ�ջ�һ���������
                _requestParams.insert({ requestParamKey, requestParamValue });
                _decodeState = HttpRequestDecodeState::BEFORE_PROTOCOL;
            }
            else {
                //do nothing
            }
            break;
        }
        case HttpRequestDecodeState::BEFORE_PROTOCOL: {
            if (isblank(ch)) {
                //do nothing
            }
            else {
                protocol.begin = p;
                _decodeState = HttpRequestDecodeState::PROTOCOL;
            }
            break;
        }
        case HttpRequestDecodeState::PROTOCOL: {
            //��������Э��
            if (ch == '/') {
                protocol.end = p;
                _protocol = protocol;
                _decodeState = HttpRequestDecodeState::BEFORE_VERSION;
            }
            else {
                //do nothing
            }
            break;
        }
        case HttpRequestDecodeState::BEFORE_VERSION: {
            if (isdigit(ch)) {
                version.begin = p;
                _decodeState = HttpRequestDecodeState::VERSION;
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID_VERSION;
            }
            break;
        }
        case HttpRequestDecodeState::VERSION: {
            //Э�����������������ֻ���. �Ͳ���
            if (ch == CR) {
                version.end = p;
                _version = version;
                _decodeState = HttpRequestDecodeState::WHEN_CR;
            }
            else if (ch == '.') {
                //�����汾�ָ�
                _decodeState = HttpRequestDecodeState::VERSION_SPLIT;
            }
            else if (isdigit(ch)) {
                //do nothing
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID_VERSION;//���ܲ�������
            }
            break;
        }
        case HttpRequestDecodeState::VERSION_SPLIT: {
            //�����汾�ָ�����ַ����������֣�����������Ǵ���
            if (isdigit(ch)) {
                _decodeState = HttpRequestDecodeState::VERSION;
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID_VERSION;
            }
            break;
        }
        case HttpRequestDecodeState::HEADER_KEY: {
            //ð��ǰ������пո�
            if (isblank(ch)) {
                headerKey.end = p;
                _decodeState = HttpRequestDecodeState::HEADER_BEFORE_COLON;//ð��֮ǰ��״̬
            }
            else if (ch == ':') {
                headerKey.end = p;
                _decodeState = HttpRequestDecodeState::HEADER_AFTER_COLON;//ð��֮���״̬
            }
            else {
                //do nothing
            }
            break;
        }
        case HttpRequestDecodeState::HEADER_BEFORE_COLON: {
            if (isblank(ch)) {
                //do nothing
            }
            else if (ch == ':') {
                _decodeState = HttpRequestDecodeState::HEADER_AFTER_COLON;
            }
            else {
                //ð��֮ǰ��״̬�����ǿո�֮��������ַ�
                _decodeState = HttpRequestDecodeState::INVALID_HEADER;
            }
            break;
        }
        case HttpRequestDecodeState::HEADER_AFTER_COLON: {
            if (isblank(ch)) {//ֵ֮ǰ�����ո���������
                //do nothing
            }
            else {
                headerValue.begin = p;
                _decodeState = HttpRequestDecodeState::HEADER_VALUE;//��ʼ����ֵ
            }
            break;
        }
        case HttpRequestDecodeState::HEADER_VALUE: {
            if (ch == CR) {
                headerValue.end = p;
                _headers.insert({ headerKey, headerValue });
                _decodeState = HttpRequestDecodeState::WHEN_CR;
            }
            break;
        }
        case HttpRequestDecodeState::WHEN_CR: {
            if (ch == LF) {
                //����ǻس����ɻ�����һ��
                _decodeState = HttpRequestDecodeState::CR_LF;
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID;
            }
            break;
        }
        case HttpRequestDecodeState::CR_LF: {
            if (ch == CR) {
                //�����CR_LF״̬֮����CR����ô�����е������ζ����
                _decodeState = HttpRequestDecodeState::CR_LF_CR;
            }
            else if (isblank(ch)) {
                _decodeState = HttpRequestDecodeState::INVALID;
            }
            else {
                //������ǣ���ô�Ϳ�������һ������ͷ�ˣ��ǾͿ�ʼ��������ͷ
                headerKey.begin = p;
                _decodeState = HttpRequestDecodeState::HEADER_KEY;
            }
            break;
        }
        case HttpRequestDecodeState::CR_LF_CR: {
            if (ch == LF) {
                //�����\r����\n ��ô�ж��ǲ�����Ҫ����������
                if (_headers.count("Content-Length") > 0) {
                    bodyLength = atoi(_headers["Content-Length"].c_str());
                    if (bodyLength > 0) {
                        _decodeState = HttpRequestDecodeState::BODY;//����������
                    }
                    else {
                        _decodeState = HttpRequestDecodeState::COMPLETE;//�����
                    }
                }
                else {
                    if (scanPos < size) {
                        bodyLength = size - scanPos;
                        _decodeState = HttpRequestDecodeState::BODY;//����������
                    }
                    else {
                        _decodeState = HttpRequestDecodeState::COMPLETE;
                    }
                }
            }
            else {
                _decodeState = HttpRequestDecodeState::INVALID_HEADER;
            }
            break;
        }
        case HttpRequestDecodeState::BODY: {
            //����������
            _body.assign(p, bodyLength);
            _decodeState = HttpRequestDecodeState::COMPLETE;
            break;
        }
        default:
            break;
        }
    }
}

const std::string& HttpParsing::getMethod() const {
    return _method;
}

const std::string& HttpParsing::getUrl() const {
    return _url;
}

const std::map<std::string, std::string>& HttpParsing::getRequestParams() const {
    return _requestParams;
}

const std::string& HttpParsing::getProtocol() const {
    return _protocol;
}

const std::string& HttpParsing::getVersion() const {
    return _version;
}

const std::map<std::string, std::string>& HttpParsing::getHeaders() const {
    return _headers;
}

const std::string& HttpParsing::getBody() const {
    return _body;
}

const int& HttpParsing::getLen() const
{
    return _reclen;
}

