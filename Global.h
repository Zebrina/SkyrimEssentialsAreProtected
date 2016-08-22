/***
 *       ____  _         _             _     _
 *      / ___|| |  ___  | |__    __ _ | |   | |__
 *     | |  _ | | / _ \ | '_ \  / _` || |   | '_ \
 *     | |_| || || (_) || |_) || (_| || | _ | | | |
 *      \____||_| \___/ |_.__/  \__,_||_|(_)|_| |_|
 *
 */

#pragma once

struct SKSEInterface;
struct SKSEMessagingInterface;
struct SKSETaskInterface;
struct SKSEPapyrusInterface;
struct SKSEScaleformInterface;

namespace Global {
	extern SKSEMessagingInterface*	MessagingInterface;
	extern SKSETaskInterface*		TaskInterface;
	extern SKSEPapyrusInterface*	PapyrusInterface;
	extern SKSEScaleformInterface*	ScaleformInterface;

	bool QueryInterfaces(const SKSEInterface* skse, UInt32 interfaces);
}
