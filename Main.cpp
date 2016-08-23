#include "SKSEMemUtil\SKSEMemUtil.h"
#include "Global.h"

#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "skse/GameAPI.h"

#include "skse/GameData.h"
#include "skse/GameObjects.h"

constexpr char* PluginName = "Essentials Are Protected";
constexpr char* ConfigFile = "Data\\SKSE\\Plugins\\EssentialsAreProtected.ini";
constexpr char* Section1 = "MustBeChangedBeforeNpcIsSpawned";
constexpr char* Section2 = "CanBeChangedAtAnyTime";

IDebugLog gLog("essentials_are_protected.log");

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

void Messaging_Callback(SKSEMessagingInterface::Message* msg) {
	if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded) {
		_MESSAGE("kMessage_DataLoaded begin");

		enum {
			kFlag_Essential = 0x0002,
			kFlag_Unique = 0x0040,
			kFlag_Protected = 0x0800,
		};

		DataHandler* dataHandler = DataHandler::GetSingleton();
		if (dataHandler) {
			UInt32 flags = kFlag_Essential;
			if (GetPrivateProfileInt(Section1, "bMakeAllUniqueNpcsProtected", 0, ConfigFile) != 0) {
				_SetFlags(flags, kFlag_Unique);
			}

			bool removeFollowerLevelCap = GetPrivateProfileInt(Section2, "bRemoveFollowerLevelCap", 0, ConfigFile) != 0;

			for (UInt32 i = 0; i < dataHandler->npcs.count; ++i) {
				TESNPC* npc = dataHandler->npcs[i];

				if (npc) {
					if (_TestFlags(npc->actorData.flags, flags)) {
						_ClearFlags(npc->actorData.flags, kFlag_Essential);
						_SetFlags(npc->actorData.flags, kFlag_Protected);

						_MESSAGE("0x%.8x (%s) is now protected", npc->formID, npc->fullName.GetName());
					}

					if (removeFollowerLevelCap) {
						//_MESSAGE("0x%.8x (%s) factions:", npc->formID, npc->fullName.GetName());

						for (UInt32 i = 0; i < npc->actorData.factions.count; ++i) {
							TESActorBaseData::FactionInfo faction = npc->actorData.factions[i];

							//_MESSAGE("\t%s", faction.faction->fullName.GetName());

							// Check if npc is in PotentialFollowerFaction.
							if (faction.faction->formID == 0x0005c84du && faction.rank >= 0) {
								npc->actorData.maxLevel = 0;

								_MESSAGE("removed level cap for potential follower '%s' (0x%.8x)", npc->fullName.GetName(), npc->formID);

								break;
							}
						}
					}
				}
			}
		}

		_MESSAGE("kMessage_DataLoaded end");
	}
}

extern "C" {

	bool SKSEPlugin_Query(const SKSEInterface* skse, PluginInfo* info) {
		_MESSAGE("SKSEPlugin_Query begin");

		// populate info structure
		info->infoVersion = PluginInfo::kInfoVersion;
		info->name = PluginName;
		info->version = 1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if (skse->isEditor) {
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if (skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0) {
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		bool interfaceRegistration = Global::QueryInterfaces(skse, kInterface_Messaging);

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		_MESSAGE("SKSEPlugin_Query end");

		// supported runtime version
		return interfaceRegistration;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse) {
		_MESSAGE("SKSEPlugin_Load begin");

		Global::MessagingInterface->RegisterListener(g_pluginHandle, "SKSE", Messaging_Callback);

		_MESSAGE("SKSEPlugin_Load end");

		return true;
	}

};
