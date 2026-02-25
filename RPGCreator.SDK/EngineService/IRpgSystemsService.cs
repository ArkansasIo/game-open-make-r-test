// RPG Creator - Open-source RPG Engine.
// (c) 2026 Ward
//
// This file is part of RPG Creator and is distributed under the Apache 2.0 License.
// You are free to use, modify, and distribute this file under the terms of the Apache 2.0 License.
// See LICENSE for details.
//
// ---
//
// Ce fichier fait partie de RPG Creator et est distribue sous licence Apache 2.0.
// Vous etes libre de l'utiliser, de le modifier et de le distribuer sous les termes de la licence Apache 2.0.
// Voir LICENSE pour plus de details.

namespace RPGCreator.SDK.EngineService;

public enum RpgEventTriggerType
{
    Action,
    Autorun,
    Parallel,
    PlayerTouch
}

public enum RpgQuestStatus
{
    Unknown,
    Inactive,
    Active,
    Completed,
    Failed
}

public interface IRpgSystemsService : IService
{
    string? CurrentMapId { get; }
    IReadOnlyCollection<string> KnownMapIds { get; }

    bool SyncMapsFromAssets();
    bool TrySetCurrentMap(string mapId);
    bool IsTilePassable(int x, int y);

    void SetSwitch(string name, bool value);
    bool GetSwitch(string name, bool defaultValue = false);

    void SetVariable(string name, int value);
    int GetVariable(string name, int defaultValue = 0);

    bool RegisterSimpleSwitchEvent(string mapId, string eventId, int x, int y, RpgEventTriggerType triggerType, string switchName, bool switchValue);
    bool TriggerEvent(string eventId, RpgEventTriggerType triggerType);

    void RegisterQuest(string questId, string questName);
    bool AddQuestStep(string questId, string stepDescription);
    bool StartQuest(string questId);
    bool CompleteQuestStep(string questId, int stepIndex);
    RpgQuestStatus GetQuestStatus(string questId);

    bool SaveRuntime(string path);
    bool LoadRuntime(string path);
}
