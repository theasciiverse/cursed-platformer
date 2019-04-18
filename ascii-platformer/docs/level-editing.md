# Guide to Level Editing

## Level structure

Levels in the game are represented as a folder in the data directory. To create your own level, simply
create a folder in the data directory, for example, "data/my-level". The directory's name will be used
to identify the level in the game and may not have spaces or special characters.

Within the folder you need to make two files: "map.txt" and "metadata.txt". 

## map.txt: Level Geometry

The map.txt file contains the ASCII representation of the static elements in the map, essentially the level 
geometry. Most ASCII characters are purely decorative, but there are a couple of characters that have a special 
role.

* '#' represents a wall or a floor, it is impassable from all directions
* '"' represents a platform or a ladder that can be climbed from below but you cannot fall through
* '^' is a static spike, falling on it will be deadly and it cannot be passed from either side
* ']' and '[' are "doors" but act essentially like wall tiles

You can then "draw" the map using your favourite text editor into the map.txt file. What you write into
the file will look the same in game. Note that the game cannot display maps larger than 80x24 but your
levels can be smaller than that.

## metadata.txt: Level Pre-Processor

The metadata file contains instructions for what we call the level pre-processor. These instructions will
inform the game about the size of the map, where starting and goal locations should be and what entities to
place on the level and where. The instructions, their arguments and their function are listed below.

Each instruction should be placed on its own line. The strings should not contain special characters.

### title <string>

Set the level's in-game title. This may differ from the folder name.

### size <width> <height>

Sets the level's size. This instruction is mandatory, since this information is used to load the level geometry.

### start <x> <y>

Sets the X,Y coordinate of the level's starting position. When the level is loaded, the player will be placed
here.

### goal <x> <y> <level-name>

Sets the location of the goal on the level. When the player steps into this coordinate, they will be moved
to the specified level. NOTE: the level name must match one of the folders in the data/ directory.

### muncher <x> <y>

Adds a muncher creature to the specified X,Y coordinate.

### spike <x> <y>

Adds a falling spike to the specified X,Y coordinate.

### bat <x> <y>

Adds a bat creature to the specified X,Y coordinate

### message <x> <y> <read> <who> <message>

Adds a message entity to the level in the specified coordinates. The <read> argument indicates if the message
has been read, <who> is a string of who is saying the message (for story-telling reasons) and <message> is the 
actual message content. When player steps on the message and <read> is 0 the message will be drawn on the screen.

### button <x> <y> <tx> <ty>

Adds a button entity to the level at the specified coordinate. When stepped on, the block specified by the <tx>
and <ty> coordinates will be turned into air. Useful for opening paths.

### teleport <x> <y> <tx> <ty>

Adds a teleporter at the specified coordinates that will transport the player to the TX,TY coordinate when
stepped on.

### platform <x> <y> <tx> <ty> <width>

Adds a moving platform between X,Y and TX,TY that is <width> tiles wide. Either the X and TX or the Y and TY
coordinates should match. Platforms cannot move diagonally.

### trampoline <x> <y> <width>

Adds a trampoline to the level at the specified coordinates. The trampoline will be <width> tiles wide.

### // <comment>

Ignores the rest of the line. Can be used to add comments to the metadata for reminders or to organize
the metadata or to temporarily remove an entity from the level. NOTE: the // should be immediately followed
by a space.

The file can be ended with an empty line or a line that says "end". This is simply to ensure the parser
doesn't repeat the last command at the end of the file.
