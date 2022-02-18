class Listbrowser1 : public Widget {
public:
    Listbrowser1() : Widget() { setName("Listbrowser1"); }
    void createGuiObject(Layout *layout) {
        Listbrowser *listbrowser = layout->createListbrowser();
        listbrowser->setColumnTitles("Row|Your|Boat");
        vector<string> text1;
        text1.push_back("Yes");
        text1.push_back("more");
        text1.push_back("please!");
        text1.push_back("Help me!");
        listbrowser->addNode(text1);
        vector<string> text2;
        text2.push_back("Hello");
        text2.push_back("again.");
        //text2.push_back("Bla.");
        listbrowser->addCheckboxNode(text2, true, true, 0, false, 0);
    }
};

class Listbrowser2 : public Widget {
public:
    Listbrowser2() : Widget() { setName("Listbrowser2"); }
    void createGuiObject(Layout *layout) {
        Listbrowser *listbrowser = layout->createListbrowser();
        listbrowser->setHierachical(true);
        listbrowser->addNode("Hello", 0, true, 1);
        listbrowser->addNode("and", 0, false, 2);
        listbrowser->addNode("welcome", 0, false, 2);
        listbrowser->addNode("I", 0, true, 1);
        listbrowser->addNode("am", 0, true, 2);
        listbrowser->addNode("your grandson", 0, false, 3);
        listbrowser->addNode("Hierymies", 0, false, 3);
        listbrowser->addNode("Isn't it awesome?", 0, false, 0);
    }
};

class Listbrowser3 : public Widget {
public:
    Listbrowser3() : Widget() { setName("Listbrowser3"); }
    void createGuiObject(Layout *layout) {
        Listbrowser *listbrowser = layout->createListbrowser();
        listbrowser->setStriping(true);
        listbrowser->addNode("Hello");
        listbrowser->addNode("and");
        listbrowser->addNode("welcome.");
        listbrowser->addNode("I");
        listbrowser->addNode("am");
        listbrowser->addNode("your grandson");
        listbrowser->addNode("Hierymies.");
        listbrowser->addNode("Isn't it awesome?");
    }
};

class Speedbar1 : public Widget {
public:
    Speedbar1() : Widget() { setName("Speedbar1"); }
    void createGuiObject(Layout *layout) {
        Speedbar *speedBar = layout->createSpeedbar();
        speedBar->addButton(1, "Hello", "rescue");
        speedBar->addButton(2, "how", "arrowright");
        speedBar->addButton(3, "are", "checkpage");
        speedBar->addSpacer();
        speedBar->addButton(4, "you?", "warning");
    }
};

class Layout1 : public Widget {
public:
    Layout1() : Widget() { setName("Layout1"); }
    void createGuiObject(Layout *layout) {
        Layout *layout1 = layout->createVerticalLayout();
        layout->createSpace();
        Layout *layout2 = layout->createLabeledLayout("Hello");
        Speedbar *speedbar = layout1->createSpeedbar();
        speedbar->addButton(1, "Hello", "rescue");
        speedbar->addButton(2, "how", "arrowright");
        speedbar->addButton(3, "are", "checkpage");
        speedbar->addSpacer();
        speedbar->addButton(4, "you?", "warning");
        layout1->createButton("Yo");
        
        Listbrowser *listbrowser = layout2->createListbrowser();
        listbrowser->setPen(PublicScreen::PENTYPE_CRITICAL);
        listbrowser->addNode("This is wrong! (CRITICAL)");
        listbrowser->setPen(PublicScreen::PENTYPE_INFO);
        listbrowser->addNode("Very interesting (INFO)");
        listbrowser->setPen(PublicScreen::PENTYPE_OUTPUT);
        listbrowser->addNode("Something is going on (OUTPUT)");
        listbrowser->setPen(PublicScreen::PENTYPE_COMMAND);
        listbrowser->addNode("DO the dishes (COMMAND)");
        listbrowser->setPen(PublicScreen::PENTYPE_EVENT);
        listbrowser->addNode("IT's YOUR BIRTHDAY! (EVENT)");
        listbrowser->setPen(PublicScreen::PENTYPE_DEFAULT);
        listbrowser->addNode("Just the ordinary (DEAFULT)");
    }
};


class Panel1 : public Widget {
public:
    Panel1() : Widget() {}
    void createGuiObject(Layout *layout) {
        Panel *panel = new Panel(this);
        panel->addWidget(new Listbrowser1);
        panel->addWidget(new Listbrowser2);
        panel->addWidget(new Listbrowser3);
        layout->addTabbedPanel(panel, 0); //weight 0
    }
};

class MainMenu1 : public Menubar {
public:
    MainMenu1(Widget *parent) : Menubar(parent) {}
    void createMenu() {
        MenuReference panel1 = addCreateMenu ("Panel 1");
        MenuReference panel2 = addCreateMenu ("Panel 2");

        addCreateMenuItem (panel1, "Item 1", "CTRL+1", 1);
        addCreateMenuItem (panel2, "Item 2", "CTRL+2", 2);
        addCreateMenuItem (panel2, "Item 2", "CTRL+3", 3);
    }
    bool handleMenuPick(int id) {
        return false;
    }
};

class Menu1 : public Widget {
public:
    Menu1() : Widget() {}
    void createGuiObject(Layout *layout) {
        MainMenu1 *mainMenu = new MainMenu1(this);
        setMenubar(mainMenu);
        layout->createButton("Try the menu");
    }
};

class MainMenu2 : public Menubar {
private:
    MainWindow *parent;
public:
    MainMenu2(MainWindow *parent) : Menubar(parent) { this->parent = parent; }
    void createMenu() {
        MenuReference panel1 = addCreateMenu ("Panel 1");
        MenuReference panel2 = addCreateMenu ("Panel 2");

        addCreateMenuItem (panel1, "Item 1", "CTRL+1", 1);
        addCreateMenuItem (panel2, "Item 2", "CTRL+2", 2);
        addCreateMenuItem (panel2, "Item 2", "CTRL+3", 3);
    }
    bool handleMenuPick(int id) {
        static bool unity = true;
        if(unity) {
            parent->closeWindow();
//            parent->openWindow();
            unity = false;
        } else {
//            parent->closeSeparated();
            return true;
        }
        return false;
    }
};

class MainWindow1 : public MainWindow {
    public:
    MainWindow1() : MainWindow() {
        setMenubar(new MainMenu2(this));
        setTopBar(new Speedbar1);
        setMainView(new Listbrowser1);
        addLeftPanelWidget(new Listbrowser2);
        addLeftPanelWidget(new Listbrowser3);
        addBottomPanelWidget(new Layout1);
    }
};