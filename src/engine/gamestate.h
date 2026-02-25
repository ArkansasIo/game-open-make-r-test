#ifndef ENGINE_GAMESTATE_H
#define ENGINE_GAMESTATE_H

#include <QList>
#include <QMap>

class GameState
{
public:
    bool getSwitch(int id) const;
    void setSwitch(int id, bool value);

    int getVariable(int id) const;
    void setVariable(int id, int value);
    void addVariable(int id, int delta);

    void reserveCommonEvent(int id);
    int popReservedCommonEvent();
    bool hasReservedCommonEvents() const;

private:
    QMap<int, bool> switches;
    QMap<int, int> variables;
    QList<int> commonEventQueue;
};

#endif
