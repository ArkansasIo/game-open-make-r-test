#include "eventruntime.h"

#include <cstdio>
#include <QtGlobal>

EventCommand EventCommand::display(const QString &message)
{
    EventCommand cmd;
    cmd.op = OP_DISPLAY_TEXT;
    cmd.a = cmd.b = cmd.c = 0;
    cmd.text = message;
    return cmd;
}

EventCommand EventCommand::setSwitch(int switchId, bool value)
{
    EventCommand cmd;
    cmd.op = OP_SET_SWITCH;
    cmd.a = switchId;
    cmd.b = value ? 1 : 0;
    cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::setVariable(int variableId, int value)
{
    EventCommand cmd;
    cmd.op = OP_SET_VARIABLE;
    cmd.a = variableId;
    cmd.b = value;
    cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::addVariable(int variableId, int delta)
{
    EventCommand cmd;
    cmd.op = OP_ADD_VARIABLE;
    cmd.a = variableId;
    cmd.b = delta;
    cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::ifSwitchGoto(int switchId, bool expectedValue, int jumpIndex)
{
    EventCommand cmd;
    cmd.op = OP_IF_SWITCH_GOTO;
    cmd.a = switchId;
    cmd.b = expectedValue ? 1 : 0;
    cmd.c = jumpIndex;
    return cmd;
}

EventCommand EventCommand::ifVariableGoto(int variableId, int comparison, int rhsValue, bool rhsFromVariable, int jumpIndex)
{
    EventCommand cmd;
    cmd.op = OP_IF_VARIABLE_GOTO;
    cmd.a = variableId;
    cmd.b = rhsValue;
    cmd.c = jumpIndex;
    cmd.text = QString("%1:%2").arg(comparison).arg(rhsFromVariable ? 1 : 0);
    return cmd;
}

EventCommand EventCommand::jumpTo(int jumpIndex)
{
    EventCommand cmd;
    cmd.op = OP_GOTO;
    cmd.a = jumpIndex;
    cmd.b = cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::callCommonEvent(int commonEventId)
{
    EventCommand cmd;
    cmd.op = OP_CALL_COMMON_EVENT;
    cmd.a = commonEventId;
    cmd.b = cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::startBattle(int enemyId)
{
    EventCommand cmd;
    cmd.op = OP_START_BATTLE;
    cmd.a = enemyId;
    cmd.b = cmd.c = 0;
    return cmd;
}

EventCommand EventCommand::end()
{
    EventCommand cmd;
    cmd.op = OP_END;
    cmd.a = cmd.b = cmd.c = 0;
    return cmd;
}

EventRuntime::EventRuntime(GameDatabase *database, GameState *state, TurnBattleEngine *battleEngine)
{
    db = database;
    gameState = state;
    battle = battleEngine;
}

void EventRuntime::registerMapEvent(int id, const QList<EventCommand> &script)
{
    mapEvents[id] = script;
}

void EventRuntime::registerCommonEvent(int id, const QList<EventCommand> &script)
{
    commonEvents[id] = script;
}

bool EventRuntime::runMapEvent(int id)
{
    if (!mapEvents.contains(id))
    {
        runtimeLog << QString("Map event %1 not found.").arg(id);
        return false;
    }

    runtimeLog << QString("Running map event %1.").arg(id);
    return runScript(mapEvents[id], 0);
}

QStringList EventRuntime::flushLog()
{
    QStringList out = runtimeLog;
    runtimeLog.clear();
    return out;
}

bool EventRuntime::runScript(const QList<EventCommand> &script, int depth)
{
    if (depth > 20)
    {
        runtimeLog << "Event recursion depth exceeded.";
        return false;
    }

    int pc = 0;
    while (pc >= 0 && pc < script.size())
    {
        if (!runCommand(script[pc], pc, script.size()))
            return false;
        ++pc;
    }

    while (gameState->hasReservedCommonEvents())
    {
        int eventId = gameState->popReservedCommonEvent();
        if (!commonEvents.contains(eventId))
        {
            runtimeLog << QString("Common event %1 not found.").arg(eventId);
            return false;
        }

        runtimeLog << QString("Running common event %1.").arg(eventId);
        if (!runScript(commonEvents[eventId], depth + 1))
            return false;
    }

    return true;
}

bool EventRuntime::runCommand(const EventCommand &command, int &pc, int scriptSize)
{
    int parsedComparison;
    int rhsFromVariable;
    bool conditionMet;
    int lhs;
    int rhs;

    switch (command.op)
    {
        case EventCommand::OP_NOP:
            return true;

        case EventCommand::OP_DISPLAY_TEXT:
            runtimeLog << QString("Text: %1").arg(command.text);
            return true;

        case EventCommand::OP_SET_SWITCH:
            gameState->setSwitch(command.a, command.b != 0);
            runtimeLog << QString("Switch[%1] = %2").arg(command.a).arg(command.b != 0);
            return true;

        case EventCommand::OP_SET_VARIABLE:
            gameState->setVariable(command.a, command.b);
            runtimeLog << QString("Variable[%1] = %2").arg(command.a).arg(command.b);
            return true;

        case EventCommand::OP_ADD_VARIABLE:
            gameState->addVariable(command.a, command.b);
            runtimeLog << QString("Variable[%1] += %2 -> %3")
                              .arg(command.a)
                              .arg(command.b)
                              .arg(gameState->getVariable(command.a));
            return true;

        case EventCommand::OP_IF_SWITCH_GOTO:
            if (gameState->getSwitch(command.a) == (command.b != 0))
            {
                if (command.c < 0 || command.c >= scriptSize)
                {
                    runtimeLog << "Conditional jump target out of range.";
                    return false;
                }
                pc = command.c - 1;
            }
            return true;

        case EventCommand::OP_IF_VARIABLE_GOTO:
            parsedComparison = 0;
            rhsFromVariable = 0;
            if (sscanf(command.text.toUtf8().data(), "%d:%d", &parsedComparison, &rhsFromVariable) != 2)
            {
                runtimeLog << "Invalid variable comparison command encoding.";
                return false;
            }

            lhs = gameState->getVariable(command.a);
            rhs = (rhsFromVariable != 0) ? gameState->getVariable(command.b) : command.b;

            conditionMet = false;
            if (parsedComparison == 0) conditionMet = (lhs == rhs);
            else if (parsedComparison == 1) conditionMet = (lhs != rhs);
            else if (parsedComparison == 2) conditionMet = (lhs > rhs);
            else if (parsedComparison == 3) conditionMet = (lhs >= rhs);
            else if (parsedComparison == 4) conditionMet = (lhs < rhs);
            else if (parsedComparison == 5) conditionMet = (lhs <= rhs);
            else
            {
                runtimeLog << "Unknown variable comparison operator.";
                return false;
            }

            if (conditionMet)
            {
                if (command.c < 0 || command.c >= scriptSize)
                {
                    runtimeLog << "Conditional variable jump target out of range.";
                    return false;
                }
                pc = command.c - 1;
            }
            return true;

        case EventCommand::OP_GOTO:
            if (command.a < 0 || command.a >= scriptSize)
            {
                runtimeLog << "Goto target out of range.";
                return false;
            }
            pc = command.a - 1;
            return true;

        case EventCommand::OP_CALL_COMMON_EVENT:
            gameState->reserveCommonEvent(command.a);
            runtimeLog << QString("Queued common event %1.").arg(command.a);
            return true;

        case EventCommand::OP_START_BATTLE:
        {
            QList<Battler> party = buildParty();
            QList<Battler> enemies = buildEnemyGroup(command.a);
            if (party.isEmpty() || enemies.isEmpty())
            {
                runtimeLog << "Cannot start battle, invalid party or enemy group.";
                return false;
            }

            battle->startBattle(party, enemies);
            while (battle->result() == TurnBattleEngine::RESULT_RUNNING)
                battle->runRound();

            runtimeLog << battle->flushLog();
            if (battle->result() == TurnBattleEngine::RESULT_PARTY_WIN)
                runtimeLog << "Battle result: WIN";
            else
                runtimeLog << "Battle result: LOSE";
            return true;
        }

        case EventCommand::OP_END:
            pc = scriptSize;
            return true;
    }

    runtimeLog << "Unknown event command.";
    return false;
}

QList<Battler> EventRuntime::buildParty() const
{
    QList<Battler> party;

    const EngineActor *actor = db->getActor(1);
    if (actor == NULL)
        return party;

    const EngineActorClass *actorClass = db->getClass(actor->classId);
    if (actorClass == NULL)
        return party;

    Battler battler;
    battler.name = actor->name;
    battler.enemy = false;
    battler.hp = actorClass->baseHP + actor->level * 5;
    battler.mp = actorClass->baseMP + actor->level * 2;
    battler.attack = actorClass->baseAttack + actor->level;
    battler.defense = actorClass->baseDefense + actor->level;
    battler.speed = actorClass->baseSpeed + actor->level;

    party << battler;
    return party;
}

QList<Battler> EventRuntime::buildEnemyGroup(int enemyId) const
{
    QList<Battler> enemies;
    const EngineEnemy *enemy = db->getEnemy(enemyId);
    if (enemy == NULL)
        return enemies;

    Battler battler;
    battler.name = enemy->name;
    battler.enemy = true;
    battler.hp = enemy->maxHP;
    battler.mp = enemy->maxMP;
    battler.attack = enemy->attack;
    battler.defense = enemy->defense;
    battler.speed = enemy->speed;

    enemies << battler;
    return enemies;
}
