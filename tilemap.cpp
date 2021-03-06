#include "enemies.h"
#include "tilemap.h"
#include "block.h"
#include <iostream>
#include <fstream>
#include <string>

// See tilemap.h for higher level context
/////////////////////////
// Public Constructors //
/////////////////////////

// Default Constructor (does nothing for now)
Tilemap::Tilemap()
{

}

// Initializes a tilemap object by passing the path
// to a file representing the tilemap as well as a vector
// containing references to every tile type encountered
// within the  tilemap.
// TO-DO: Automatic detection and passing of the relevant tiles
//		  pertinent to our tileArray vector? (Proc Gen do you hear me???)
Tilemap::Tilemap(std::string tilemap, std::vector<Tile*> tiles, Background* bg)
{
	// Assign values
	this->xMax = 0;			// Default placeholder
	this->yMax = 0;			// Default placeholder
	this->tileArray = tiles;
	this->bg = bg;

	// Use the path to load in the tilemap's
	// text representation and populate the
	// tileMap array
	this->generateTilemap(tilemap);
}

// Initializes a tile map based on given X and Y dimensions.
// Does not use a text file, instead tries to create a Tilemap using randomness
Tilemap::Tilemap(int xDim, int yDim, int room, std::vector<Tile*> tiles, Background* bg,
	int mapType, bool powerUp, bool isHealth, std::vector<Enemy*> enemies) {
	//Assign attributes
	this->yMax = yDim;
	this->xMax = xDim;
	this->room = room;
	this->tileArray = tiles;
	this->bg = bg;
	this->mapType = mapType;
	this->powerUp = powerUp;
	this->isHealth = isHealth;
	this->cDoor = false;

	if (mapType != 3 && mapType != 0) {
		int nEnemies = rand() % 5;
		if (nEnemies > 4) {
			nEnemies = 4;
		}
		for (int i = 0; i < nEnemies; i++) {
			enemies.pop_back();
		}
		this->enemies = enemies;
	}
	else if (mapType == 3) {
		this->enemies = enemies;
	}

	//generate tilemap without text file
	this->generateTilemap();
}

Tilemap::Tilemap(int xDim, int yDim, int room, std::vector<Tile*> tiles, Background* bg) {
	//Assign attributes
	this->yMax = yDim;
	this->xMax = xDim;
	this->room = room;
	this->tileArray = tiles;
	this->bg = bg;

	//generate tilemap without text file
	this->generateTilemap();
}

// Calls draw() for each tile that is on the screen currently (with offset and accounting for tile size)
void Tilemap::drawTilemap(SDL_Renderer* render, int offset_x, int offset_y) {

	for (int i = 0; i < this->yMax; i++) {
		for (int j = 0; j < this->xMax; j++) {

			if (this->tileMap[i][j] != 0 && this->tileMap[i][j] != 3 && this->tileMap[i][j] != 8 && this->tileMap[i][j] != 9) {
				this->tileArray[this->tileMap[i][j] - 1]->getTileSprite()->draw(render, -offset_x + (j * 16), -offset_y + (i * 16));
			}
		}
	}

}

///////////////////////
// Private Functions //
///////////////////////

// Our generateTilemap function simply
// reads in the data passed during construction
// and uses it to initialize and populate
// our tileMap array
void Tilemap::generateTilemap(std::string mapPath)
{
	// Read in the data pointed to by
	// our passed mapPath
	std::ifstream tileFile;
	tileFile.open(mapPath.c_str());

	// Now that our file is open,
	// we read in the dimensions
	// our our tilemap array and then 
	// iterate through, populating the
	// array with the values.
	// NOTE: It is imperative that the tilemap
	// .txt file is in the following format:
	// Line 0: xDimension (max)
	// Line 1: yDimension (max)
	// Line 2: tilemap contents
	// Line 3: tilemap contents
	// Line 4: tilemap contents
	if (tileFile.is_open())
	{
		// initialize a temp variable
		// to hold our read in input,
		// then  begin stepping through our
		// tilefile
		char readIn;
		// Read in the first two lines
		tileFile >> this->xMax;
		tileFile >> this->yMax;
		// Drop our trailing newline
		tileFile.get(readIn);

		// Use our read in dimensions to 
		// initialize our 2d tilemap array
		this->tileMap = new int*[this->yMax];
		for (int i = 0; i < this->yMax; i++)
			this->tileMap[i] = new int[this->xMax];

		// Now that our tileMap array is initialized,
		// step through the text file and read in the 
		// values
		for (int i = 0; i < this->yMax; i++)
		{
			for (int j = 0; j < this->xMax; j++)
			{
				// Get input and 
				// add to the array
				tileFile.get(readIn);
				this->tileMap[i][j] = (readIn - 48);
				//std::cout << tileMap[i][j] << " ";
			}
			// Drop our trailing newline
			tileFile.get(readIn);
			//std::cout << "\n";
		}

		// We're finished, close our file
		tileFile.close();
	}
	// Failed to open??
	else
		std::cout << "Error opening  tilemap file";
}

// Initialize and populate tileMap array without use of
// an external text file.
void Tilemap::generateTilemap() {

	//using block system
	// so we have room size: 45x210
	// we define a block that is: 9x10
	int blockHeight = 9;
	int blockWidth = 10;
	// so any room/map is a 2d array of 5x21 blocks
	int h = this->yMax / blockHeight;
	int w = this->xMax / blockWidth;

	// I have looked into maybe using std::array for this but eh
	Block*** blockMap = new Block**[h];
	for (int i = 0; i < h; i++) {
		blockMap[i] = new Block*[w];

		// iterate over blockMap and create a Block in each spot
		// FIRST PASS - create and generate walls
		for (int j = 0; j < w; j++) {

			// create a block in this spot
			blockMap[i][j] = new Block(i, j, h, w, mapType);

			// then generate this block
			// *Generate creates basic walls that outline the room*
			blockMap[i][j]->generateBlock();

			// set door flags
			if (i == 0 && j == (w - 1) / 2 && this->room >= 8) { //door on the top
				blockMap[i][j]->setDoor();
				this->cDoor = true;
				this->room -= 8;
			}
			if (i == h - 1 && j == (w - 1) / 2 && (this->room >= 4 && this->room < 8)) { //door on the bottom
				blockMap[i][j]->setDoor();
				this->room -= 4;
			}
			if (i == h - 1 && j == 0 && (this->room >= 2 && this->room < 4)) {	//door on the left
				blockMap[i][j]->setDoor();
				blockMap[i - 1][j]->setDoor();
				this->room -= 2;
			}
			if (i == h - 1 && j == w - 1 && (this->room < 2 && this->room > 0)) {	//door on the right
				blockMap[i][j]->setDoor();
				blockMap[i - 1][j]->setDoor();
				this->room -= 1;
			}

		}
	}

	//Make sure left and right doors worked
	if (room != 0) {
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				if (i == h - 1 && j == 0 && (this->room >= 2 && this->room < 4)) {	//door on the left
					blockMap[i][j]->setDoor();
					blockMap[i - 1][j]->setDoor();
					this->room -= 2;
				}
				if (i == h - 1 && j == w - 1 && (this->room < 2 && this->room > 0)) {	//door on the right
					blockMap[i][j]->setDoor();
					blockMap[i - 1][j]->setDoor();
					this->room -= 1;
				}
			}
		}
	}


	// First Pass: Generate each block in terms of the tiles it needs by defualt -> from bottom left to top right
	for (int i = h - 1; i > -1; i--) { //reverse, bot to top
		for (int j = 0; j < w; j++) { //left to right

			// check around this block on all 4 sides
			if (j > 0) { // there is a block to the left
				blockMap[i][j]->checkBlock(blockMap[i][j - 1]);
			}
			if (j < w - 1) { // there is a block to the right
				blockMap[i][j]->checkBlock(blockMap[i][j + 1]);
			}
			if (i > 0) { // there is a block above
				blockMap[i][j]->checkBlock(blockMap[i - 1][j]);
			}
			if (i < h - 1) { // there is a block below
				blockMap[i][j]->checkBlock(blockMap[i + 1][j]);
			}

			//WHAT DO WE DO WITH CHECK
			// This way, when generating the block, it accounts for the blocks around it -> knows where the block next to it would like to connect to it (if we decide to connect to it)
			// maybe we also include a priority for connection (must connect vs can connect)

			// idea is to pass in ptr of blocks that pass a connected check adjacent to this
			// think there is 3 tiles of platform going out of the right of the block on our left
			// this block would be told that the left block wants to connect, and then would know where to place a platform in order to connect across blocks

			// generate block, with info of blocks around it (that have been generated)
			blockMap[i][j]->generateBlock();

			// After generating the block, we set it 0 - initially generated
			blockMap[i][j]->setBlock(0);

			// If a block has 0s and 1s on all sides, it becomes 1 - perma set
			// not doing this yet

		}
	}

	// we pass the room num into the tilemap -> from here room can be used to set the door flag for the correct block (Corner WallR at bottom if 0)

	// now that the room and walls exist, we want to make a second pass and add platforms
	// we should gen a number of platforms here to say how many platforms will be in the room

	//number of platforms for this room?
	int numPlats = rand() % 4 + 5;
	if (this->cDoor) {
		blockMap[1][w / 2]->placePlatforms(3, 5);
		blockMap[2][w / 2 - 2]->placePlatforms(6, 5);
		blockMap[2][w / 2]->placePlatforms(6, 8);
		blockMap[2][w / 2 + 1]->placePlatforms(3, 7);
		blockMap[h - 2][w / 2 - 1]->placePlatforms(7, 6);
	}

	// Second Pass: Platforms (and Doors? we have doors in first pass rn)
	// call it PopulateBlock()? or AddPlatforms()?
	for (int i = h - 1; i > -1; i--) { //reverse, bot to top
		for (int j = 0; j < w; j++) { //left to right

			// check around this block on all 4 sides
			if (j > 0) { // there is a block to the left
				blockMap[i][j]->checkBlock(blockMap[i][j - 1]);
			}
			if (j < w - 1) { // there is a block to the right
				blockMap[i][j]->checkBlock(blockMap[i][j + 1]);
			}
			if (i > 0) { // there is a block above
				blockMap[i][j]->checkBlock(blockMap[i - 1][j]);
			}
			if (i < h - 1) { // there is a block below
				blockMap[i][j]->checkBlock(blockMap[i + 1][j]);
			}

			blockMap[i][j]->populateBlock();
			if (blockMap[i][j]->hasPlatform() == false && blockMap[i][j]->isMiddle()) {
				if (!doorCheck(blockMap, i, j)) {
					int pLocation = rand() % 8;
					if (blockMap[i - 1][j]->hasPlatform()) {
						pLocation = blockMap[i - 1][j]->getPlatLocation();
					}
					else if (blockMap[i + 1][j]->hasPlatform()) {
						pLocation = blockMap[i + 1][j]->getPlatLocation();
					}
					else {
						blockMap[i][j]->addPlatforms(pLocation);
					}
				}
			}
		}
	}


	// Use our dimensions to 
	// initialize our 2d tilemap array
	this->tileMap = new int*[this->yMax];
	for (int i = 0; i < this->yMax; i++) {
		this->tileMap[i] = new int[this->xMax];
	}
	// At this point all blocks should have been generated and set according to all other blocks in the room
	// now we convert our blockMap into our final tilemap
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			int** curr = blockMap[i][j]->getBlockMap();

			// this sucks is there a better way to do this? I am dumb for doing it this way perhaps?

			// copy into tilemap
			for (int row = 0; row < blockHeight; row++) {
				for (int col = 0; col < blockWidth; col++) {
					// index in tilemap is offset by which block we are copying
					int rowInd = row + (i * blockHeight);
					int colInd = col + (j * blockWidth);
					//std::cout << curr[row][col] << " to tilemap index " << rowInd << " " << colInd << std::endl;
					this->tileMap[rowInd][colInd] = curr[row][col];
				}
			}

		}
	}

	if (this->powerUp == true && this->isHealth == false) {
		this->tileMap[43][160] = 9;
		this->tileMap[40][145] = 8;
	}
	else if (this->powerUp == true && this->isHealth == true) {
		this->tileMap[40][145] = 8;
	}
}

bool Tilemap::doorCheck(Block*** blocks, int i, int j) {

	if (blocks[i - 1][j - 1]->isDoor()) {
		return true;
	}
	else if (blocks[i + 1][j + 1]->isDoor()) {
		return true;
	}
	else if (blocks[i + 1][j - 1]->isDoor()) {
		return true;
	}
	else if (blocks[i - 1][j + 1]->isDoor()) {
		return true;
	}
	else if (blocks[i - 1][j]->isDoor()) {
		return true;
	}
	else if (blocks[i + 1][j]->isDoor()) {
		return true;
	}
	else {
		return false;
	}

}

/////////////////////////
// Getters and Setters //
/////////////////////////

// Width and Height Getters
int Tilemap::getMaxWidth()
{
	return this->xMax;
}
int Tilemap::getMaxHeight()
{
	return this->yMax;
}

// Tilemap Getter
int** Tilemap::getTileMap()
{
	return this->tileMap;
}

// Tile Getter
// WARNING: DON'T GO OUT OF BOUNDS ON THE INDEX PLSS
Tile* Tilemap::getTile(int index)
{
	return this->tileArray.at(index);
}

Background* Tilemap::getBackground() {
	return this->bg;
}

std::vector<Enemy*> Tilemap::getEnemies() {
	return this->enemies;
}


/////////////////
// Destructor  //
/////////////////

// Make sure to clean up memory
Tilemap::~Tilemap()
{
	delete this->tileMap;
}

