#ifndef SUBDIRECTIVE_H
#define SUBDIRECTIVE_H
#include "Services/Directives/mainoperdirecitve.h"

class SubDirective : public MainOperDirecitve
{
public:
    SubDirective(const DirectiveStruct& directive, QObject* parent = nullptr) : MainOperDirecitve(directive, parent) {}
    DirectType getTypeDirect() const override {return DirectType::SUB_DIRECTIVE;}
};

#endif // SUBDIRECTIVE_H
