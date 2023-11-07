#include <algorithm>
#include <codecvt>
#include <vector>
#include <format>
#include <locale>

#if defined(if)
#define if EMIT WARNING C4005
#error The C++ Standard Library forbids macroizing the keyword "if". \
Enable warning C4005 to find the forbidden define.
#endif // if


	// std::cout << "IPStr: " << IPStr << '\n';

	if (IPStr == "127.0.0.1" || IPStr == "68.134.74.228" || IPStr == "26.66.97.190" || IsOp(PlayerController))
	{
		return true;
	}

void ServerCheatHook(AFortPlayerControllerAthena* PlayerController, FString Msg)
{
	auto PlayerState = Cast<AFortPlayerStateAthena>(PlayerController->PlayerState);

	// std::cout << "aa!\n";

	if (!PlayerState || !IsOperatora(PlayerState, PlayerController))
		return;

	std::vector<std::string> Arguments;
	auto OldMsg = Msg.ToString();

	auto ReceivingController = PlayerController; // for now
	auto ReceivingPlayerState = PlayerState; // for now

	auto firstBackslash = OldMsg.find_first_of("\\");
	auto lastBackslash = OldMsg.find_last_of("\\");

	auto& ClientConnections = GetWorld()->NetDriver->ClientConnections;

	/* if (firstBackslash == lastBackslash)
	{
		SendMessageToConsole(PlayerController, L"Warning: You have a backslash but no ending backslash, was this by mistake? Executing on you.");
	} */

	if (firstBackslash != lastBackslash && firstBackslash != std::string::npos && lastBackslash != std::string::npos) // we want to specify a player
	{
		std::string player = OldMsg;

		player = player.substr(firstBackslash + 1, lastBackslash - firstBackslash - 1);

		for (int i = 0; i < ClientConnections.Num(); i++)
		{
			auto CurrentPlayerController = Cast<AFortPlayerControllerAthena>(ClientConnections[i]->PlayerController);

			if (!CurrentPlayerController)
				continue;

			auto CurrentPlayerState = Cast<AFortPlayerStateAthena>(CurrentPlayerController->PlayerState);

			if (!CurrentPlayerState)
				continue;

			if (CurrentPlayerState->GetPlayerName().ToString() == player) // hopefully we arent on adifferent thread
			{

			}
		}
	}

	if (!ReceivingController || !ReceivingPlayerState)
	{
		SendMessageToConsole(PlayerController, L"Unable to find player!");
		return;
	}

	{
		auto Message = Msg.ToString();

		size_t start = Message.find('\\');
		while (start != std::string::npos) // remove the playername
		{
			size_t end = Message.find('\\', start + 1);

			if (end == std::string::npos)
				break;

			Message.replace(start, end - start + 2, "");
			start = Message.find('\\');
		}

		int zz = 0;

		// std::cout << "Message Before: " << Message << '\n';

		while (Message.find(" ") != -1)
		{

		}

		// if (zz == 0)
		{
			Arguments.push_back(Message);
			// std::cout << std::format("[{}] {}\n", zz, Message);
			zz++;
		}

		// std::cout << "Message After: " << Message << '\n';
	}

	auto NumArgs = Arguments.size() == 0 ? 0 : Arguments.size() - 1;

	// std::cout << "NumArgs: " << NumArgs << '\n';

	if (Arguments.size() >= 1)
	{
		auto& Command = Arguments[0];
		std::transform(Command.begin(), Command.end(), Command.begin(), ::tolower);

		if (Command == "giveitem")
		{
			if (NumArgs < 1)
			{
				SendMessageToConsole(PlayerController, L"Please provide a WID!");
				return;
			}

			auto& weaponName = Arguments[1];
			int count = 1;

			try
			{
				if (NumArgs >= 2)
					count = std::stoi(Arguments[2]);
			}
			catch (...)
			{
			}

			auto WID = Cast<UFortWorldItemDefinition>(UObject::FindObjectSlow(weaponName + "." + weaponName));

			if (WID)
			{
				if (IsPrimaryQuickbar(WID) && IsInventoryFull(ReceivingController, 1, WID, count))
				{
					auto Pawn = Cast<AFortPlayerPawnAthena>(ReceivingController->Pawn);

					if (!Pawn)
					{
						SendMessageToConsole(PlayerController, L"No pawn to spawn weapon at!");
						return;
					}

					SpawnPickup(WID, Pawn->K2_GetActorLocation(), count, EFortPickupSourceTypeFlag::Player, EFortPickupSpawnSource::Unset, -1, Pawn);
					Update(ReceivingController);
					FString Message = L"";
					SendMessageToConsole(PlayerController, std::wstring(L" Successfully spawned pickup, " + count + std::wstring(L" ") + string_towstring(weaponName)).c_str());
				}
				// else if (Command == "restartserver") { RestartServer(); }

				else if (Command == "help")
				{
					std::wstring Message = LR"(
cheat infiniteammo - Toggles infinite ammo and infinite consumables
cheat buildfree - Toggles material depletion
cheat giveitem <ShortWID> <Count=1> - Gives a weapon to the executing player, if inventory is full drops a pickup on the player.
cheat summon <BlueprintClassPathName> <Count=1>- Summons the specified blueprint class at the executing player's location. Note: There is a limit on the count.
cheat bugitgo <X> <Y> <Z> - Teleport to a location.
cheat sethealth <Health=100.f> - Sets executing player's health.
cheat setshield <Shield=0.f> - Sets executing player's shield.
cheat setmaxplayers <MaxPlayers=PlaylistDefault> - Sets the max players that can join. This will not kick anyone, for ex. 20 people in server, but max is 10.
cheat listplayers - Gives you all players names.
cheat god - Toggles god.
cheat suicide - Kills you.
cheat dbno - Toggles down but not out.
cheat spawnvolume - Spawns a creative volume at a player.
cheat launch <X> <Y> <Z> - Launches a player.
cheat teleport - Teleports to what the player is looking at.
cheat fly - Enables fly on a player.
cheat ghost - Enables ghost on a player.
cheat setflyspeed <Speed=600.f> - Sets fly speed for a player (VERY BUGGY).
cheat spawnplayset <PlaysetFullPath> - Spawns a playset at a player.
cheat applycid <CIDShortName> - Sets a player's character.
cheat demospeedserver <Speed=1.f> - Sets the server's speed/time dilation.
cheat setpickaxe <ShortPickaxeName> - Changes the player's pickaxe to the new pickaxe.
cheat op \PlayerName\ - Gives operator to this player's ip.
cheat deop \PlayerName\ - Removes operator from this player's ip.
cheat giveperm - This command lets everyone on the player's island to have permissions (fly, build, etc.).
cheat pausesafezone - Pauses safe zone.

If you want to execute a command on a certain player, surround their name (case sensitive) with \, and put the param anywhere. Example: cheat sethealth \Milxnor\ 100
)";

					/*
					cheat whitelist <on/off> - Turns whitelist on or off.
					cheat addwhitelist <IP> - Adds an IP to the whitelist.
					cheat removewhitelist <IP> - Removes an IP from the whitelist.
					cheat kick <Player> <Reason> - Kicks a player.
					cheat ban <IP/Player> <Reason> - Bans a player.
					cheat pardon <IP> - Unbans a player.
					cheat restartserver - Restarts the server.
					cheat applyge <GameplayEffectClass> <Level=1.0f> - Gives a gameplay effect to executing player.
					cheat freeze - Toggles freeze.
					

				else
				{
					SendMessageToConsole(PlayerController, L"Not a valid command!");
				}
			}
		}
