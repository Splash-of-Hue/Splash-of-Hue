# Team08
# Game
### Team Name: Splash of Hue
### Game Name: Splash of Hue
### Team members: Sophia, Abrielle, Clair/Celeste, Spencer, Geetika

## Concept
Our game is a top-down puzzle based dungeon crawler where the player will find and collect colours that you will be able to emit and use to solve puzzles.
## Setting
A bioluminescent sea creature journeys up to the surface to return to the sun that it has heard legends of and believes it originates from. Along the way, this creature introduces colour and light to a world that has never experienced it before.
## Controls
WASD for up, left, down, right movement. Use left and right arrow keys for blue and red colours, and up/down to return to neutral. 
## How to Play
Move using WASD and enter new rooms by approaching the openings in walls. Collect a colour by walking up to and colliding with a colour orb. Once you have collected the corresponding colour orbs, change colour using left/right arrow keys and return to neutral with up/down. Collide with entities while your colour radius is on and see what happens! Use these interactions and your colour to solve puzzles and progress through the dungeon. 
## Gameplay
Change colour and walk through walls of same colour.
![Screenshot 2026-04-03 225004](https://github.students.cs.ubc.ca/user-attachments/assets/a87a9081-550c-49b3-8d66-57bed8f549b6)

Interact with and change the colour of creatures and entities around you, like the anglerfish and moss toggle below.
![Screenshot 2026-04-03 225022](https://github.students.cs.ubc.ca/user-attachments/assets/c8edf811-26b5-4c01-b23d-cedae995942f)

Interacting with creatures helps open up new pathways and progress further into the dungeon.
![Screenshot 2026-04-03 225039](https://github.students.cs.ubc.ca/user-attachments/assets/f7d9aab6-b930-4c7d-ab8c-7f6699784f0f)

Unlock new colours and use those colours to have get creatures to act differently, such as the sawfish charging and breaking boxes when red, and following player when blue. 
![Screenshot 2026-04-03 225147](https://github.students.cs.ubc.ca/user-attachments/assets/b00a5470-91e6-412f-b129-5ad3de7016c1)

## Milestone 4
## Milestone Engagement Criteria
- Discovery -> Player can discover new things by moving and exploring rooms, and collide with objects to see what happens.
- Challenge -> Player is stopped by obstacles, like the pistol shrimp, non-passable boxes and walls, dangerous sawfish, and must figure out ways to bypass obstacles and understand AI behaviours, such as sawfish following, charging and pistol shrimp rotation, to traverse the tutorial and enter the final room.
## Milestone Required Elements
- Memory Management: We used Visual Studio's diagnostic tools and Visual Studio's performance profiler and found our game uses about 500 MB of memory, but there were no instances of memory leaks or hoarding. 

- Realtime performance: We profiled the runtime of our systems, and found that the rendering and physics systems were runtime bottlenecks, where the rendering system took about 90% of the time, and the physics system took about 6%. We changed the physics system to use broad pass by checking for an AABB collision and only running mesh collision if that check was successful, which made slight improvements. We also attempted to resolve rendering slowdown by unloading entities to decrease the amount of entities that need to be rendered, but are still working on optimizing the rendering system further. 
## Milestone Creative Features
- [10] Precise collisions -> Mesh based collisions using seperability axis theorem implemented by Geetika
- [16] Swarm behaviour -> Minnows will swarm and flock to each other and move like a school of fish, implemented by Sophia  using BOIDS, who has also set it up so that simply changing the separation, cohesion, and alignment parameters in the code can vastly change the way the minnows swarm, allowing for different movement patterns. 
- [15] Advanced decision-making -> A* pathfinding implemented by Clair, and decision tree for sawfish and shrimp behaviours implemented by Sophia and Spencer.
- [19] Reloadability -> Loading and unloading levels using LDtk library implemented by Clair
- [26] Numerous sophisticated integrated assets -> Same art style for assets implemented by Clair, Abby and Sophia
## Milestone Screenshots
Minimap and colour menu UI added to tutorial, and updated
![Screenshot 2026-04-03 225004](https://github.students.cs.ubc.ca/user-attachments/assets/07c5c2c4-0bd2-41a5-9cd0-3ed584e1fd1a)
New dungeon with new entities, with updated assets and fixed behaviour
![Screenshot 2026-04-03 230352](https://github.students.cs.ubc.ca/user-attachments/assets/7a37c5f7-a1a3-4d38-bbbe-017baab417d4)
Improved dungeon design, assets and hints
![Screenshot 2026-04-03 225147](https://github.students.cs.ubc.ca/user-attachments/assets/d33b24e0-053b-4395-b269-4525c54e33cc)

# Docs

- [Test Plan](./doc/test-plan.pdf)
- [Game Proposal](./doc/proposal.pdf)

