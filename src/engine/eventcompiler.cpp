#include "eventcompiler.h"

#include <QtXml/QDomDocument>

bool OpenRpgEventCompiler::compileMapEventXml(const QString &xml, QList<EventCommand> *outScript, QStringList *warnings) const
{
    QDomElement root;
    if (!parseRoot(xml, &root, warnings))
        return false;

    outScript->clear();
    if (!compileEventNode(root, outScript, warnings))
        return false;
    outScript->append(EventCommand::end());
    return true;
}

bool OpenRpgEventCompiler::compileCommonEventXml(const QString &xml, QList<EventCommand> *outScript, QStringList *warnings) const
{
    return compileMapEventXml(xml, outScript, warnings);
}

bool OpenRpgEventCompiler::compileEventNode(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    QString type = eventNode.attribute("type");

    if (type == "topLevelEvent")
        return compileTopLevel(eventNode, script, warnings);
    if (type == "displayMessage")
        return compileDisplayMessage(eventNode, script);
    if (type == "switchOperation")
        return compileSwitchOperation(eventNode, script, warnings);
    if (type == "variableOperation")
        return compileVariableOperation(eventNode, script, warnings);
    if (type == "callCommonEvent")
        return compileCallCommonEvent(eventNode, script, warnings);
    if (type == "enemyEncounter")
        return compileEnemyEncounter(eventNode, script, warnings);
    if (type == "conditionalBranch")
        return compileConditionalBranch(eventNode, script, warnings);
    if (type == "endEventProcessing")
    {
        script->append(EventCommand::end());
        return true;
    }

    warnings->append(QString("Unsupported event type '%1'; skipped.").arg(type));
    return true;
}

bool OpenRpgEventCompiler::compileTopLevel(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    QDomElement child = eventNode.firstChildElement("event");
    while (!child.isNull())
    {
        if (!compileEventNode(child, script, warnings))
            return false;
        child = child.nextSiblingElement("event");
    }
    return true;
}

bool OpenRpgEventCompiler::compileDisplayMessage(const QDomElement &eventNode, QList<EventCommand> *script) const
{
    QStringList lines;
    QDomElement lineNode = eventNode.firstChildElement("line");
    while (!lineNode.isNull())
    {
        lines.append(lineNode.text());
        lineNode = lineNode.nextSiblingElement("line");
    }
    script->append(EventCommand::display(lines.join("\n")));
    return true;
}

bool OpenRpgEventCompiler::compileSwitchOperation(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    QString target = eventNode.attribute("switchToChange");
    QString op = eventNode.attribute("operation");
    int startId;
    int endId;

    if (target == "single")
    {
        if (!parseIntAttr(eventNode, "switchID", &startId))
            return false;
        endId = startId;
    }
    else if (target == "range")
    {
        if (!parseIntAttr(eventNode, "startSwitchID", &startId))
            return false;
        if (!parseIntAttr(eventNode, "endSwitchID", &endId))
            return false;
    }
    else if (target == "referenced")
    {
        warnings->append("SwitchOperation with referenced target is not supported yet.");
        return true;
    }
    else
        return false;

    if (startId > endId)
    {
        int tmp = startId;
        startId = endId;
        endId = tmp;
    }

    int id;
    for (id = startId; id <= endId; ++id)
    {
        if (op == "turnOn")
            script->append(EventCommand::setSwitch(id, true));
        else if (op == "turnOff")
            script->append(EventCommand::setSwitch(id, false));
        else if (op == "toggle")
        {
            int trueJump = script->size() + 3;
            int endJump = script->size() + 4;
            script->append(EventCommand::ifSwitchGoto(id, true, trueJump));
            script->append(EventCommand::setSwitch(id, true));
            script->append(EventCommand::jumpTo(endJump));
            script->append(EventCommand::setSwitch(id, false));
        }
        else
            return false;
    }

    return true;
}

bool OpenRpgEventCompiler::compileVariableOperation(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    QString operation = eventNode.attribute("operation");
    QString target = eventNode.attribute("variableToChange");
    QString operand = eventNode.attribute("operand");
    int startId;
    int endId;
    int value;
    int i;

    if (target == "single" || target == "reference")
    {
        if (!parseIntAttr(eventNode, "variableID", &startId))
            return false;
        endId = startId;
    }
    else if (target == "range")
    {
        if (!parseIntAttr(eventNode, "startVariableID", &startId))
            return false;
        if (!parseIntAttr(eventNode, "endVariableID", &endId))
            return false;
    }
    else
        return false;

    if (target == "reference")
    {
        warnings->append("VariableOperation with referenced destination is not supported yet.");
        return true;
    }

    if (startId > endId)
    {
        int tmp = startId;
        startId = endId;
        endId = tmp;
    }

    if (operand == "value")
    {
        if (!parseIntAttr(eventNode, "value", &value))
            return false;
        for (i = startId; i <= endId; ++i)
        {
            if (operation == "set")
                script->append(EventCommand::setVariable(i, value));
            else if (operation == "add")
                script->append(EventCommand::addVariable(i, value));
            else if (operation == "subtract")
                script->append(EventCommand::addVariable(i, -value));
            else
                warnings->append(QString("VariableOperation '%1' with value operand is not supported yet.").arg(operation));
        }
        return true;
    }

    if (operand == "variable")
    {
        warnings->append("VariableOperation with variable operand is not supported yet.");
        return true;
    }

    warnings->append(QString("VariableOperation operand '%1' is not supported yet.").arg(operand));
    return true;
}

bool OpenRpgEventCompiler::compileCallCommonEvent(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    int eventId;
    bool refByVariable = false;
    if (!parseIntAttr(eventNode, "eventID", &eventId))
        return false;
    parseBoolAttr(eventNode, "eventReferencedInVariable", &refByVariable);
    if (refByVariable)
    {
        warnings->append("CallCommonEvent using variable lookup is not supported yet.");
        return true;
    }

    script->append(EventCommand::callCommonEvent(eventId));
    return true;
}

bool OpenRpgEventCompiler::compileEnemyEncounter(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    int enemyGroupId;
    bool refByVariable = false;
    if (!parseIntAttr(eventNode, "monsterGroupID", &enemyGroupId))
        return false;
    parseBoolAttr(eventNode, "monsterGroupReferencedInVariable", &refByVariable);
    if (refByVariable)
    {
        warnings->append("EnemyEncounter with variable monster group is not supported yet.");
        return true;
    }

    script->append(EventCommand::startBattle(enemyGroupId));

    // Current runtime only supports linear post-battle flow; compile victory events as continuation.
    QDomElement victoryNode = eventNode.firstChildElement("victoryEvents");
    if (!victoryNode.isNull())
    {
        QDomElement child = victoryNode.firstChildElement("event");
        while (!child.isNull())
        {
            if (!compileEventNode(child, script, warnings))
                return false;
            child = child.nextSiblingElement("event");
        }
    }
    return true;
}

bool OpenRpgEventCompiler::compileConditionalBranch(const QDomElement &eventNode, QList<EventCommand> *script, QStringList *warnings) const
{
    QDomElement conditionsNode = eventNode.firstChildElement("conditions");
    if (conditionsNode.isNull())
        return false;

    QDomElement conditionNode = conditionsNode.firstChildElement("condition");
    if (conditionNode.isNull())
        return false;

    int ifIndex = script->size();
    QString condType = conditionNode.attribute("type");

    if (condType == "switch")
    {
        int switchId;
        bool expectedOn;
        bool refByVariable = false;
        if (!parseIntAttr(conditionNode, "switchID", &switchId))
            return false;
        if (!parseBoolAttr(conditionNode, "isOn", &expectedOn))
            return false;
        parseBoolAttr(conditionNode, "switchReferencedInVariable", &refByVariable);
        if (refByVariable)
            warnings->append("Conditional switch with variable-referenced switch id is not supported.");
        script->append(EventCommand::ifSwitchGoto(switchId, expectedOn, 0));
    }
    else if (condType == "variable")
    {
        int srcVar;
        int dstValue;
        bool dstStoredInVariable = false;
        if (!parseIntAttr(conditionNode, "srcVariableID", &srcVar))
            return false;
        if (!parseIntAttr(conditionNode, "dstValue", &dstValue))
            return false;
        parseBoolAttr(conditionNode, "dstValueStoredInVariable", &dstStoredInVariable);

        script->append(EventCommand::ifVariableGoto(srcVar,
                                                    parseCompareOp(conditionNode.attribute("comparison")),
                                                    dstValue,
                                                    dstStoredInVariable,
                                                    0));
    }
    else
    {
        warnings->append(QString("Conditional type '%1' is not supported.").arg(condType));
        return true;
    }

    int gotoFalseIndex = script->size();
    script->append(EventCommand::jumpTo(0));

    int trueStart = script->size();
    QDomElement onTrue = eventNode.firstChildElement("onTrueEvents");
    QDomElement trueEvent = onTrue.firstChildElement("event");
    while (!trueEvent.isNull())
    {
        if (!compileEventNode(trueEvent, script, warnings))
            return false;
        trueEvent = trueEvent.nextSiblingElement("event");
    }

    int gotoEndIndex = script->size();
    script->append(EventCommand::jumpTo(0));

    int falseStart = script->size();
    QDomElement onFalse = eventNode.firstChildElement("onFalseEvents");
    QDomElement falseEvent = onFalse.firstChildElement("event");
    while (!falseEvent.isNull())
    {
        if (!compileEventNode(falseEvent, script, warnings))
            return false;
        falseEvent = falseEvent.nextSiblingElement("event");
    }

    int endIndex = script->size();

    if ((*script)[ifIndex].op == EventCommand::OP_IF_SWITCH_GOTO)
        (*script)[ifIndex].c = trueStart;
    else if ((*script)[ifIndex].op == EventCommand::OP_IF_VARIABLE_GOTO)
        (*script)[ifIndex].c = trueStart;

    (*script)[gotoFalseIndex].a = falseStart;
    (*script)[gotoEndIndex].a = endIndex;
    return true;
}

bool OpenRpgEventCompiler::parseRoot(const QString &xml, QDomElement *eventRoot, QStringList *warnings) const
{
    QDomDocument doc;
    QString parseError;
    int line = 0;
    int column = 0;
    if (!doc.setContent(xml, &parseError, &line, &column))
    {
        warnings->append(QString("XML parse error at %1:%2 %3").arg(line).arg(column).arg(parseError));
        return false;
    }

    QDomElement root = doc.documentElement();
    if (root.tagName() != "event")
    {
        warnings->append("Expected XML root node <event>.");
        return false;
    }

    *eventRoot = root;
    return true;
}

bool OpenRpgEventCompiler::parseIntAttr(const QDomElement &node, const QString &name, int *out) const
{
    bool ok = false;
    int value = node.attribute(name).toInt(&ok);
    if (!ok)
        return false;
    *out = value;
    return true;
}

bool OpenRpgEventCompiler::parseBoolAttr(const QDomElement &node, const QString &name, bool *out) const
{
    QString value = node.attribute(name);
    if (value == "true")
    {
        *out = true;
        return true;
    }
    if (value == "false")
    {
        *out = false;
        return true;
    }
    return false;
}

int OpenRpgEventCompiler::parseCompareOp(const QString &op) const
{
    if (op == "==") return CMP_EQ;
    if (op == "!=") return CMP_NE;
    if (op == ">") return CMP_GT;
    if (op == ">=") return CMP_GE;
    if (op == "<") return CMP_LT;
    if (op == "<=") return CMP_LE;
    return CMP_EQ;
}
