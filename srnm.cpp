#include <stdio.h>
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
  std::vector<std::string> files;

  directory = opendir(".");

  if (directory)
  {
    while ((dir = readdir(directory)) != NULL) 
    {
      std::string filename = dir->d_name;

      if (filename.compare(".") != 0 && filename.compare("..") != 0) {
        files.push_back(filename);
      }
    }

  closedir(directory);
  }

  return files;
}

void writeRenameToDisk(std::vector<std::string> newFiles, std::vector<std::string> files)
{
  DIR *directory;
  struct dirent *dir;
  directory = opendir(".");
  unsigned short idx = 0;

  if (directory)
  {
    while ((dir = readdir(directory)) != NULL) 
    {
      std::string filename = dir->d_name;

      if (filename.compare(".") != 0 && filename.compare("..") != 0) 
      {
        if (files[idx].compare(filename) == 0)
        {
          rename(filename.c_str(), newFiles[idx].c_str());
          idx++;
        }
      }
    }
  }

  closedir(directory);
}

void printDirectory(std::vector<std::string> *files, WINDOW *filesWin)
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

  wattron(filesWin, A_BOLD | COLOR_PAIR(2));
  for (std::vector<std::string>::iterator it = (*files).begin(); it != (*files).end(); it++)
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
  wattroff(filesWin, A_BOLD | COLOR_PAIR(2));
}

unsigned short getDigitCount(unsigned short digit)
{
  unsigned short numDigits = 1;

  while ((digit = digit / 10) > 0)
  {
    numDigits++;
  }

  return numDigits;
}

std::string getLeadingZeroes(unsigned short *maxDigitCount, unsigned short *filesIdx)
{
  std::string leadingZeroes;
  unsigned short idx = 0;

  for (idx = 0; idx < (getDigitCount(*maxDigitCount) - (getDigitCount(*filesIdx) - 1)); idx++)
  {
    leadingZeroes.append("0");
  }

  return leadingZeroes;
}

void handleAlphanumericKeypress(std::string newFilename, std::vector<std::string> *newFiles, std::vector<std::string> *files, WINDOW *filesWin)
{
  unsigned short numFiles = files->size(),
                 maxDigitCount = getDigitCount(numFiles), 
                 filesIdx = 1;
  
  for (std::vector<std::string>::iterator it = (*newFiles).begin(); it != (*newFiles).end(); it++)
  {
    std::string leadingZeroes = getLeadingZeroes(&maxDigitCount, &filesIdx),
                period = ".",
                dash = "-",
                extension = period.append((*it).substr((*it).find_last_of(".") + 1));

    leadingZeroes = dash.append(leadingZeroes.append(std::to_string(filesIdx)).append(extension));
    (*it) = newFilename;
    (*it) = (*it).append(leadingZeroes);
    filesIdx++;
  }

  printDirectory(newFiles, filesWin);
  move(0, (27 + newFilename.length()));
}

void printInstructions(unsigned short *termWidth)
{
  attron(A_BOLD | COLOR_PAIR(1));

  std::string instructions = "(<ENTER> to rename, <CTRL+C> to quit)";
  mvprintw(0, (*termWidth - instructions.length()), instructions.c_str());

  attroff(A_BOLD | COLOR_PAIR(1));
}

void printWarning(PANEL *warningPanel, WINDOW *warningWin)
{
  unsigned short boxHeight = 0,
                 boxWidth = 0;

  warningPanel = new_panel(warningWin);
  werase(warningWin);
  box(warningWin, 0, 0);

  std::string warning = "This action cannot be undone.",
              confirm = "Press <ENTER> once more to confirm.",
              cancel = "Press <CTRL+C> to cancel and quit.";

  getmaxyx(warningWin, boxHeight, boxWidth);

  wattron(warningWin, A_BOLD | COLOR_PAIR(1));

  mvwprintw(warningWin, (boxHeight / 2) - 1, (boxWidth / 2) - (warning.length() / 2), warning.c_str());
  mvwprintw(warningWin, (boxHeight / 2), (boxWidth / 2) - (confirm.length() / 2), confirm.c_str());
  mvwprintw(warningWin, (boxHeight / 2) + 1, (boxWidth / 2) - (cancel.length() / 2), cancel.c_str());

  wattroff(warningWin, A_BOLD | COLOR_PAIR(1));

  move(0,0);
  update_panels();
  doupdate();
}

int main(int argc, char* argv[])
{
  WINDOW *filesWin, *warningWin;
  PANEL *filesPanel, *warningPanel;
  cbreak();

  unsigned short filesPanelY = 3,
                 filesPanelX = 0,
                 termHeight = 0,
                 termWidth = 0;

  int ch;
  bool hasAccepted = false;

  initscr();
  getmaxyx(stdscr, termHeight, termWidth);
  filesWin = newwin(termHeight - filesPanelY, termWidth, filesPanelY, filesPanelX);
  warningWin = newwin(termHeight / 2, termWidth / 2, (termHeight / 2) - (termHeight / 4), (termWidth / 2) - (termWidth / 4));

  std::vector<std::string> files = getFiles(),
                           newFiles = files;
  std::string newFilename;
  
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_GREEN, COLOR_BLACK);
 
  filesPanel = new_panel(filesWin);
  printDirectory(&files, filesWin);
  printInstructions(&termWidth);

  mvprintw(0, 0, "Enter sequential filename: ");
   
  attron(A_BOLD | COLOR_PAIR(1));

  noecho();
  
  while ((ch = getch()) != KEY_F(1))
  {
    noecho();
    switch(ch)
    {
      // KEY_BACKSPACE isn't currently working, so we'll use 127 instead.
      case 127:
        if (newFilename.length() > 0) {
          newFilename.pop_back();

          move(0, 27);
          clrtoeol();
       
          mvprintw(0, 27, newFilename.c_str());
          printInstructions(&termWidth);
          attron(A_BOLD | COLOR_PAIR(1));

          handleAlphanumericKeypress(newFilename, &newFiles, &files, filesWin);
        }
        break;

      case 10:
        if (!hasAccepted) {
          printWarning(warningPanel, warningWin);
          hasAccepted = true;
        } else {
          writeRenameToDisk(newFiles, files);
          endwin();
          return 0;
        }
        break;

      default:
        if (!hasAccepted && newFilename.length() < 10) {
          newFilename += ch;
          mvprintw(0, 27, newFilename.c_str());
          handleAlphanumericKeypress(newFilename, &newFiles, &files, filesWin);
        }
    }
  }

  endwin();
  return 0;
}
