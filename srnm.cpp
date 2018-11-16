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
  werase(filesWin);
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

void handleAlphanumericKeypress(std::string newFilename, std::vector<std::string> newFiles, std::vector<std::string> files, WINDOW *filesWin)
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
  move(0, (25 + newFilename.length()));
}

void writeRenameToDisk(std::vector<std::string> newFiles, std::vector<std::string> files)
{

}

void printInstructions(unsigned short *termWidth)
{
  attron(A_BOLD);
  attron(COLOR_PAIR(1));

  std::string instructions = "(<ENTER> to rename, <CTRL+C> to quit)";
  mvprintw(0, (*termWidth - instructions.length()), instructions.c_str());

  attroff(A_BOLD);
  attroff(COLOR_PAIR(1));
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

  int ch;

  initscr();
  getmaxyx(stdscr, termHeight, termWidth);
  filesWin = newwin(termHeight - filesPanelY, termWidth, filesPanelY, filesPanelX);

  std::vector<std::string> files = getFiles();
  std::vector<std::string> newFiles;
  std::string newFilename;
  
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
 
  filesPanel = new_panel(filesWin);
  printDirectory(files, filesWin);
  printInstructions(&termWidth);

  mvprintw(0, 0, "Enter template filename: ");
   
  attron(A_BOLD);
  attron(COLOR_PAIR(1));

  noecho();
  
  while ((ch = getch()) != KEY_F(1))
  {
    noecho();
    switch(ch)
    {
      case 127:
        if (newFilename.length() > 0) {
          newFilename.pop_back();

          move(0, 25);
          clrtoeol();
       
          mvprintw(0, 25, newFilename.c_str());
          printInstructions(&termWidth);
          attron(A_BOLD);
          attron(COLOR_PAIR(1));

          handleAlphanumericKeypress(newFilename, newFiles, files, filesWin);
        }
        break;

      case 10:
        writeRenameToDisk(newFiles, files);
        endwin();
        return 0;

      default:
        newFilename += ch;
        mvprintw(0, 25, newFilename.c_str());
        handleAlphanumericKeypress(newFilename, newFiles, files, filesWin);
    }
  }

  endwin();
  return 0;
}
