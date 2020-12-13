#include "StartMenu.h"
#include "MerkelMain.h"
#include "MerkelBot.h"
#include <iostream>
#include <string>

StartMenu::StartMenu()
{

}

void StartMenu::init()
{
    int input;
    while (true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}

void StartMenu::printMenu()
{
    std::cout << "==== Choose an Option ====" << std::endl;
    std::cout << "1: Start MerkelRex user mode " << std::endl;
    std::cout << "2: Start MerkelRex bot mode " << std::endl;

}

int StartMenu::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1 or 2" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void StartMenu::processUserOption(int userOption)
{
    if (userOption != 1 && userOption != 2)
    {
        std::cout << "Invalid choice. Choose option 1 or 2" << std::endl;
    }
    if (userOption == 1)
    {
        std::cout << "Entering user mode" << std::endl;
        MerkelMain app{};
        app.init();
    }
    if (userOption == 2)
    {
        std::cout << "Starting bot" << std::endl;
        MerkelBot botApp{};
        botApp.init();
    }
     
}