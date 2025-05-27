// Email: nitzanwa@gmail.com

#include <iostream>
#include <string>
#include <limits>

using namespace std;

// Forward declaration
int runConsoleGUI();
int runDemo();

void printWelcome() {
    cout << "\n" << string(60, '=') << endl;
    cout << "🎮              COUP GAME LAUNCHER              🎮" << endl;
    cout << string(60, '=') << endl;
    cout << "\nChoose your version:" << endl;
    cout << "1. Interactive Console GUI (Recommended)" << endl;
    cout << "2. Demo Mode (Automated)" << endl;
    cout << "3. Exit" << endl;
    cout << "\nEnter choice (1-3): ";
}

int main() {
    int choice;
    
    while (true) {
        printWelcome();
        
        if (!(cin >> choice)) {
            cout << "❌ Invalid input! Please enter a number." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                cout << "\n🎮 Starting Interactive Console GUI..." << endl;
                return runConsoleGUI();
                
            case 2:
                cout << "\n🎬 Starting Demo Mode..." << endl;
                return runDemo();
                
            case 3:
                cout << "\n👋 Thanks for playing! Goodbye!" << endl;
                return 0;
                
            default:
                cout << "❌ Invalid choice! Please enter 1, 2, or 3." << endl;
                break;
        }
    }
}

// Include the console GUI implementation (now in same folder)
#include "ConsoleGUI.cpp"

int runConsoleGUI() {
    try {
        ConsoleGUI gui;
        gui.run();
        return 0;
    } catch (const exception& e) {
        cout << "❌ Console GUI Error: " << e.what() << endl;
        return 1;
    }
}

int runDemo() {
    // Include the demo implementation (go up one level to find Demo.cpp)
    #include "../Demo.cpp"
    // The demo main function will be called
    return 0;
}