# metaData - NG++ tool
**metaData** is a *New Game +* (NG+) hack tool written in **C\+\+17** (hence the NG++ name).
It has been written just for the fun of making something like this, and to get some very cool games to even new possibilities.

You can safely see some videos at [this YouTube channel](https://www.youtube.com/channel/UCFNSWeOer5wZQH19ODpvCyA).

## Instructions
### Requeriments
* Visual C++17 x64 compiler
* Add the executable as an exception in the configuration of your antivirus (due to the nature of this software, is targeted as a potential harmful program)

### Games avaliables
All this games need to be played from Steam. Other binaries will be of no use with this software. This is **NOT** an exploit from Steam's software, instead, it's more like adversiting, because that is the platform I use to actually play the games I love :-)
Keep in mind that if the developers get upset with this, they may release a new build and the hacks will be version incompatible (and I will not start a nonsense race reverse engineering and updating this software).
The list so far:
* **Dishonored 2**
* **Mark of the Ninja**
* **Resident Evil 2 Remake**

### Procedure
1. Run the game from Steam
2. Run **metaData** from the command line to target a specific game
	1. If you run **metaData** without parameters, it will show you a usage message

Example
```
> metaData.exe ninja
```

3. Use the keys mapped to different hacks while you play the game
	1. The keys mapping are configured in the file named *keyMapping.cfg*
	2. You may change the mapping as you see fit, and if you screw the configuration file, you will be able to create the default configuration file again with this command:

```
> metaData.exe default
```

These are the actions programmed for the Mark of the Ninja game:

```
NINJA_INSTANT_CAST_OFF                                      X
NINJA_INSTANT_CAST_ON                                       C
NINJA_INFINITE_ITEMS_OFF                                    Z
NINJA_INFINITE_ITEMS_ON                                     V
```

While your playing the game, if you press *C* key, the **metaData** console will give you a message showing that casting a trick won't have any cooldown (you may use 2 screens, play the game in window mode, or just ignore the **metaData** console and keep playing the game in fullscreen mode). If you want to disable the cooldown hack, you may press *X* while in game and the delay between tricks executions will return to normal.
Due to the fact that this software hooks low level API keyboard calls, you don't have to switch focus to enable/disable hacks. This allows infinite possibilites to explore!
Be aware that in Resident Evil 2 the *Full Damage* hack will also kill you instantly if you get a single point of damage!
Read the *keyMapping.cfg* file to learn all the hacks implemented.

## Final note
Nothing in this software was meant to damage anything, nor give anyone wrong ideas.