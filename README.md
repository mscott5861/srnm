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
sudo ln -s /usr/bin/srnm ##full-path-of-compiled-srnm##/srnm 
```


## Usage

`cd` to the directory whose files you'd like to rename, and just run `srnm`. From within the program, start typing a new filename, and `srnm` will sequentially rename the files in the directory. Key presses and combinations are few:

|Key(s)|Effect|
|------|------|
|<Enter> |Commit the renames to disk|
|<Ctrl+D>|Toggle directories|
|<Ctrl+C>|Quit `srnm`|
