# Tutorials

# TO CREATE AN EVENT:

In Events.h, create a new event on the form:
```cpp
    struct EventName : public Event
    {
        EventName(var param1, var param2) : variable1{ param1 }, variable2{ param2 } {};
        var variable1;
        var variable2;
    }
```

The number of parameters and variables is arbitrary.

In your class, create a (private) method. As a parameter this method should only take a pointer to the
type of event it will subscribe to, all other parameters needed for the function will be sent throught the event bus when publishing the event.

```cpp
    void methodName(EventName* evnt);
```
Don't forget to add

```cpp
    struct EventName;
```

to your class for forward declaration.
<br><br>
### TO SUBSCRIBE TO AN EVENT:

To subscribe to the event, run the following code in a method (for example in your class constructor)
in the class you wish to subscribe to the event. This can be either an existing class or a newly
created one:

```cpp
    EventBus::GetInstance().Subscribe(this, &ClassName::methodName);
```

To unsubscribe, simply call

```cpp
    EventBus::GetInstance().Unsubscribe(this, &ClassName::methodName);
```

In methodName, put the code you want to execute when the event is published. The parameters
of the event can be accessed by:

```cpp
    evnt->variable1;
    evnt->variable2;
```

To publish the event to make the code in methodName run, call the following code:

```cpp
    EventBus::GetInstance().Publish(&EventName(param1, param2));
```

# TO USE INPUT:

To use input by the keys W, A, S, D, Q and E, subscribe to the event type MovementInput.
To use input by the left CTRL key, subscribe to the event type ModifierInput. The parameters
are the SCAN_CODE of the key and whether it has just been pressed or released.

To use input from the mouse buttons, subscribe to the event type MouseClick. The parameters
are the MOUSE_BUTTON code of the button and whether it has just been pressed or released.

To use input from mouse movement, subscribe to the event type MouseMovement. The parameters
are the x and y coordinates of the mouse's movement.

To use input from the mouse scroll, subscribe to the event type MouseScroll. The parameters
are either 1 if scrolling forward or -1 if scrolling backwards.

## TO CREATE NEW INPUT:
To add new keys to the output, create a new event (if needed) as described above. In Input.cpp,
in the function SetKeyState, add an else if statement in the following way:

```cpp
    else if (key == SCAN_CODES::KEY_NAME1 || key == SCAN_CODES::KEY_NAME1)
    {
        if (justPressed)
        {
            EventBus::GetInstance().Publish(&EventName(key, justPressed));
        }
        else if (!pressed)
        {
            EventBus::GetInstance().Publish(&EventName(key, pressed));
        }
    }
```
or add an additional SCAN_CODE to the conditions of one of the existing statements. 

Then subscribe to the correct event type as previously described.


# PBR TEXTURES:
In a .mtl file the textures should be placed as follows.

map_Kd "Albedo_texture.suffix" (albedo/diffuse)
map_Ks "Roughness_texture.suffix" (roughness/specular)
map_Ka "Metalness_texture.suffix" (metalness/ambient)
map_Kn "Normals_texture.suffix" (normals)
map_Ke "Emissive_texture.suffix" (emissive)

# DDS Texture Extension
If you want to reduce aliasing at large distances, you should convert your textures to the .dds image format.

In the following link you can download a .dds-converter which converts a texture from a given format into the .dds format.
https://vvvv.org/contribution/texconvgui

## Settings in the texConvGui-tool
You will not need to change every setting in the tool, only the ones mentioned below:

### Folders
In the program, you will have to enter the source & destination folders for the textures

### File Mask
Set the "File Mask" to the format of the textures you want to convert from. 
If you want to convert from jpg to dds, set the "File Mask" to .jpg

### Resizing
In order for the creation of mipmaps to work, the texture needs to be in "common sizes" such as 512x512, 1024x1024, 2048x2048.
If your texture is close to either one of them, (for example 500x500), you can use the "resizing" option to resize the texture to 512x512.
Set to 0 if you do not want to resize the texture.

### Adressing Mode
Set this to wrap

### Output Options
Set this to B8G8R8A8_UNORM

### Mip Map Level
Set this to 0 to generate mipmaps

### Object File modification
When you have created your new textures, you will have to modify how your object file reads the new texture (depending on object file format)

### .Obj-file modifications (changing the .mtl file)
This is the file format which will be most commonly used in our project, and all you need to do is change the file extensions of the textures to .dds from whatever file extension it had earlier

### Other
If you wish to use other object formats where the textures are embedded, you will have to change them using external programs such as "Blender" or "Maya".

## How to create a new font format/image
Hiero is a tool to generate new fonts in *fnt* format.
**Download:** *http://www.mediafire.com/file/hlwbhemfgog51tu/hiero.jar/file*

The result of this will be *two files*, one containing information about the font and characters, and one containing the font image.

First select the font you want in the top left combo box *(it is also possible to choose a custom .ttf file)*.
Below the combo box, select the **Java** radio button for **Rendering**. This will enable the **Effects** combo box on the top right. 
For this tutorial, we will not be using any effects, but you may want to implement *distance field* once you have your text rendering.

Next select the **Glyph Cache** radio button above the font preview box at the bottom. By default **Sample Text** is selected. 
This will give us more options for the output image. You will see **Page width** and **Page height** to the right of the font preview box,
under where you have selected **Glyph Cache**. We will be making a *512x512* font image, so set these both to *512*.

Now back up to the top left, right above where you set the rendering to **Java**, you will see a size input.
What you want to do is increment the size to just before **Pages** is *2* 
*(you will see **Pages** above where you set the width and height of the output image)*. 
This will increase the size of the font as large as can possibly fit on one page *(all the characters fit in the 512x512 image)*. 
With the **Arial font** i was able to increase the size to *73* before some characters had to be moved to the next page, 
where you would have *two 512x512* font images instead of just *one*.

On the bottom right are *four* input boxes for padding, for left, top, right and bottom.
We want to give each character a little padding in the final output image so we have less a chance of sampling surrounding
characters in the shaders when sampling the texture for a character. When the quad on the screen that we are drawing a character
is too small, sampling from the texture is less accurate and may end up getting values from the next character.
Let's set all these to *five* pixels for padding.

That should be it for setting up. Let's get our bitmap font files now. Click on **File** in the top menu, 
then click **Save BMFont files (text)...**. Choose a directory and name for the output file, however,
make sure that the name does not include any spaces or special letters. 
When you click save, it will save a *.fnt* file containing information about rendering the font, and *.png* containing the font image.
If you choose a font with a space in its name, for example **Javanese Text**, make sure that you *delete* the space in the fnt file,
and that the name of the *fnt* and the *png files* have the same name as mentioned in the *fnt file*.

Having troubles running the program?
Make sure that you have the latest *Java Runtime* installed.

# How to handle the window
**Resolution width** and **height** are the sizes of the rendered scene, while **window width** and **height** are the sizes of the window which 
the scene is rendered in. These can be changed in *config.txt*.

The **window mode** variable, which also can be found the config.txt file, controls whether the user wants a **window** *(0)*, 
**windowed fullscreen** *(1)* or **exclusive fullscreen** *(2)*:

**Windowed fullscreen** means that the window will cover all of your screen with the chosen resolution and therefore *overrides* the chosen 
window size. This allows other applications and windows to continue running in the background.

**Exclusive fullscreen** mode gives your game complete ownership of the display and allocation of resources of your graphics card. This means that
exclusive fullscreen may save a couple of your frames per second and is therefore recommended while playing. It should also be noted that the
exclusive fullscreen mode will also override the window size settings until you loose focus by, for example, pressing the *alt+enter* combination on
your keyboard. While doing so, the exclusive fullscreen will be changed to a window which will have the size which is decided in the config.txt file.

# Upgrades
## Making new Upgrades
To make a new upgrade you need to make a new class that inherits from **Upgrade.h**. 
In the constructor of this class you need to set the **name** of the class as well as its **type**.
You will also need to decide on the **price** of your upgrade, also set the **starting price** to be the same as **price**.
A **description** is also needed . 
The naming convention we have chosen is to name it the same as the class itself. 
When it comes to types there are three of them: **PLAYER**, **RANGE** and **ENEMYSPECIFIC**.
**RANGE** is for when the upgrade has to go on projectiles, **PLAYER** on player/enemy and **ENEMYSPECIFIC** are only for enemies.
An example of an upgrade constructor:

```cpp
	UpgradeMeleeTest::UpgradeMeleeTest(Entity* parentEntity) : Upgrade(parentEntity)
	{
		SetName("UpgradeMeleeTest");
		SetType(F_UpgradeType::PLAYER);
		m_DamageChange = 2;
		m_Price = 5;
		m_StartingPrice = m_Price;
		m_Description = "Gives the player 2 extra damage at level 1. At subsequent levels the health increase will increase by 2 for each level. So level 1-5 vill be 2, 4, 6, 8, 10.";
	}
```

An uppgrade has many inherited functions such as **OnHit()**, **ApplyStat()** or **OnDamage()**.
It is by using these functions that you decide where/what your upgrade will affect. 
As an Example, take **UpgradeRangeTest**, which will have an immediate effect on player health in its **ApplyBoughtUpgrade()** function which calls on the **ApplyStat()** function.
Here we also increse the price since the upgrade has been bought. 
**UpgradeRangeTest** as well as making projectiles shoot upwards when hitting something in the function **OnRangeHit()**.

```cpp
	void UpgradeRangeTest::OnRangedHit()
	{
		m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationDirection(m_Direction);
		m_pParentEntity->GetComponent<component::AccelerationComponent>()->SetAccelerationSpeed(m_AccelerationSpeed);
	}

	void UpgradeRangeTest::ApplyStat()
	{
		if (m_pParentEntity->HasComponent<component::HealthComponent>())
		{
			m_pParentEntity->GetComponent<component::HealthComponent>()->ChangeHealth(m_HealthChange);
		}
	}

	void UpgradeRangeTest::ApplyBoughtUpgrade()
	{
		ApplyStat();
		m_Price = m_StartingPrice * m_Level; 
	}
```

If an upgrade is bought more than once its level should be increased in the function **IncreaseLevel()**.
It is in this function you define what will happen with each increase in level. 
Examples could be multiplying stat increases by level or maybe a switch case that adds functionallity for every level.
Here is an example from **UpgradeRangeTest** where the speed at which they are accelerating is multiplied by level. The health change you get will not increase but you will still get 100 more health for each level.
Price is also increased.

```cpp
	void UpgradeRangeTest::IncreaseLevel()
	{
		m_Level++;
		m_AccelerationSpeed = 1000 * m_Level;
		m_Price = m_StartingPrice * m_Level;
		ApplyStat();
	}
```

## UpgradeManager
When you have made your upgrade there is only two or three things left to do depending on if it is of type **RANGE** or not.
Firstly for all upgrades you will have to add an enum at the top of **UpgradeManager.h**. 
The naming convention for this is to use the same name as the class.

```cpp
	enum E_UpgradeIDs
	{
		UPGRADE_RANGE_TEST = 1,
		UPGRADE_MELEE_TEST = 2,
	};
```

After that add the upgrade to the list of all upgrades in **UpgradeManager**. This is done in the function **fillUpgradeMap()**.
Here is an example with the two test upgrades:

```cpp
	void UpgradeManager::fillUppgradeMap()
	{
		Upgrade* upgrade;

		// Adding RangeTest Upgrade
		upgrade = new UpgradeRangeTest(m_pParentEntity);
		// Set upgrade ID to the appropriate enum in E_UpgradeIDs
		upgrade->SetID(UPGRADE_RANGE_TEST);	
		// add the upgrade to the list of all upgrades
		m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
	

		// Adding MeleeTest Upgrade
		upgrade = new UpgradeMeleeTest(m_pParentEntity);
		// Set upgrade ID to the appropriate enum in E_UpgradeIDs
		upgrade->SetID(UPGRADE_MELEE_TEST);
		// add the upgrade to the list of all upgrades
		m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;

	}
```

As can be seen in the code, this is mostly a copy paste operation where the main change is which class you make a new instance of, 
as well as setting the enum as the **upgrade ID**.

Lastly you also have to add the upgrade to the switch case in the function called **newUpgrade**.
Here you only have to copy the previous cases and change the **enum** and **class**.

```cpp
	Upgrade* UpgradeManager::newUpgrade(std::string name, Entity* ent)
	{
		// Using the enum that is mapped to name,
		// return the correct NEW upgrade with parentEntity ent
		switch (m_AppliedUpgradeEnums[name])
		{
		case UPGRADE_RANGE_TEST:
			return new UpgradeRangeTest(ent);
			break;
		case UPGRADE_MELEE_TEST:
			return new UpgradeMeleeTest(ent);
			break;
		default:
			break;
		}
	}
```

# How to use heightmaps
Heightmaps are defined through a greyscale image which the program assumes uses **4 channels of color** that is RGB and opacity. The program will however only read the R channel to determine the height of a pixel on the map.

Like any other model, the heightmap also needs a material. Define this with an mtl file and appropriate textures (metallic, albedo, roughness and normal) in the DDS format.

## Defining the heightmap
To define a heightmap, write a file which points out a png file to define the actual heightmap and an mtl file to define material **IN THAT ORDER**. An example of this can be found in hm.hm (found in the folder *Vendor\\Resources\\Textures\\HeightMaps\\hm.hm*)

```
planet_surface_Height.png
ground.mtl
```

## Loading and using the heightmap
Once you have defined your heightmap it may be loaded into the program. Use the method **LoadHeightmap** from the assetloader and specify the path to the heightmap descriptive file (hm.hm). The heightmap may be transformed, so if you want to scale the plane or the heights, use the TransformComponent that the entity should have.

# How to load a level/map
The assetloader may load a scene with non moving entities. Each entity may have a modelcomponent (A transform is given on loading), a light component and a collisioncomponent.
The entities are loaded via a txt file, the following will be a description of the different commands that may be given.
## Entity related commands
To create an entity you need to give it a name. This name will be used to identify the entity.
```
Name tstEntityName
```
To add a component to the entity use the Submit command. Note however that all aspects of the component needs to be finished before it is finished. For example, the modelcomponent needs its position and a rotation (If you don't wish to use the default ones that is.)
```
Submit Model
Submit Heightmap
Submit PointLight
Submit SpotLight
Submit DirectionalLight
Submit CollisionSphere 
Submit CollisionCapsule
Submit CollisionCube
Submit CollisionHeightmap
```
## Model related commands
The model related commands are used to define a model.

**ModelPath** Sets the path to the model file. (OBJ for regular model and heightmap file for heightmaps). This path is relative to the textfiles position.
```
ModelPath Models/Cube/crate.obj
// the txt file is in this example located in Vendor/Resources.
```
**ModelScaling** Sets the scaling in x,y,z (localspace) axises for the model.
```
ModelScaling 1.0,1.0,1.0
```

**ModelRotation** Sets the rotation around the x,y,z axises for the model.
```
ModelRotation 1.0,0.0,0.0
```

**ModelPosition** Sets the position in x,y,z axises for the model.
```
ModelPosition 1.0,10.0,1.0
```
**ModelDrawFlag** Sets a draw flag for the model. The first argument says which flag should be used while the second describes if it should be used for the model (0 = don't use, 1 = use)
```
ModelDrawFlag 1,1
```

## Light related commands
The Light related commands are used to define lights that are attached to an entity with a model.

**ModelLightFlag** is used to define a flag for the light. The first argument says which flag should be used while the second describes if it should be used for the light (0 = don't use, 1 = use)
```
ModelLightFlag 0,1
```
**ModelLightColor** is used to define the color of the light in RGB values.
```
ModelLightColor 2.0,0.0,0.0
```
**ModelLightDirection** is used to define the direction of the light. It is not used for pointlights!
```
ModelLightDirection 0.0,-1.0,0.0
```
**ModelLightAttenuation** is used to define the attenuation of the light in RGB values.
```
ModelLightAttenuation 0.0,0.8,0.0
```
**ModelLightAspectRatio** Sets the aspect for shadow camera of the light.

**ModelLightCutOff** Sets the angle where the light starts to fade out (Spotlight).
```
ModelLightCutOff 30.0
```
**ModelLightOuterCutoff** Sets the angle where the light completely fades out (Spotlight).
```
ModelLightOuterCutOff 35.0
```
**ModelLightNear** sets the distance to the nearplane of the shadow camera (Directional light).
```
ModelLightNear -1000.0
```
**ModelLightFar** sets the distance to the farplane of the shadow camera (Directional light).
```
ModelLightFar 1000.0
```
**ModelLightLeft** sets limit for shadow camera to the Left (Directional light).
```
ModelLightLeft -355.0
```
**ModelLightRight** sets limit for shadow camera to the Right (Directional light).
```
ModelLightLeft 355.0
```
**ModelLightTop** sets limit for shadow camera to the Top (Directional light).
```
ModelLightTop 250.0
```
**ModelLightBottom** sets limit for shadow camera to the Bottom (Directional light).
```
ModelLightBottom -250.0
```
**ModelMass** Sets the mass of the collision object.
```
ModelMass 1.0
```
## Collision related commands
The collision related commands used to define the collision shape of an entity.

**ModelFriction** sets the friction of the collisioncomponent.
```
ModelFriction 1.0
```
**ModelRestitution** sets the restitution of the collisioncomponent.
```
ModelRestitution 1.0
```
**Submit CollisionSphere** this command, in difference to the other submit commands, takes in data for the sphere. In this case, a float for the radius.
```
Submit CollisionSphere 1.5
```
**Submit CollisionSphere** this command, in difference to the other submit commands, takes in data for the capsule. In this case, a float for the radius and a float for the cylinder height.
```
Submit CollisionCapsule
```
**Submit CollisionCube** this command, in difference to the other submit commands, takes in data for the capsule. In this case, three floats for width, height and depth/length.
```
Submit CollisionCube
```
**Submit CollisionHeightmap** this command, in difference to the other submit commands, needs a heightmap to have been loaded for the entity before it is called.
```
Submit CollisionHeightmap
```

## Navmesh
To create a NavMesh, use the command NavMesh. To add a NavQuad to the NavMesh, define its position and size, and then submit it, with the following commands.
```
NavQuadPosition 0.0, 0.0, 0.0
NavQuadSize 5.0, 5.0
Submit NavQuad
```
In a similar manner, to add a connection between two NavQuads use the following commands. The parameters for **NavConnectionQuads** are the id's of the NavQuads to be connected. These are defined by the order in which they have been added to the file. 
```
NavConnectionQuads 0, 1
Submit NavConnection
```
Finally, submit the NavMesh with the command **Submit NavMesh**. A complete command sequence could look like the following.
```
NavMesh
#0
NavQuadPosition 0.0, 0.0, 0.0
NavQuadSize 5.0, 5.0
Submit NavQuad

#1
NavQuadPosition 5.0, 0.0, 2.0
NavQuadSize 5.0, 5.0
Submit NavQuad

#2
NavQuadPosition 4.0, 0.0, -2.0
NavQuadSize 3.0, 3.0
Submit NavQuad

NavConnectionQuads 0, 1
Submit NavConnection

NavConnectionQuads 2, 0
Submit NavConnection

NavConnectionQuads 2, 1
Submit NavConnection

Submit NavMesh
```