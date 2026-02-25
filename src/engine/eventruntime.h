#ifndef ENGINE_EVENTRUNTIME_H
#define ENGINE_EVENTRUNTIME_H

#include <QMap>
#include <QString>
#include <QStringList>

#include "battleengine.h"
#include "gamedatabase.h"
#include "gamestate.h"

struct EventCommand
{
    enum Opcode
    {
        OP_NOP = 0,
        OP_DISPLAY_TEXT,
        OP_SET_SWITCH,
        OP_SET_VARIABLE,
        OP_ADD_VARIABLE,
        OP_IF_SWITCH_GOTO,
        OP_IF_VARIABLE_GOTO,
        OP_GOTO,
        OP_CALL_COMMON_EVENT,
        OP_START_BATTLE,
        OP_END
    };

    Opcode op;
    int a;
    int b;
    int c;
    QString text;

    static EventCommand display(const QString &message);
    static EventCommand setSwitch(int switchId, bool value);
    static EventCommand setVariable(int variableId, int value);
    static EventCommand addVariable(int variableId, int delta);
    static EventCommand ifSwitchGoto(int switchId, bool expectedValue, int jumpIndex);
    static EventCommand ifVariableGoto(int variableId, int comparison, int rhsValue, bool rhsFromVariable, int jumpIndex);
    static EventCommand jumpTo(int jumpIndex);
    static EventCommand callCommonEvent(int commonEventId);
    static EventCommand startBattle(int enemyId);
    static EventCommand end();
};

class EventRuntime
{
public:
    EventRuntime(GameDatabase *database, GameState *state, TurnBattleEngine *battleEngine);

    void registerMapEvent(int id, const QList<EventCommand> &script);
    void registerCommonEvent(int id, const QList<EventCommand> &script);

    bool runMapEvent(int id);
    QStringList flushLog();

private:
    bool runScript(const QList<EventCommand> &script, int depth);
    bool runCommand(const EventCommand &command, int &pc, int scriptSize);
    QList<Battler> buildParty() const;
    QList<Battler> buildEnemyGroup(int enemyId) const;

    GameDatabase *db;
    GameState *gameState;
    TurnBattleEngine *battle;
    QMap<int, QList<EventCommand> > mapEvents;
    QMap<int, QList<EventCommand> > commonEvents;
    QStringList runtimeLog;
};

#endif
