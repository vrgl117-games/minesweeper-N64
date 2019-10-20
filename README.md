# minesweeper-N64
*This code was used for the panel N64 HOMEBREW DEVELOPMENT presented at PGRE2019, here are the slides: https://vrgl117.games/PGRE2019*

[Part1](https://github.com/vrgl117-games/minesweeper-N64/tree/init) is the skeleton of the game, the code compiles and produce a ROM but does nothing.

[Part2](https://github.com/vrgl117-games/minesweeper-N64/tree/colors) add the basics of the game and used colors to display the board.

[Part3](https://github.com/vrgl117-games/minesweeper-N64/tree/pngs) add some more features (like flags) to the game and uses sprites (pngs) to display the board.


## Screenshots

<div><img src="./screenshots/minesweeper1.png?raw=true" width="400px">
<img src="./screenshots/minesweeper2.png?raw=true" width="400px"></div>
<div><img src="./screenshots/minesweeper3.png?raw=true" width="400px">
<img src="./screenshots/minesweeper4.png?raw=true" width="400px"></div>

## Note

This ROM file has been tested to work on real Nintendo 64 hardware using the
[EverDrive-64 by krikzz](http://krikzz.com/). It should also work with
[64drive by retroactive](http://64drive.retroactive.be/).

This ROM file is only known to work on low-level, high-accuracy Nintendo 64
emulators such as [CEN64](https://cen64.com/) or [MAME](http://mamedev.org/)
due to the use of [libdragon](https://dragonminded.com/n64dev/libdragon/)
instead of the proprietary SDK used by official licensed Nintendo software.

## Building

### Using Docker

All development can be done using Docker. It's the easiest way to build the ROM on Windows and MacOS.

 * Install [Docker](https://docker.com)
 * Run `make` to produce the `minesweeper-64.z64` ROM file.

 ### On linux

* Install [libdragon development toolchain](https://github.com/DragonMinded/libdragon)
* Run `make minesweeper-64.z64` to produce the `minesweeper-64.z64` ROM file.

## Thanks

* Thanks to [Jennifer Taylor](https://github.com/DragonMinded) for libdragon.
* Thanks to [conker64](https://github.com/conker64) and the [#n64dev](irc://irc.efnet.org/#n64dev) IRC community for their help.
* Thanks to [Christopher Bonhage](https://github.com/meeq) for his work on [FlappyBird-N64](https://github.com/meeq/FlappyBird-N64).
