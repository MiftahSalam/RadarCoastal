#ifndef APPLICATIONCONFIG_H
#define APPLICATIONCONFIG_H

#include "navigation_config.h"

class ApplicationConfig
{
public:
    ApplicationConfig();

        NavigationConfig *getNavConfig() const;

    private:
        NavigationConfig *navConfig;
};

#endif // APPLICATIONCONFIG_H
