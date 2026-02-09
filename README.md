# Team08
# Game
Team Name: Splash of Hue
Game Name: Splash of Hue
Team members: Sophia, Abrielle, Clair/Celeste, Spencer, Geetika

Concept: Our game is a top-down puzzle based dungeon crawler where the player will find and collect colours that you will be able to emit and use to solve puzzles.
Setting: A bioluminescent sea creature journeys up to the surface to return to the sun that it has heard legends of and believes it originates from. Along the way, this creature introduces colour and light to a world that has never experienced it before.
Controls: WASD for up, left, down, right movement. E to open up colour menu. Use arrow keys when colour menu pops up to toggle between different colours. Move and collide with other objects to change their colours. 

# Milestone 1
Core Game Mechanics: Basic input driven responses via keyboard (WASD, E and arrow keys) for movement and colour menu. Basic collision detection is between axis-aligned bounding boxes. Event driven responses: collision between player and certain entities results in different behaviours. For instance, colliding with a circular toggle of a different colour will change both the toggles colour and walls associated with it. Colliding with walls of the same colour will allow the player to pass through them, whereas walls of different colour are impassable. Colliding with sawfish entities when the player is in colour state red will cause the sawfish to follow the player around. 

Milestone Required Elements: 
Textured Geometry -> rendering rectangle and circle shapes with associated sprite assets.
2D transformations -> translation of position of entities. 
Key-frame/State interpolation -> AI pathfinding behaviour (sawfish pathfinds player when collided with). Implemented by getting vector distance between player and sawfish, 

Milestone Creative Features:
[1] (basic) Colour change upon collision -> sprite rendering of shapes and colour change of sprites when collision is detected (using fragment shaders) implemented by Clair and Abby. 
[8] (basic) Basic physics -> Inelastic collisions using axis-aligned bounding boxes implemented by Geetika.
[10] (advanced) Precise collisions -> Collisions between two different shapes (circle and rectangle) implemented by Spencer. 
[14] (basic) Simple Pathfinding -> distance between two entities along vector and moving sawfish along vector towards player implemented by Sophia.
[23] (basic) Audio feedback -> background music implemented by Spencer. 
[24] (basic) Basic integrated assets -> sprite assets and rendering implemented by Clair and Abby.

Milestone Screenshots:
The following screenshot is of a maze that the player can navigate by changing colour and colliding with the circular toggles to change the colours of walls so that the player can pass through walls of the same colour. 
![maze](https://github.students.cs.ubc.ca/user-attachments/assets/39fb26c5-1811-43e7-991e-0db038571c11)

# Docs

- [Test Plan](./doc/proposal.pdf)
- [Game Proposal](./doc/proposal.pdf)

