# How To...
This is a document to help understand existing systems and what is required to get something working or to use their API.


## How to create a new animation

### 1. Adding Sprites/Spritesheets
Export any spritesheets as `.png` files and include them in the `assets/textures/` folder, taking note of the filenames.

### 2. Import the files into code
In the [`render_constants.hpp file`](../src/renderer/render_constants.hpp), add a new value in the `TEXTURE_ASSET_ID` enum for each new file.

In the [`render_system.hpp`](../src/renderer/render_system.hpp) file, add the path to the new file under the `texture_paths` constant.
		
> [!NOTE]
> For example, if you add a new file `player_idle_01.png`, you may want to add a value `PLAYER_IDLE_01` to the `TEXTURE_ASSET_ID`,
> and `textures_path("player_idle_01.png")` to `texture_paths`.

### 3. Update any animation types and triggers
A single entity may have multiple animations but only one animation type. Each different animation needs a trigger. For many animations,
triggers (`ANIMATION_TRIGGER`) can be shared as they will be unique to each animation type (`ANIMATABLE_TYPE`).
These are both in the [`animation_constants.hpp`](../src/animations/animation_constants.hpp) file.

For example, the animation type might be `PLAYER` and you need to add a new animation `FLIP_HAPPY`.

### 4. Define the animation
In the [`animation_system_init.hpp`](../src/animations/animation_system_init.hpp) file, define a new animation by using the type, trigger,
and files. For each file, the portion of the file should be defined in terms of percentages.

It will look something like this:

```
// Using multiple files
generate_animation(
	ANIMATABLE_TYPE::PLAYER,
	ANIMATION_TRIGGER::IDLE,
	{
		{ TEXTURE_ASSET_ID::PLAYER_IDLE_01, RenderSourceRect()},
		{ TEXTURE_ASSET_ID::PLAYER_IDLE_02, RenderSourceRect()},
		{ TEXTURE_ASSET_ID::PLAYER_IDLE_03, RenderSourceRect() }
	}
);
// Using a sprite sheet where the sheet is 256x256 and each frame is 64x128
generate_animation(
	ANIMATABLE_TYPE::PLAYER,
	ANIMATION_TRIGGER::IDLE,
	{
		{ TEXTURE_ASSET_ID::PLAYER_IDLE, RenderSourceRect { vec2(0), vec2(0.25, 0.5) } },
		{ TEXTURE_ASSET_ID::PLAYER_IDLE, RenderSourceRect { vec2(0.25, 0), vec2(0.25, 0.5) } },
		{ TEXTURE_ASSET_ID::PLAYER_IDLE, RenderSourceRect { vec2(0.5,  0), vec2(0.25, 0.5) } }
	}
);
```

This creates an idle animation for the player using 3 images (or textures).

> [!WARNING]
> There is a current limitation where the system does not support spritesheets, as each image/texture is a seperate frame.
> This is to be resolved in
> [Task B5PfBTKS](https://trello.com/c/B5PfBTKS/32-2-extend-the-animation-system-to-use-spritesheets-instead-of-seperate-sprite-fileshttps://trello.com/c/B5PfBTKS/32-2-extend-the-animation-system-to-use-spritesheets-instead-of-seperate-sprite-files).

## How to change or add new controls

As a level of abstraction, the `controls/` folder takes everything from `input/` and turns it into what the other systems listen for.
This has the added benefit that adding new controls is much easier and involves adding only a few extra lines.

### Defining new input

Regardless of if you want to add input for, the GLFW library has constants that are available such as `GLFW_KEY_W` on keyboard
or `GLFW_GAMEPAD_BUTTON_Y` on gamepad.

If a new input needs to be added for a joystick, these can be defined in `inputs/input_constants.hpp`.

### Converting inputs to controls

Controls can be defined and added in `controls/control_constants.hpp` and then mapped for each input type.
Each map can have up to 2 `float`s that can be defined in any way that is most helpful. They can also be `int`s
but will be passed as `float`s and must be cast back to `int`.

This might look like the below:

```
// inputs/input_constants.hpp
#define SOH_JOYSTICK_BUTTON_X 2

// controls/control_constants.hpp
enum class CONTROL_TYPE
{
	...
	NEW_CONTROL_TYPE,
	...
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> keyboard_control_map = {
	...
	{ GLFW_KEY_H, { CONTROL_TYPE::NEW_CONTROL_TYPE, {  0.0f, 1.0f } } },
	...
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> joystick_control_map = {
	...
	{ SOH_JOYSTICK_BUTTON_X, { CONTROL_TYPE::NEW_CONTROL_TYPE, { 1, 1 } } },
	...
};

const std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>> gamepad_control_map = {
	...
	{ GLFW_GAMEPAD_BUTTON_X, { CONTROL_TYPE::NEW_CONTROL_TYPE, { 1.0f, 1.0f } } },
	...
};
```

### Input modifiers

For mouse and keyboard, there are also modifier maps.

```
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> keyboard_modified_control_map;
const std::map<int, std::map<int, std::pair<CONTROL_TYPE, std::vector<float>>>> mouse_modified_control_map;
```

These maps can be used for additional controls that involve using the `ctrl`, `alt`, or `shift` keys.

### Using controls

Once the controls are defined, you can use them by listening to `registry.controls`. This container is cleared at the start of each loop.
Controls have phases such as `CONTROL_PHASE::START` which can be checked. This might look like this:

```
for (Control& control : registry.controls.components)
{
	if (control.type == CONTROL_TYPE:: NEW_CONTROL_TYPE && control.phase == CONTROL_PHASE::START)
	{
		// Do something
	}
}
```

## How to add a new UI menu

Adding a new menu is as easy as creating a class that inherits from the `Menu` interface such as how the `PauseMenu` does. Add new menus to `/src/ui/menus/`.

```
class PauseMenu : public Menu
{
   ...
};
```

Then you need to add that menu to the `UiSystem`'s constructor, like below:

```
// src/ui/ui_system.hpp
public:
	UiSystem()
	{
		menu_references[MENU_TYPE::PAUSE_MENU] = &pause_menu;
		...
	}
```

Each menu is responsible for handling its construction and deconstruction, as well as handling inputs with the `confirm`, `cancel`, and `move` functions.
Since menus might be quite different from each other, these might do different things on each menu.
