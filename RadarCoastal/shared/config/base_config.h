#ifndef BASE_CONFIG_H
#define BASE_CONFIG_H

#include <QString>

class BaseConfig
{
public:
    virtual void setup(const QString path) = 0;
};

#endif // BASE_CONFIG_H
