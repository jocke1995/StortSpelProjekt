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
