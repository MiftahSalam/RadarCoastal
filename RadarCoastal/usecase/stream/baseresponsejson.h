#ifndef BASERESPONSEJSON_H
#define BASERESPONSEJSON_H

#include <QJsonObject>
#include <QJsonValue>

template <typename T>
class BaseResponseJson
{
public:
    BaseResponseJson(int code = 0, QString status = "ok", T* data = nullptr)
    {
        m_code = code;
        m_message = status;
        m_data = data;
    }
    QJsonValue build() const
    {
        QJsonObject obj;

        obj["code"] = m_code;
        obj["message"] = m_message;
        if (m_data) obj["data"] = *m_data;
        else obj["data"] = QJsonValue::Null;

        return QJsonObject(obj);
    }

protected:
    int m_code;
    QString m_message;
    T* m_data;
};

#endif // BASERESPONSEJSON_H
