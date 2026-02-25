#ifndef ENGINE_GAMEDATABASE_H
#define ENGINE_GAMEDATABASE_H

#include <QList>
#include <QMap>
#include <QString>
#include <QStringList>

struct EngineSkill
{
    int id;
    QString name;
    int mpCost;
    int power;
};

struct EngineActorClass
{
    int id;
    QString name;
    int baseHP;
    int baseMP;
    int baseAttack;
    int baseDefense;
    int baseSpeed;
};

struct EngineActor
{
    int id;
    QString name;
    int classId;
    int level;
};

struct EngineEnemyAction
{
    int skillId;
    int minHpPercent;
    int weight;
};

struct EngineEnemy
{
    int id;
    QString name;
    int maxHP;
    int maxMP;
    int attack;
    int defense;
    int speed;
    QList<EngineEnemyAction> actionRules;
};

class GameDatabase
{
public:
    bool addClass(const EngineActorClass &actorClass);
    bool addActor(const EngineActor &actor);
    bool addSkill(const EngineSkill &skill);
    bool addEnemy(const EngineEnemy &enemy);

    const EngineActorClass *getClass(int id) const;
    const EngineActor *getActor(int id) const;
    const EngineSkill *getSkill(int id) const;
    const EngineEnemy *getEnemy(int id) const;

    QStringList validate() const;

private:
    QMap<int, EngineActorClass> classes;
    QMap<int, EngineActor> actors;
    QMap<int, EngineSkill> skills;
    QMap<int, EngineEnemy> enemies;
};

#endif
