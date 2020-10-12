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

## Upgrades
### Making new Upgrades
To make a new upgrade you need to make a new class that inherites from **Upgrade.h**. 
In the constructor of this class you need to set the **name** of the class as well as its **type**. 
The namingconvention we have chosen is to name it the same as the class itself. 
When it comes to types there are three of them. **PLAYER**, **RANGE** and **ENEMYSPECIFIC**.
**RANGE** is for when the upgrade has to go on projectiles, **PLAYER** on player/enemy and **ENEMYSPECIFIC** are only for enemies.
An example of an upgrade constructor:

´´´cpp
	UpgradeMeleeTest::UpgradeMeleeTest(Entity* parentEntity) : Upgrade(parentEntity)
	{
		SetName("UpgradeMeleeTest");
		SetType(F_UpgradeType::PLAYER);
		m_DamageChange = 2;
	}
´´´

An uppgrade has many inherited functions such as OnHit(), ApplyStat() or OnDamage().
It is using these functions that you decide where/what your upgrade will affect. 
As an Example take UpgradeRangeTest which will have an emmidiate effect on player health in its ApplyStat() function,
as well as making projectiles shot upwards when hitting something in the function OnRangeHit().

´´´cpp
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
´´´

If an upgrade is bought more than once its level should increase in the function **IncreaseLevel()**.
It is in this function you define what will happen with each increase in level. 
Examples could be multiplying stat increases by level or maybe a switch case that adds functionallity for every level.
Here is an example from UpgradeRangeTest where the speed at wich they are accelerating is mutiplied by level. The health change you get will not increase but you will still get 100 more health.

´´´cpp
	void UpgradeRangeTest::IncreaseLevel()
	{
		m_Level++;
		m_AccelerationSpeed = 1000 * m_Level;
		ApplyStat();
	}
´´´

### UpgradeManager
When you have made your uppgrade there is only one or three things left to do depening on if it is of type **RANGE** or not.
Firstly you need to add the upgrade to the list of all upgrade in **UpgradeHandler**. This is done in the function **fillUpgradeMap()**.
Here is an example with the two test upgrades:

´´´cpp
	void UpgradeManager::fillUppgradeMap()
	{
		Upgrade* upgrade;

		// Adding RangeTest Upgrade
		upgrade = new UpgradeRangeTest(m_pParentEntity);
		// add the upgrade to the list of all upgrades
		m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
		// Also, since it is of type RANGE, add its' Enum to the enum map.
		m_RangeUpgradeEnmus[upgrade->GetName()] = UPGRADE_RANGE_TEST;		

		// Adding MeleeTest Upgrade
		upgrade = new UpgradeMeleeTest(m_pParentEntity);
		// add the upgrade to the list of all upgrades
		m_AllAvailableUpgrades[upgrade->GetName()] = upgrade;
	}
´´´

As can be seen in the code, this is mostly a copy paste operation where the main change is which class you make a new instance of.
Notice that UpgradeRangeTest has to add an extra enum to a map. This is because it is of type **RANGE**.
For these types of upgrades you will have to make add en enum at the top of **UpgradeHandler.h**. 
The naming convention for this is to use the same name as the class.
´´´cpp
	enum E_RangeName
	{
		UPGRADE_RANGE_TEST = 1,
	};
´´´
Lastly for **Range** upgrades you also have to add the upgrade to the switch case in the function called **RangeUpgrade**.
Here you only have to copy the previouse cases and change the enum and class.

´´´cpp
	Upgrade* UpgradeManager::RangeUpgrade(std::string name, Entity* ent)
	{
		// Using the enum that is mapped to name,
		// return the correct NEW range upgrade with parentEntity ent
		switch (m_RangeUpgradeEnmus[name])
		{
		case UPGRADE_RANGE_TEST:
			return new UpgradeRangeTest(ent);
			break;
		default:
			break;
		}
	}
´´´

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
