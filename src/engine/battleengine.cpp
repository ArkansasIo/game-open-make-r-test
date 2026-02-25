#include "battleengine.h"

#include <QtGlobal>

void TurnBattleEngine::startBattle(const QList<Battler> &party, const QList<Battler> &enemies)
{
    battlers.clear();
    roundLog.clear();

    battlers.append(party);
    battlers.append(enemies);

    roundLog << "Battle started.";
}

TurnBattleEngine::BattleResult TurnBattleEngine::runRound()
{
    if (result() != RESULT_RUNNING)
        return result();

    int i;
    for (i = 0; i < battlers.size(); ++i)
    {
        Battler &attacker = battlers[i];
        if (!attacker.isAlive())
            continue;

        int targetIndex = pickRandomTargetIndex(!attacker.enemy);
        if (targetIndex < 0)
            continue;

        Battler &target = battlers[targetIndex];
        int damage = computeDamage(attacker, target);
        target.hp -= damage;
        if (target.hp < 0)
            target.hp = 0;

        roundLog << QString("%1 hits %2 for %3 damage.")
                        .arg(attacker.name)
                        .arg(target.name)
                        .arg(damage);

        if (!target.isAlive())
            roundLog << QString("%1 is defeated.").arg(target.name);

        if (result() != RESULT_RUNNING)
            break;
    }

    return result();
}

TurnBattleEngine::BattleResult TurnBattleEngine::result() const
{
    bool anyPartyAlive = false;
    bool anyEnemyAlive = false;

    int i;
    for (i = 0; i < battlers.size(); ++i)
    {
        if (!battlers[i].isAlive())
            continue;

        if (battlers[i].enemy)
            anyEnemyAlive = true;
        else
            anyPartyAlive = true;
    }

    if (anyPartyAlive && anyEnemyAlive)
        return RESULT_RUNNING;
    if (anyPartyAlive)
        return RESULT_PARTY_WIN;
    return RESULT_PARTY_LOSE;
}

QStringList TurnBattleEngine::flushLog()
{
    QStringList out = roundLog;
    roundLog.clear();
    return out;
}

int TurnBattleEngine::pickRandomTargetIndex(bool chooseEnemy) const
{
    QList<int> candidates;

    int i;
    for (i = 0; i < battlers.size(); ++i)
    {
        if (!battlers[i].isAlive())
            continue;
        if (battlers[i].enemy == chooseEnemy)
            candidates.append(i);
    }

    if (candidates.isEmpty())
        return -1;

    return candidates[qrand() % candidates.size()];
}

int TurnBattleEngine::computeDamage(const Battler &attacker, const Battler &target) const
{
    int damage = attacker.attack - (target.defense / 2);
    if (damage < 1)
        damage = 1;
    return damage;
}
