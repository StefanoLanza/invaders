First, this has been the most fun test I've done so far.

Time Spent
About 24 hours, divided as
60% coding
30% game play tweaking
10% googling

Game Objects

The initial code had a GameObject base class and a runtime type mechanism based on strings. This design was common in old engines and has been replaced by component based architectures. For this game, I replaced the existing code with individual classes for each object type (PlayerShip, Alien, etc.) Each game object in then stored in a corresponding vector container in the PlayField class. No runtime dispatch mechanism is thus required as all types are known at compilation time.
Objects contain a position, a collision area, a visual component (position, sprite, color) and other variables required by the gameplay rules.

Vector2D
The game does not need a fully featured Vector class with operators, methods etc. I simplified it to a struct containing the x,y coordinates of an object.

Rendering
Obviously std::cout is not suitable for efficient text mode rendering. My last time doing console programming was probably 20 years ago but I quickly found all win32 API methods I needed. I removed double buffering as it's not necessary with WriteConsoleOutput. I added methods to draw the game state, scores, etc.

Input
I added a KeyboardInput class for handling input from the keyboard. The class uses the GetAsyncKeyState function to query the current state of a key.
I added a enum with a set of keys to the header file in order not to introduce a dependency with the win32 API (window.h header).

Game Play
The game play runs at a fixed time step: 30 Hz. The game loop sleeps each iteration after simulation and rendering to keep the CPU free.
Game objects move and update their state based on the time step. The speed of objects on screen is thus frame independent. The gameplay is NOT.
The PlayField class updates the gameplay in four phases:
1) Movement
2) Update: aliens and players shoot lasers and update the internal state
3) Collision detection: pairs of objects are checked for collision and events triggered according to the rules.
4) Deletion of dead objects:

Collisions
Each game object has a rectangular collision area defined by the current and previous position. Collision detection relies on an axis aligned rectangle vs rectangle intersection function. This method is good enough for this game, but not accurate for oblique trajectories. For example, 45 degree lasers report false positives. A little help for the player :)
Also, game objects use floating-point coordinates for movements and collision detection, and integer coordinates for display. This can cause a visual discrepancy between collisions and the position of objects and explosion on the console. It's not that noticeable anyway.

Misc
The code uses standard smart pointers and containers for managing the lifetime of various objects.

Extensions
* I added multiplayer to the game. The second player can be controlled by the keyboard (A, D keys) or the CPU.
* I added a invulnerability power-up which lasts n seconds. The player flashes color while he's invulnerable.
* I added a message log to display encouraging messages when aliens are spawned and when the player collects a power up
* I added pause / restart functionalities 
* I added aliens vs walls collision detection. When aliens hit a wall, they change trajectory to avoid it.

Future Work
- More power ups (bombs!), more alien types, better AI, animations, sounds etc. 
- Multi-sprite objects.
- Accurate collision detection
Also the game itself could be structured in levels and have a score board. 
On the practical side, one of the most important features currently missing is loading/saving and runtime editing of the game configuration. A scripting language like Lua could be used for that and quickly integrated.
