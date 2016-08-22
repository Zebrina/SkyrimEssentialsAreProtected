#include "SKSEMemUtilLib\SKSEMemUtil.h"
#include "Global.h"

#include "skse/PluginAPI.h"
#include "skse/skse_version.h"
#include "skse/GameAPI.h"

#include "skse/GameData.h"

constexpr char* PluginName = "Essentials Are Protected";

IDebugLog gLog("essentials_are_protected.log");

PluginHandle g_pluginHandle = kPluginHandle_Invalid;

void Messaging_Callback(SKSEMessagingInterface::Message* msg) {
	if (msg->type == SKSEMessagingInterface::kMessage_DataLoaded) {
		_MESSAGE("kMessage_DataLoaded begin");

		DataHandler* dataHandler = DataHandler::GetSingleton();

		enum {
			kFlag_Essential = 0x0002,
			kFlag_Protected = 0x0800,
		};

		if (dataHandler) {
			for (UInt32 i = 0; i < dataHandler->npcs.count; ++i) {
				TESNPC* npc = dataHandler->npcs[i];

				if (npc && _TestFlags(npc->actorData.flags, kFlag_Essential)) {
					_ClearFlags(npc->actorData.flags, kFlag_Essential);
					_SetFlags(npc->actorData.flags, kFlag_Protected);

					//_MESSAGE("\t0x%.8x (%s) is now protected", npc->formID, npc->fullName.GetName());
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
