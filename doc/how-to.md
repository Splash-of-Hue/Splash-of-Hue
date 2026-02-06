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
and files. It will look something like this:

```
	generate_animation(
		ANIMATABLE_TYPE::PLAYER,
		ANIMATION_TRIGGER::IDLE,
		{ TEXTURE_ASSET_ID::PLAYER_IDLE_01, TEXTURE_ASSET_ID::PLAYER_IDLE_02, TEXTURE_ASSET_ID::PLAYER_IDLE_03 }
	);
```

This creates an idle animation for the player using 3 images (or textures).

> [!WARNING]
> There is a current limitation where the system does not support spritesheets, as each image/texture is a seperate frame.
> This is to be resolved in
> [Task B5PfBTKS](https://trello.com/c/B5PfBTKS/32-2-extend-the-animation-system-to-use-spritesheets-instead-of-seperate-sprite-fileshttps://trello.com/c/B5PfBTKS/32-2-extend-the-animation-system-to-use-spritesheets-instead-of-seperate-sprite-files).
