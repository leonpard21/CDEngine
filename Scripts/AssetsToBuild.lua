--[[ This is the build assets file, 
which points out all of the files we want to build for our game. ]] 

return
{
    {
        buildTool = "MeshBuilder.exe",
        relativePath = "Meshes/",
		assets = 
        {   
			{
                source = "Plane.aosmeshtext",
                target = "ground.aosmesh",
            },
			{
                source = "PlayerPlane.aosmeshtext",
                target = "playerPlane.aosmesh",
            },
			{
				source = "warehouse.aosmeshtext",
				target = "warehouse.aosmesh",
			},
        }
    },
	{
         buildTool = "EffectBuilder.exe",
		 relativePath = "Effects/",		 
         assets =
         {
		    {
                source = "standard.effect",
                target = "standard.effect",
            },
			{
                source = "transparent.effect",
                target = "transparent.effect",
            },
			{
                source = "debugPrimitive.effect",
                target = "debugPrimitive.effect",
            },
         },
    },
    {
         buildTool = "ShaderBuilder.exe",
		 relativePath = "Shaders/",		 
         assets =
         {
		    {
                source = "standard.vshd",
                target = "standard.vshd",
				extraInfo = { dependencies = { "shaders.common" }, optionalArguments = {"vertex"}, }
            },
            {
                source = "standard.fshd",
                target = "standard.fshd",
				extraInfo = { dependencies = { "shaders.common" }, optionalArguments = {"fragment"}, }
            },
			{
                source = "transparent.fshd",
                target = "transparent.fshd",
				extraInfo = { dependencies = { "shaders.common" }, optionalArguments = {"fragment"}, }
            },
			--DebugPrimitives Shader
			{
                source = "debugPrimitive.vshd",
                target = "debugPrimitive.vshd",
				extraInfo = { dependencies = { "shaders.common" }, optionalArguments = {"vertex"}, }
            },
			{
                source = "debugPrimitive.fshd",
                target = "debugPrimitive.fshd",
				extraInfo = { dependencies = { "shaders.common" }, optionalArguments = {"fragment"}, }
            },
         },
    },  
	{
         buildTool = "TextureBuilder.exe",
		 relativePath = "Textures/",		 
         assets =
         {
			{
                source = "eae6320.png",
                target = "eae6320.dds",
            },
			{
                source = "grey.jpg",
                target = "grey.dds",
            },
			{
                source = "floor_D.png",
                target = "floor_D.dds",
            },
			{
                source = "railing_D.png",
                target = "railing_D.dds",
            },
			{
                source = "metal_brace_D.png",
                target = "metal_brace_D.dds",
            },
			{
                source = "cement_wall_D.png",
                target = "cement_wall_D.dds",
            },
			{
                source = "wall_D.png",
                target = "wall_D.dds",
            },
		 },
	},
	{
         buildTool = "MaterialBuilder.exe",
		 relativePath = "Materials/",		 
         assets =
         {
		    {
                source = "lambert.material",
                target = "lambert.material",
            },
			{
                source = "white.material",
                target = "white.material",
            },
			{
                source = "floor.material",
                target = "floor.material",
            },
			{
                source = "railing.material",
                target = "railing.material",
            },
			{
                source = "ceiling.material",
                target = "ceiling.material",
            },			
			{
                source = "metal.material",
                target = "metal.material",
            },
			{
                source = "cement.material",
                target = "cement.material",
            },
			{
                source = "walls.material",
                target = "walls.material",
            },
			{
                source = "debugPrimitives.material",
                target = "debugPrimitives.material",
            },
		 },
	},
}