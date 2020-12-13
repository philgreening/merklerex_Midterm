
class StartMenu
{
    public:
        StartMenu();
        void init();

    private:
        void printMenu();
        int getUserOption();
        void processUserOption(int userOption);
};
