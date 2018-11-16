#include <stdlib.h>
#include <dirent.h>
#include <curses.h>
#include <panel.h>

#include <string>
#include <vector>
#include <iostream>

std::vector<std::string> getFiles() 
{
  DIR *directory;
  struct dirent *dir;
  unsigned short numFiles = 0;
  std::vector<std::string> files;

  directory = opendir(".");

  if (directory)
  {
    while ((dir = readdir(directory)) != NULL && numFiles < 30) 
    {
      std::string filename = dir->d_name;

      if (filename.compare(".") != 0 && filename.compare("..") != 0) {
        files.push_back(filename);
        numFiles++;
      }
    }

  closedir(directory);
  }

  return files;
}


void printDirectory(std::vector<std::string> files, WINDOW *filesWin)
{
  unsigned short padding = 2,
                 filesIdx = 0,
                 filesX = 3,
                 filesY = padding,
                 winHeight = 0,
                 winWidth = 0;

  getmaxyx(stdscr, winHeight, winWidth);

  for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
    {
      if (filesIdx > 0 && filesIdx % (winHeight - 7) == 0)
      {
        filesX += 20;
        filesY = padding;
      }

      mvwprintw(filesWin, filesY, filesX, (*it).c_str());
      refresh();
      filesY++;
      filesIdx++;
    }
}

int main(int argc, char* argv[])
{
  WINDOW *filesWin;
  PANEL *filesPanel;

  unsigned short filesPanelY = 3,
                 filesPanelX = 0,
                 termHeight = 0,
                 termWidth = 0;

  initscr();
  getmaxyx(stdscr, termHeight, termWidth);
  filesWin = newwin(termHeight - filesPanelY, termWidth, filesPanelY, filesPanelX);
  box(filesWin, 0, 0);

  std::vector<std::string> files = getFiles();

  printDirectory(files, filesWin);
  
  mvprintw(0, 0, "Enter template filename: ");
  filesPanel = new_panel(filesWin);
  update_panels();
  doupdate();
  getch();
  endwin();

  return(0);
}
