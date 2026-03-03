# Team08
# Game
### Team Name: Splash of Hue
### Game Name: Splash of Hue
### Team members: Sophia, Abrielle, Clair/Celeste, Spencer, Geetika

## Concept
Our game is a top-down puzzle based dungeon crawler where the player will find and collect colours that you will be able to emit and use to solve puzzles.
## Setting
A bioluminescent sea sheep, born different from the rest of its herd, journeys up to the surface to return to the sun that it has heard legends of and believes it originates from. Along the way, the sheep introduces colour and light to a world that has never experienced it before. However, its mother is determined to stop the sheep's journey, believing she knows whats best for it, and sends obstacles to stop the sheep from continuing its journey. 
## Controls
WASD for up, left, down, right movement. Left arrow key for blue, right arrow key for red, up/down to deactivate colours. 

# Milestone 2
## Milestone Engagement Criteria
- Discovery -> Player can discover new things by moving and exploring rooms, and collide with objects to see what happens. 
- Challenge -> Player is stopped by obstacles, like the pistol shrimp, non-passable boxes and walls, dangerous sawfish, and must figure out ways to bypass obstacles and understand AI behaviours, such as sawfish following, charging and pistol shrimp rotation, to traverse the tutorial and enter the final room. 

## Milestone Required Elements
- Game AI: Implemented state and decision tree for sawfish and shrimp behaviours that depends on colour state and colour interactions. 
- Mesh-based Collision: Implemented mesh-based collision detection and resolution using the seperating axis theorem, player's mesh has been defined to be a triangle. 
- Tutorial: Game progressively introduces new mechanics, and forces player to use previously learned mechanics to 
- Animation: Player, shrimp, sawfish and moss animation implemented. 
- Assets: Original sprite and background assets, as well as original music. 
- FPS Counter: FPS counter in window caption. 

## Milestone Creative Features 
- [10] Precise collisions -> Mesh based collisions using seperability axis theorem implemented by Geetika
- [15] Advanced decision-making -> A* pathfinding implemented by Clair, and decision tree for sawfish and shrimp behaviours implemented by Sophia and Spencer. 
- [19] Reloadability -> Loading and unloading levels using LDtk library implemented by Clair
- [26] Numerous sophisticated integrated assets -> Same art style for assets implemented by Clair and Abby
## Milestone Screenshots
In the following screenshots, the integrated assets, activated colour radius, room loading, and different behaviours are demonstrated. 
- Shrimp in neutral state.
![Screenshot 2026-03-02 225627](https://github.students.cs.ubc.ca/user-attachments/assets/01f51942-66f6-41ca-a607-09d34421a53c)
- Shrimp rotated in blue state
![Screenshot 2026-03-02 225659](https://github.students.cs.ubc.ca/user-attachments/assets/aede040b-7c72-4b09-aeaf-d7aa73b6eced)
- Sawfish following player when blue. 
![Screenshot 2026-03-02 225736](https://github.students.cs.ubc.ca/user-attachments/assets/d943e95e-671a-4018-a133-268cc36a5946)

# Docs
- [Test Plan](./doc/test-plan.pdf)
- [Game Proposal](./doc/proposal.pdf)

