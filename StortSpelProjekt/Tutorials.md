# Tutorials

## TO CREATE AN EVENT:

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

## TO USE INPUT:

To use input by the keys W, A, S, D, Q and E, subscribe to the event type MovementInput.
To use input by the left CTRL key, subscribe to the event type ModifierInput. The parameters
are the SCAN_CODE of the key and whether it has just been pressed or released.

To use input from the mouse buttons, subscribe to the event type MouseClick. The parameters
are the MOUSE_BUTTON code of the button and whether it has just been pressed or released.

To use input from mouse movement, subscribe to the event type MouseMovement. The parameters
are the x and y coordinates of the mouse's movement.

To use input from the mouse scroll, subscribe to the event type MouseScroll. The parameters
are either 1 if scrolling forward or -1 if scrolling backwards.

### TO CREATE NEW INPUT:
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


### PBR TEXTURES:
In a .mtl file the textures should be placed as follows.

map_Kd "Albedo_texture.suffix" (albedo/diffuse)
map_Ks "Roughness_texture.suffix" (roughness/specular)
map_Ka "Metalness_texture.suffix" (metalness/ambient)
map_Kn "Normals_texture.suffix" (normals)
map_Ke "Emissive_texture.suffix" (emissive)

### DDS Texture Extension
If you want to reduce aliasing at large distances, you should convert your textures to the .dds image format.

In the following link you can download a .dds-converter which converts a texture from a given format into the .dds format.
[a link]https://vvvv.org/contribution/texconvgui

## Settings in the texConvGui-tool
You wont need to change every setting in the tool, only the ones mentioned below:

# Folders
In the program, you will have to enter the source & destination folders for the textures

# File Mask
Set the "File Mask" to the format of the textures you want to convert from. 
If you want to convert from jpg to dds, set the "File Mask" to .jpg

# Resizing
In order for the creation of mipmaps to work, the texture need to be in "common sizes" such as 512x512, 1024x1024, 2048x2048.
If your texture is close to either one of them, (for example 500x500), you can use the "resizing" option to resize the texture to 512x512.
Set to 0 if you dont want to resize the texture.

# Adressing Mode
Set this to wrap

# Output Options
Set this to B8G8R8A8_UNORM

# Mip Map Level
Set this to 0 to generate mipmaps

## Object File modification
When you created your new textures, you will have to modify how your object file reads the new texture (depending on object file format)

# .Obj-file modifications (changing the .mtl file)
This is the file format which will be most commonly used in our project, and all you need to do is change the file extensions of the textures to .dds from whatever file extension it had earlier

# Other
If you wish to use other object formats were the textures are embedded, you will have to change them using a extern program such as "Blender" or "Maya".
