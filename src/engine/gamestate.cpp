#include "gamestate.h"

bool GameState::getSwitch(int id) const
{
    if (!switches.contains(id))
        return false;
    return switches[id];
}

void GameState::setSwitch(int id, bool value)
{
    switches[id] = value;
}

int GameState::getVariable(int id) const
{
    if (!variables.contains(id))
        return 0;
    return variables[id];
}

void GameState::setVariable(int id, int value)
{
    variables[id] = value;
}

void GameState::addVariable(int id, int delta)
{
    variables[id] = getVariable(id) + delta;
}

void GameState::reserveCommonEvent(int id)
{
    commonEventQueue.append(id);
}

int GameState::popReservedCommonEvent()
{
    if (commonEventQueue.isEmpty())
        return 0;

    int id = commonEventQueue.first();
    commonEventQueue.removeFirst();
    return id;
}

bool GameState::hasReservedCommonEvents() const
{
    return !commonEventQueue.isEmpty();
}
