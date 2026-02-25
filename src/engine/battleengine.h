#ifndef ENGINE_BATTLEENGINE_H
#define ENGINE_BATTLEENGINE_H

#include <QList>
#include <QString>
#include <QStringList>

struct Battler
{
    QString name;
    bool enemy;
    int hp;
    int mp;
    int attack;
    int defense;
    int speed;

    bool isAlive() const { return hp > 0; }
};

class TurnBattleEngine
{
public:
    enum BattleResult
    {
        RESULT_RUNNING = 0,
        RESULT_PARTY_WIN,
        RESULT_PARTY_LOSE
    };

    void startBattle(const QList<Battler> &party, const QList<Battler> &enemies);
    BattleResult runRound();
    BattleResult result() const;
    QStringList flushLog();

private:
    int pickRandomTargetIndex(bool chooseEnemy) const;
    int computeDamage(const Battler &attacker, const Battler &target) const;

    QList<Battler> battlers;
    QStringList roundLog;
};

#endif
