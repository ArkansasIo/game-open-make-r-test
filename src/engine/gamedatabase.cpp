#include "gamedatabase.h"

bool GameDatabase::addClass(const EngineActorClass &actorClass)
{
    if (actorClass.id <= 0 || classes.contains(actorClass.id))
        return false;

    classes.insert(actorClass.id, actorClass);
    return true;
}

bool GameDatabase::addActor(const EngineActor &actor)
{
    if (actor.id <= 0 || actors.contains(actor.id))
        return false;

    actors.insert(actor.id, actor);
    return true;
}

bool GameDatabase::addSkill(const EngineSkill &skill)
{
    if (skill.id <= 0 || skills.contains(skill.id))
        return false;

    skills.insert(skill.id, skill);
    return true;
}

bool GameDatabase::addEnemy(const EngineEnemy &enemy)
{
    if (enemy.id <= 0 || enemies.contains(enemy.id))
        return false;

    enemies.insert(enemy.id, enemy);
    return true;
}

const EngineActorClass *GameDatabase::getClass(int id) const
{
    QMap<int, EngineActorClass>::const_iterator it = classes.constFind(id);
    if (it == classes.constEnd())
        return NULL;
    return &it.value();
}

const EngineActor *GameDatabase::getActor(int id) const
{
    QMap<int, EngineActor>::const_iterator it = actors.constFind(id);
    if (it == actors.constEnd())
        return NULL;
    return &it.value();
}

const EngineSkill *GameDatabase::getSkill(int id) const
{
    QMap<int, EngineSkill>::const_iterator it = skills.constFind(id);
    if (it == skills.constEnd())
        return NULL;
    return &it.value();
}

const EngineEnemy *GameDatabase::getEnemy(int id) const
{
    QMap<int, EngineEnemy>::const_iterator it = enemies.constFind(id);
    if (it == enemies.constEnd())
        return NULL;
    return &it.value();
}

QStringList GameDatabase::validate() const
{
    QStringList issues;

    QMap<int, EngineActor>::const_iterator actorIt = actors.constBegin();
    while (actorIt != actors.constEnd())
    {
        if (!classes.contains(actorIt.value().classId))
        {
            issues << QString("Actor %1 references missing class %2.")
                          .arg(actorIt.value().id)
                          .arg(actorIt.value().classId);
        }
        ++actorIt;
    }

    QMap<int, EngineEnemy>::const_iterator enemyIt = enemies.constBegin();
    while (enemyIt != enemies.constEnd())
    {
        const EngineEnemy &enemy = enemyIt.value();
        int i;
        for (i = 0; i < enemy.actionRules.size(); ++i)
        {
            if (enemy.actionRules[i].skillId > 0 &&
                !skills.contains(enemy.actionRules[i].skillId))
            {
                issues << QString("Enemy %1 references missing skill %2.")
                              .arg(enemy.id)
                              .arg(enemy.actionRules[i].skillId);
            }
        }
        ++enemyIt;
    }

    return issues;
}
