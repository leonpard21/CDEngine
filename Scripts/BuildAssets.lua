--[[
	This file contains the logic for building assets
]]

-- Static Data Initialization
--===========================

local s_AuthoredAssetDir, s_BuiltAssetDir, s_BinDir
do
	-- AuthoredAssetDir
	do
		local key = "AuthoredAssetDir"
		local errorMessage
		s_AuthoredAssetDir, errorMessage = GetEnvironmentVariable( key )
		if not s_AuthoredAssetDir then
			error( errorMessage )
		end
	end
	-- BuiltAssetDir
	do
		local key = "BuiltAssetDir"
		local errorMessage
		s_BuiltAssetDir, errorMessage = GetEnvironmentVariable( key )
		if not s_BuiltAssetDir then
			error( errorMessage )
		end
	end

	-- BinDir
	do
		local key = "BinDir"
		local errorMessage
		s_BinDir, errorMessage = GetEnvironmentVariable( key )
		if not s_BinDir then
			error( errorMessage )
		end
	end
end

-- Function Definitions
--=====================
local function GetLastestDependAssetTime(i_relativePath, i_dependenices)
	local lastWriteTime_denpendency = 0;
	for i, dependency in ipairs( i_dependenices ) do
		print("dependency:"..i_relativePath .. dependency)
		local lastwritetime = GetLastWriteTime(i_relativePath .. dependency)
		print("lastwritetime:"..lastwritetime)
		lastWriteTime_denpendency = math.max( lastWriteTime_denpendency, lastwritetime )
	end
	return lastWriteTime_denpendency
end

local function BuildAsset( i_relativePath, i_sourceFile, i_targetFile, i_builderFileName, i_extraInfo )
	-- Get the absolute paths to the source and target
	local path_source = s_AuthoredAssetDir .. i_relativePath .. i_sourceFile
	local path_target = s_BuiltAssetDir .. i_relativePath .. i_targetFile

	local dependAssetsTable = nil
	local optionalArgumentsTable = nil
	if i_extraInfo ~= nil then
		dependAssetsTable = i_extraInfo.dependencies
		optionalArgumentsTable = i_extraInfo.optionalArguments
	end
	
	-- If the source file doesn't exist then it can't be built
	do
		local doesSourceExist = DoesFileExist( path_source )
		if not doesSourceExist then
			OutputErrorMessage( "The source asset doesn't exist", path_source )
			return false
		end
	end

	-- Find the appropriate builder for this asset type
	local path_builder
	do
		if type( i_builderFileName ) == "string" then
			path_builder = s_BinDir .. i_builderFileName
			if not DoesFileExist( path_builder ) then
				error( "The specified builder (\"" .. path_builder .. "\") doesn't exist", stackLevel )
			end
		else
			error( "The specified builder file name must be a string (instead of a " ..
				type( i_builderFileName ) .. ")", stackLevel )
		end
	end

	-- Decide if the target needs to be built
	local shouldTargetBeBuilt
	do
		-- The simplest reason a target should be built is if it doesn't exist
		local doesTargetExist = DoesFileExist( path_target )
		if doesTargetExist then	
			-- Even if the target exists it may be out-of-date.
			-- If the source has been modified more recently than the target
			-- then the target should be re-built.
			local lastWriteTime_source = GetLastWriteTime( path_source )
			local lastWriteTime_target = GetLastWriteTime( path_target ) 
			shouldTargetBeBuilt = lastWriteTime_source > lastWriteTime_target
			if (not shouldTargetBeBuilt) and dependAssetsTable ~= nil then
				local lastWriteTime_DenpencyAsset = GetLastestDependAssetTime( s_AuthoredAssetDir .. i_relativePath, dependAssetsTable );
				shouldTargetBeBuilt = lastWriteTime_DenpencyAsset > lastWriteTime_target
			end
			if not shouldTargetBeBuilt then
				-- Even if the target was built from the current source
				-- the builder may have changed which could cause different output
				local lastWriteTime_builder = GetLastWriteTime( path_builder )
				shouldTargetBeBuilt = lastWriteTime_builder > lastWriteTime_target
			end
		else
			shouldTargetBeBuilt = true;
		end
	end
	
--[[
	-- Build the target if necessary
	if shouldTargetBeBuilt then
		-- Create the target directory if necessary
		CreateDirectoryIfNecessary( path_target )

		-- Copy the source to the target
		local result, errorMessage = CopyFile( path_source, path_target )
		if result then
			-- Display a message when an asset builds successfully
			print( "Built " .. path_source )
			return true;
		else
			-- Display a message describing why the asset didn't build
			OutputErrorMessage( errorMessage, path_source )
			return false
		end
	else
		return true
	end
]]
	
	-- Build the target if necessary
	if shouldTargetBeBuilt then
		-- Create the target directory if necessary
		CreateDirectoryIfNecessary( path_target )
		-- Build
		do
			-- The command starts with the builder
			local command = "\"" .. path_builder .. "\""
			-- The source and target path must always be passed in
			local arguments = "\"" .. path_source .. "\" \"" .. path_target .. "\""
			-- If you create a mechanism so that some asset types could include extra arguments
			-- you would concatenate them here, something like:
			-- arguments = arguments .. " " .. i_optionalArguments
			-- If the arguments are a table of strings, 
			-- you can convert a table of strings to a single string separated 
			-- by spaces using the table.concat()
			if(optionalArgumentsTable ~= nil) then
				--print("the optionalArguments are:" .. i_optionalArguments)
				arguments = arguments .. " " .. table.concat( optionalArgumentsTable, " " )
			end
			-- IMPORTANT NOTE:
			-- If you need to debug a builder you can put print statements here to
			-- find out what the exact command line should be.
			-- "command" should go in Debugging->Command
			-- "arguments" should go in Debugging->Command Arguments

			-- Surround the entire command line in quotes
			local commandLine = "\"" .. command .. " " .. arguments .. "\""
			local result, terminationType, exitCode = os.execute( commandLine )
			if result then
				-- Display a message for each asset
				print( "Built " .. path_source )
				-- Return the exit code for informational purposes since we have it
				return true, exitCode
			else
				-- The builder should already output a descriptive error message if there was an error
				-- (remember that you write the builder code,
				-- and so if the build process failed it means that _your_ code has returned an error code)
				-- but it can be helpful to still return an additional vague error message here
				-- in case there is a bug in the specific builder that doesn't output an error message
				do
					local errorMessage = "The command " .. tostring( commandLine )
					if terminationType ~= "signal" then
						errorMessage = errorMessage .. " exited with code "
					else
						errorMessage = errorMessage .. " was terminated by the signal "
					end
					errorMessage = errorMessage .. tostring( exitCode )
					OutputErrorMessage( errorMessage, path_source )
				end
				-- There's a chance that the builder already created the target file,
				-- in which case it will have a new time stamp and wouldn't get built again
				-- even though the process failed
				if DoesFileExist( path_target ) then
					local result, errorMessage = os.remove( path_target )
					if not result then
						OutputErrorMessage( "Failed to delete the incorrectly-built target: " .. errorMessage, path_target )
					end
				end

				return false, exitCode
			end
		end
	else
		return true
	end
end

local function BuildAssets( i_assetsToBuild )
	local wereThereErrors = false
	
	for i, assetInfo_singleType in ipairs( i_assetsToBuild ) do
		local builderFileName = assetInfo_singleType.buildTool
		local relativePath = assetInfo_singleType.relativePath
		local assets = assetInfo_singleType.assets
		for j, assetInfo in ipairs( assets ) do
			local source = assetInfo.source
			local target = assetInfo.target
			local extraInfo = assetInfo.extraInfo
			if not BuildAsset( relativePath, source, target, builderFileName, extraInfo) then
				-- If there's an error then the asset build should fail,
				-- but we can still try to build any remaining assets
				wereThereErrors = true
			end
		end
	end

	--[["i_assetsToBuild" should be the exact table that you return from AssetsToBuild.lua,
	and so you will need to fill in this function to iterate through every asset in that table
	and then call BuildAsset() with the necessary information to build that asset.]]

	--[[You may want to review tableExamples.lua in the Using Lua example.
	I also provide a quick reference below to the functionality you'll need.]]
    --[[
	To iterate through an array:

	for i, someValue in ipairs( someTable ) do
		-- If "someTable" contains an array, then "i" is the index (starting at 1)
		-- and "someValue" is the value at that index (which could be another table).
		-- Note that the name "i" is arbitrary, and you can name this local variable
		-- anything you want.
	end

	To iterate through a dictionary:

	for key, value in pairs( someTable ) do
		-- This will give you _every_ key/value pair in "someTable" (including integer keys),
		-- but the order is non-deterministic.
		-- "key" and "value" are both arbitrary variable names, and you can choose whatever you'd like.
	end

	To find specific string keys in a table you can use the "." operator, like this:
	local someValue = someTable.someKey
	Notice that there are no quotes around "someKey", even though it is a string key.
	So, if you had a table of assets for a specific type and you wanted to find the builder to use
	you might do something like this:
	local builder = assets.builder

	So, a rough skeleton of what this function might look like if you list every asset in a single array would be:
	for i, assetInfo in ipairs( i_assetsToBuild ) do
		local builderFileName = assetInfo.builder
		-- etc.
		if not BuildAsset( builderFileName, etc. ) then
			-- If there's an error then the asset build should fail,
			-- but we can still try to build any remaining assets
			wereThereErrors = true
		end
	end

	And a rough skeleton of what this function might look like if you group assets accordin to their type might be:
	for i, assetInfo_singleType in ipairs( i_assetsToBuild ) do
		local builderFileName = assetInfo_singleType.builder
		-- etc.
		local assets = assetInfo_singleType.assets
		for j, assetInfo in ipairs( assets ) do
			-- TODO
				if not BuildAsset( builderFileName, etc. ) then
				-- If there's an error then the asset build should fail,
				-- but we can still try to build any remaining assets
				wereThereErrors = true
			end
		end
	end

	Or, perhaps:
	for assetType, assetInfo_singleType in ppairs( i_assetsToBuild ) do
		local builderFileName = assetInfo_singleType.builder
		-- etc.
		local assets = assetInfo_singleType.assets
		for j, assetInfo in ipairs( assets ) do
			-- TODO
				if not BuildAsset( builderFileName, etc. ) then
				-- If there's an error then the asset build should fail,
				-- but we can still try to build any remaining assets
				wereThereErrors = true
			end
		end
	end
	--]]
	-- End of TODO. The following should be the final line of the function:
	return not wereThereErrors
end

-- Entry Point
--============

-- Command line arguments are represented in Lua as three dots ("...")
local commandLineArgument = ...
if commandLineArgument then
	local path_assetsToBuild = commandLineArgument
	if DoesFileExist( path_assetsToBuild ) then
		local assetsToBuild = dofile( path_assetsToBuild )
		return BuildAssets( assetsToBuild )
	else
		OutputErrorMessage( "The path to the list of assets to build that was provided to BuildAssets.lua as argument #1 (\"" ..
			path_assetsToBuild .. "\") doesn't exist" )
	end
else
	OutputErrorMessage( "BuildAssets.lua must be called with the path to the list of assets to build as an argument" )
end
