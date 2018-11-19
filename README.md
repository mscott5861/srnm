# srnm

A deliberately simple ncurses utility that sequentially (and dumbly) renames the files in the directory it's invoked from to whichever string is entered. Useful for bulk renaming things like wallpapers, photos, etc., with basically no learning curve.


## Requirements
### Dependencies

* `g++`
* `libncurses5-dev`
* `libncursesw5-dev`

## Installation

### Debian

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
git clone https://github.com/mscott5861/srnm.git
cd srnm
g++ srnm.cpp -lncurses -lpanel -o srnm
sudo ln -s #full-path-of-compiled-srnm#/srnm /usr/bin/srnm
```


## Usage

`cd` to the directory whose files you'd like to rename, and just run `srnm`. Type in the new filename, press `Enter` to rename the files (followed by a confirmation), or `Ctrl+C` to exit the program.
