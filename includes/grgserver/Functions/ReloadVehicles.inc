/**
 * <summary>
 *	Destroyes all vehicles, creates the vehicles saved in the database, adds vehicle components and puts the NPCs in the vehicles if the ID of the vehicle is linked to a NPC.
 * </summary>
 *
 * <param name="isStatic">
 *	<item name="true">Use AddStaticVehicleEx for creating the vehicle (Only possible in OnGameModeInit and OnFilterScriptInit)</item>
 *	<item name="false">Use CreateVehicle for creating the vehicle (Can not spawn some trains)</item>
 * </param>
 *
 * <param name="playerID">
 *	The ID of the player which executes a command which reloads the vehicles (Optional).
 * </param>
 *
 * <remarks>
 *	Vehicles with model ID 537 (Freight) and 538 (Streak) are only destroyed if isStatic is set to true!
 * </remarks>
 */

ReloadVehicles(isStatic, playerID = !INVALID_PLAYER_ID)
{
	AdminPrint(playerID, COLOR_INFO, "* Destroying vehicles...");
	Count1(vehicleID, MAX_VEHICLES)
	{
		if (VehicleProperty:vehicleID[NPC_ONLY])// We should not remove NPC vehicles
		{
			continue;
		}
		new modelID = GetVehicleModel(vehicleID);
		if (modelID)
		{
			if (isStatic || (modelID != 537 && modelID != 538))// We are unable to respawn Freight and Streak using CreateVehicle... so only remove them if isStatic is true
			{
				DestroyVehicle(vehicleID);
			}
		}
		VehicleProperty:vehicleID[CURRENTFUEL] = 0;
		VehicleProperty:vehicleID[JOBID] = 0;
		VehicleProperty:vehicleID[SAVEID] = 0;
	}

	AdminPrint(playerID, COLOR_INFO, "* Loading vehicles...");
	new count;
	mysql_query("SELECT * FROM `vehicles`");
	mysql_store_result();
	while (FetchMySQLRow())
	{
		new vehicleID = CreateVehicleEx(strval(GetMySQLField("modelId")), floatstr(GetMySQLField("posX")), floatstr(GetMySQLField("posY")), floatstr(GetMySQLField("posZ")), floatstr(GetMySQLField("angle")), strval(GetMySQLField("color1")), strval(GetMySQLField("color2")), isStatic);
		if (vehicleID)
		{
			VehicleProperty:vehicleID[FRONTLIGHTID] = strval(GetMySQLField("frontLightId"));
			VehicleProperty:vehicleID[HASNAVI] = strval(GetMySQLField("hasNavi"));
			VehicleProperty:vehicleID[JOBID] = strval(GetMySQLField("jobId"));
			VehicleProperty:vehicleID[MILEAGE] = floatstr(GetMySQLField("mileAge"));
			VehicleProperty:vehicleID[NEONLIGHTID] = strval(GetMySQLField("neonLightId"));
			VehicleProperty:vehicleID[PAINTJOBID] = strval(GetMySQLField("paintjobId"));
			VehicleProperty:vehicleID[PRICE] = floatstr(GetMySQLField("price"));
			VehicleProperty:vehicleID[SAVEID] = strval(GetMySQLField("id"));
			SetVehicleHealth(vehicleID, floatstr(GetMySQLField("health")));
			LinkVehicleToInterior(vehicleID, strval(GetMySQLField("interior")));
			LockVehicle(vehicleID, strval(GetMySQLField("locked")));
			ChangeVehiclePaintjob(vehicleID, VehicleProperty:vehicleID[PAINTJOBID]);
			VehicleProperty:vehicleID[CURRENTFUEL] = floatstr(GetMySQLField("currentFuel"));
			VehicleProperty:vehicleID[OWNERUSERID] = strval(GetMySQLField("ownerUserId"));
			VehicleProperty:vehicleID[RADIOSTATIONID] = strval(GetMySQLField("radioStationId"));
			VehicleProperty:vehicleID[SELLERUSERID] = strval(GetMySQLField("sellerUserId"));
			SetVehicleNumberPlate(vehicleID, GetMySQLField("numberplate"));
			count++;
		}
		else
		{
			AdminPrint(playerID, COLOR_ERROR, "  Can not create vehicle '%d' (ModelID %d)!", strval(GetMySQLField("id")), strval(GetMySQLField("modelId")));
		}
	}
	mysql_free_result();
	AdminPrint(playerID, COLOR_INFO, "  %d vehicles loaded", count);

	count = 0;
	AdminPrint(playerID, COLOR_INFO, "* Adding vehicle components...");
	mysql_query("SELECT * FROM `vehiclecomponents`");
	mysql_store_result();
	while (FetchMySQLRow())
	{
		new foundVehicle;
		Count1(vehicleID, MAX_VEHICLES)
		{
			if (VehicleProperty:vehicleID[SAVEID] == strval(GetMySQLField("vehicleId")))
			{
				AddVehicleComponentEx(vehicleID, strval(GetMySQLField("component")));
				count++;
				foundVehicle = true;
				break;
			}
		}
		if (!foundVehicle)
		{
			AdminPrint(playerID, COLOR_ERROR, "  Vehicle not found: %d", strval(GetMySQLField("vehicleId")));
		}
	}
	mysql_free_result();
	AdminPrint(playerID, COLOR_INFO, "  %d vehicle components added", count);
}