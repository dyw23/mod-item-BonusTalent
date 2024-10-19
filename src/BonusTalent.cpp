#include "Chat.h"
#include "Configuration/Config.h"
#include "Player.h"
#include "Creature.h"
#include "AccountMgr.h"
#include "ScriptMgr.h"
#include "Define.h"
#include "GossipDef.h"

uint32 MaxBonusTalent = 80;
bool BonusTalentItemEnable = true;
bool BonusTalentItemAnnounce = true;

// Configuration
class mod_BonusTalent_Conf : public WorldScript
{
public:
    mod_BonusTalent_Conf() : WorldScript("mod_BonusTalent_Conf") { }

    // Load Configuration Settings
    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        BonusTalentItemEnable = sConfigMgr->GetOption<bool>("BonusTalent.Enable", true);
        BonusTalentItemAnnounce = sConfigMgr->GetOption<bool>("BonusTalent.Announce", true);
        //MaxBonusTalent = sConfigMgr->GetOption<int>("BonusTalent.MaxBonusTalent", 80);
		
    }
};
// Announce
class mod_BonusTalent_Announce : public PlayerScript
{

public:
    mod_BonusTalent_Announce() : PlayerScript("mod_BonusTalent_Announce") {}

    void OnLogin(Player* player) override
    {
        if (BonusTalentItemAnnounce)
        {
            ChatHandler(player->GetSession()).SendSysMessage("本服务器正在运行 |cff4CFF00额外天赋石 |r模块.");
        }
    }
};

class BonusTalent : public ItemScript
{
public:
    BonusTalent() : ItemScript("BonusTalent") { }

    bool OnUse(Player* p, Item* i, const SpellCastTargets &) override
    {
        if (!BonusTalentItemEnable || p->IsInCombat() || p->IsInFlight() || p->GetMap()->IsBattlegroundOrArena())
        {
            ChatHandler(p->GetSession()).PSendSysMessage("你还不能使用此物品！请稍后再试！");
            return false;
        }
		
		std::vector<uint32> vecMaxTalent = {};
        std::stringstream ss(sConfigMgr->GetOption<std::string>("BonusTalent.MaxBonusTalent", ""));
        for (std::string MaxBonusTalents; std::getline(ss, MaxBonusTalents, ';');)
        {
            vecMaxTalent.push_back(stoul(MaxBonusTalents));
        }
		if (p->GetLevel() <= 10)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("少年，你太年轻了，还是抓紧时间升级吧！");
            return false;
        }
        else
        {
			int index = (p->GetLevel() - 11) / 10;
			if (index < vecMaxTalent.size())
            {
				MaxBonusTalent = vecMaxTalent[index];
            }
            else
            {
				MaxBonusTalent = vecMaxTalent.back();
            }
        }

        if (p->GetExtraBonusTalentPoints() >= MaxBonusTalent)
        {
            ChatHandler(p->GetSession()).PSendSysMessage("你的额外天赋数量达到上限，上限为：{}!", MaxBonusTalent);
            return false;
        }

        p->RewardExtraBonusTalentPoints(1);
        p->InitTalentForLevel();
        p->DestroyItemCount(i->GetEntry(), 1, true);
        ChatHandler(p->GetSession()).PSendSysMessage("你的天赋点永久的增加了1点，你变得更强力了！");
        return true;
    }
};

void AddBonusTalentScripts()
{
    new mod_BonusTalent_Conf();
    new mod_BonusTalent_Announce();
    new BonusTalent();
}
