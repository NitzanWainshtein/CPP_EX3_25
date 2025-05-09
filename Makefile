CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# ספריות
INCLUDES = -IPlayers -IPlayers/Roles -IGameLogic -IGUI
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# קבצי מקור
SRC = main.cpp \
      GameLogic/Game.cpp \
      GameLogic/BankManager.cpp \
      GameLogic/PlayerFactory.cpp \
      Players/Player.cpp \
      Players/Roles/Governor.cpp \
      Players/Roles/Baron.cpp \
      Players/Roles/Spy.cpp \
      Players/Roles/Merchant.cpp \
      GUI/GUI.cpp

# קבצי אובייקט
OBJ = $(SRC:.cpp=.o)

# קובץ פלט סופי
TARGET = main

# קומפילציה ראשית
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
