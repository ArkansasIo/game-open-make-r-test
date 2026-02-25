#include <QCoreApplication>
#include <QDateTime>
#include <QStringList>
#include <QTextStream>

#include "battleengine.h"
#include "eventcompiler.h"
#include "eventruntime.h"
#include "gamedatabase.h"
#include "gamestate.h"
#include "tilemap.h"

static GameDatabase buildDemoDatabase()
{
    GameDatabase db;

    EngineActorClass heroClass;
    heroClass.id = 1;
    heroClass.name = "Adventurer";
    heroClass.baseHP = 40;
    heroClass.baseMP = 15;
    heroClass.baseAttack = 12;
    heroClass.baseDefense = 8;
    heroClass.baseSpeed = 7;
    db.addClass(heroClass);

    EngineActor hero;
    hero.id = 1;
    hero.name = "Stephen";
    hero.classId = 1;
    hero.level = 3;
    db.addActor(hero);

    EngineSkill slash;
    slash.id = 1;
    slash.name = "Slash";
    slash.mpCost = 0;
    slash.power = 10;
    db.addSkill(slash);

    EngineEnemy slime;
    slime.id = 1;
    slime.name = "Green Slime";
    slime.maxHP = 30;
    slime.maxMP = 0;
    slime.attack = 8;
    slime.defense = 4;
    slime.speed = 4;
    db.addEnemy(slime);

    return db;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QTextStream out(stdout);

    qsrand(static_cast<uint>(QDateTime::currentMSecsSinceEpoch() & 0xFFFFFFFF));

    GameDatabase db = buildDemoDatabase();
    QStringList issues = db.validate();
    if (!issues.isEmpty())
    {
        out << "Database validation failed:\n";
        int i;
        for (i = 0; i < issues.size(); ++i)
            out << " - " << issues[i] << "\n";
        return 1;
    }

    GameState state;
    TurnBattleEngine battle;
    EventRuntime events(&db, &state, &battle);
    OpenRpgEventCompiler compiler;
    QStringList compileWarnings;
    TileMap map(20, 15, 3);

    map.setTile(5, 5, 0, 12);
    map.setRegionId(5, 5, 0, 7);
    map.setBlocked(5, 5, 1, true);

    QString commonEventXml =
        "<event type=\"topLevelEvent\">"
        "  <event type=\"displayMessage\">"
        "    <line>Common Event: You found a potion.</line>"
        "  </event>"
        "  <event type=\"variableOperation\" operation=\"add\" variableToChange=\"single\" variableID=\"2\" operand=\"value\" value=\"1\"/>"
        "</event>";

    QList<EventCommand> compiledCommonEvent;
    if (!compiler.compileCommonEventXml(commonEventXml, &compiledCommonEvent, &compileWarnings))
    {
        out << "Common event compile failed.\n";
        return 2;
    }
    events.registerCommonEvent(1, compiledCommonEvent);

    QString mapEventXml =
        "<event type=\"topLevelEvent\">"
        "  <event type=\"displayMessage\">"
        "    <line>Map Event: A slime appears!</line>"
        "  </event>"
        "  <event type=\"switchOperation\" switchToChange=\"single\" switchID=\"1\" operation=\"turnOn\"/>"
        "  <event type=\"conditionalBranch\">"
        "    <conditions conditionsToMatch=\"all\" executeCustomHandlerIfConditionsNotMet=\"true\">"
        "      <condition type=\"switch\" switchID=\"1\" switchReferencedInVariable=\"false\" isOn=\"true\"/>"
        "    </conditions>"
        "    <onTrueEvents>"
        "      <event type=\"enemyEncounter\" specialConditions=\"none\" battleBackgroundType=\"useMapOrTerrainSetting\" escapeHandler=\"endEventProcessing\" defeatHandler=\"gameOver\" monsterGroupID=\"1\" monsterGroupReferencedInVariable=\"false\" beginBattleWithFirstStrike=\"false\">"
        "        <victoryEvents>"
        "          <event type=\"displayMessage\"><line>Victory!</line></event>"
        "        </victoryEvents>"
        "      </event>"
        "      <event type=\"callCommonEvent\" eventID=\"1\" eventReferencedInVariable=\"false\"/>"
        "      <event type=\"variableOperation\" operation=\"add\" variableToChange=\"single\" variableID=\"1\" operand=\"value\" value=\"10\"/>"
        "    </onTrueEvents>"
        "    <onFalseEvents>"
        "      <event type=\"displayMessage\"><line>Condition failed.</line></event>"
        "    </onFalseEvents>"
        "  </event>"
        "</event>";

    QList<EventCommand> compiledMapEvent;
    if (!compiler.compileMapEventXml(mapEventXml, &compiledMapEvent, &compileWarnings))
    {
        out << "Map event compile failed.\n";
        return 3;
    }
    events.registerMapEvent(1, compiledMapEvent);

    if (!compileWarnings.isEmpty())
    {
        out << "Compiler warnings:\n";
        int w;
        for (w = 0; w < compileWarnings.size(); ++w)
            out << " - " << compileWarnings[w] << "\n";
    }

    if (!events.runMapEvent(1))
    {
        out << "Map event execution failed.\n";
        return 4;
    }

    QStringList runtimeLog = events.flushLog();
    int i;
    for (i = 0; i < runtimeLog.size(); ++i)
        out << runtimeLog[i] << "\n";

    out << "Variable[1] (gold): " << state.getVariable(1) << "\n";
    out << "Variable[2] (potions): " << state.getVariable(2) << "\n";
    out << "Switch[1] (slime encounter seen): " << (state.getSwitch(1) ? "true" : "false") << "\n";
    out << "Map tile(5,5,0): " << map.tileId(5, 5, 0) << "\n";
    out << "Map region(5,5,0): " << map.regionId(5, 5, 0) << "\n";
    out << "Map blocked(5,5): " << (map.isBlocked(5, 5) ? "true" : "false") << "\n";
    out.flush();

    return 0;
}
