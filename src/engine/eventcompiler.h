#ifndef ENGINE_EVENTCOMPILER_H
#define ENGINE_EVENTCOMPILER_H

#include <QList>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QtXml/QDomElement>

#include "eventruntime.h"

class OpenRpgEventCompiler
{
public:
    bool compileMapEventXml(const QString &xml, QList<EventCommand> *outScript, QStringList *warnings) const;
    bool compileCommonEventXml(const QString &xml, QList<EventCommand> *outScript, QStringList *warnings) const;

private:
    enum CompareOp
    {
        CMP_EQ = 0,
        CMP_NE,
        CMP_GT,
        CMP_GE,
        CMP_LT,
        CMP_LE
    };

    bool compileEventNode(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileTopLevel(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileDisplayMessage(const QDomElement &eventNode, QList<EventCommand> *script) const;
    bool compileSwitchOperation(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileVariableOperation(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileCallCommonEvent(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileEnemyEncounter(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;
    bool compileConditionalBranch(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const;

    bool parseRoot(const QString &xml, QDomElement *eventRoot, QStringList *warnings) const;
    bool parseIntAttr(const QDomElement &node, const QString &name, int *out) const;
    bool parseBoolAttr(const QDomElement &node, const QString &name, bool *out) const;
    int parseCompareOp(const QString &op) const;
};

#endif
