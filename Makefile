CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -g

# ספריות
INCLUDES = -IPlayers -IGameLogic -IGUI
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# קבצים
SRC = main.cpp \
      GameLogic/Game.cpp \
      GameLogic/BankManager.cpp \
      GameLogic/PlayerFactory.cpp \
      Players/Player.cpp \
      Players/Roles/Governor.cpp \
      Players/Roles/Baron.cpp \
      Players/Roles/Spy.cpp \
      GUI/GUI.cpp

OBJ = $(SRC:.cpp=.o)

# קובץ סופי
TARGET = main

# ברירת מחדל
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
