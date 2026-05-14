#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <StandardDefines.h>
#include "auth/ISecurityConfig.h"

/* @Configuration */
class MyConfig : public ISecurityConfig {
    Public MyConfig() = default;
    Public ~MyConfig() = default;

    Public Void Configure(IEndpointSecurityRuleManagerPtr endpointSecurityRuleManager) override {
        //PLACEHOLDER FOR SECURITY CONFIG CONFIGURATION
    }
};

#endif // MYCONFIG_H