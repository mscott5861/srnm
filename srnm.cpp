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
  wclear(filesWin);
  box(filesWin, 0, 0);

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

  update_panels();
  doupdate();
}

void handleRename(std::string newFilename, std::vector<std::string> files, WINDOW *filesWin)
{
  unsigned short numFiles = files.size(),
                 numLeadingZeroes = 1, 
                 filesIdx = 0;

  while ((numFiles = numFiles / 10) > 0)
  {
    numLeadingZeroes++;
  }
  
  for (std::vector<std::string>::iterator it = files.begin(); it != files.end(); it++)
  {
    std::string period = ".";
    std::string dash = "-";
    std::string extension = period.append((*it).substr((*it).find_last_of(".") + 1));
    std::string fileNumber = dash.append(std::to_string(filesIdx).append(extension));
    (*it) = newFilename;
    (*it) = (*it).append(fileNumber);
    filesIdx++;
  }

  
  printDirectory(files, filesWin);
}



int main(int argc, char* argv[])
{
  WINDOW *filesWin;
  PANEL *filesPanel;
  cbreak();

  unsigned short filesPanelY = 3,
                 filesPanelX = 0,
                 termHeight = 0,
                 termWidth = 0;

  char ch;

  initscr();
  getmaxyx(stdscr, termHeight, termWidth);
  filesWin = newwin(termHeight - filesPanelY, termWidth, filesPanelY, filesPanelX);

  std::vector<std::string> files = getFiles();
  std::string newFilename;
  
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
 
  filesPanel = new_panel(filesWin);
  printDirectory(files, filesWin);

  attron(A_BOLD);
  attron(COLOR_PAIR(1));

  std::string instructions = "(<ENTER> to rename, <CTRL+C> to quit)";
  mvprintw(0, (termWidth - instructions.length()), instructions.c_str());

  attroff(A_BOLD);
  attroff(COLOR_PAIR(1));

  mvprintw(0, 0, "Enter template filename: ");
   
  attron(A_BOLD);
  attron(COLOR_PAIR(1));
  
  while ((ch = getch()) != KEY_F(1))
  {
    newFilename += ch;
    handleRename(newFilename, files, filesWin);
  }

  endwin();
  return 0;
}
