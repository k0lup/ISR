#ifndef METATYPES_H
#define METATYPES_H
#include <QMetaType>

#include "Services/Programs/programtypes.h"
#include "Services/DirectiveExecutor/executiontypes.h"
#include "Services/GUI/guitypes.h"
#include "Services/PRIS/pristypes.h"
#include "Services/RequestIDGenerator/requestIDGenerator.h"

Q_DECLARE_METATYPE(SectionStruct)
Q_DECLARE_METATYPE(ExecutionSnapshot)
Q_DECLARE_METATYPE(ExecuteMode)
Q_DECLARE_METATYPE(WidgetTypes)
Q_DECLARE_METATYPE(WidgetInfo)
Q_DECLARE_METATYPE(WindowResultData)
Q_DECLARE_METATYPE(PRISMessage)
Q_DECLARE_METATYPE(PRISResultData)
Q_DECLARE_METATYPE(RequestId)
Q_DECLARE_METATYPE(ExecuteParam)

#endif // METATYPES_H
