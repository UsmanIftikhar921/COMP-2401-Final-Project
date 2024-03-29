#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define C_FALSE			0
#define C_TRUE			1
#define NORMAL			0
#define GHOSTLY			1
#define MAX_STR			64
#define FEAR_RATE		5
#define BOREDOM_RATE		5
#define MAX_FEAR		100
#define MAX_HUNTERS		4
#define USLEEP_TIME		50000
#define BOREDOM_MAX		99

// You may rename these types if you wish
typedef enum { EMF, TEMPERATURE, FINGERPRINTS, SOUND } EvidenceClassType;
typedef enum { POLTERGEIST, BANSHEE, BULLIES, PHANTOM } GhostClassType;

// Hunder list
typedef struct HunterArrayType {
	struct HunterType * hunters[MAX_HUNTERS];
	sem_t * mutex;
	int size;
} HunterArrayType;


// Evidence list
typedef struct EvidenceListType {
	struct EvidenceNodeType * head;
	struct EvidenceNodeType * tail;
	sem_t mutex;
	int size;
} EvidenceListType;

// Room list, nodes, and type structs
typedef struct RoomListType {
	struct RoomNodeType * head;
	struct RoomNodeType * tail;
	int size;
} RoomListType;

// Evidence node, and type structs
typedef struct EvidenceNodeType {
	struct EvidenceType * evidenceData;
	struct EvidenceNodeType * next;
} EvidenceNodeType;

typedef struct RoomNodeType {
	struct RoomType * roomData;
	struct RoomNodeType * next;
} RoomNodeType;

typedef struct RoomType {
 	char name[MAX_STR];
 	struct GhostType * ghost;
 	EvidenceListType * evidence;
  	RoomListType * attached;
  	HunterArrayType * hunters;
	sem_t mutex;
} RoomType;
 
// Ghost type struct
typedef struct GhostType {
	GhostClassType ghostType;
	RoomType * room;
	struct BuildingType * building;
	int boredom;
} GhostType;

typedef struct EvidenceType {
	int ghostliness;
	EvidenceClassType type;
	float value;
} EvidenceType;

// Hunter type
typedef struct HunterType {
	char name[MAX_STR];
	RoomType * room;
	EvidenceClassType evidenceType;
	EvidenceListType * evidence;
	int fear;
	int boredom;
    	int id;
} HunterType;

// Building type struct
typedef struct BuildingType {
	GhostType * ghost;
	RoomListType * rooms;
	HunterArrayType * hunters;
	EvidenceListType * evidence;
	int haunted;
} BuildingType;


// Helper functions
int randInt(int, int);							// Generates a pseudorandom integer between the parameters
float randFloat(float, float);						// Generates a pseudorandom float between the parameters


// Building functions
void populateRooms(BuildingType *);					// Populates the building with sample data for rooms
void initBuilding(BuildingType *);					// Initializes a BuildingType
void cleanupBuilding(BuildingType *);					// Frees all data associated with a building


// Room functions
void initRoomList(RoomListType *);					// Initializes a RoomListType 
void initRoom(RoomType **, char *);					// Initializes a RoomType 
void addRoom(RoomListType *, RoomType *);				// Adds a room to the end of a RoomListType
void connectRooms(RoomType *, RoomType *);				// Connects two RoomTypes
void cleanupRoom(RoomType *);						// Frees all data in a room
void cleanupRoomList(RoomListType *);					// Frees all nodes in a RoomList
RoomType * randRoom(RoomListType *, int);          			// Gets a random room in a list
void cleanupRoomListNodes(RoomListType *);				// Cleans up the nodes of a RoomListType

// Ghost functions
void initGhost(GhostClassType, RoomType *, GhostType **, BuildingType *);	// Initializes a GhostType
void moveGhost(GhostType *);						// Moves a ghost to another room
void cleanupGhostData(GhostType *);					// Frees all data associated with a ghost
void addGhostEvidence(GhostType *);					// Adds ghostly evidence to a room
void spawnGhost(GhostType *, BuildingType *);                       	// Adds the ghost to a random room that is not the van

// Evidence functions
void initEvidenceList(EvidenceListType *);				// Initializes an EvidenceListType
void initEvidence(EvidenceClassType, EvidenceType **);			// Initializes an EvidenceType
EvidenceType * getRandEvidence(EvidenceListType *, EvidenceClassType);	// Gets evidence at a random index
EvidenceType * getEvidenceAtIndex(EvidenceListType*, int);		// Returns the evidence at the index specified
void addEvidence(EvidenceListType *, EvidenceType *);			// Adds an EvidenceType to and EvidenceTypeList
void cleanupEvidenceData(struct EvidenceNodeType *);			// Frees all data associated with a piece of evidence
void cleanupEvidenceNode(struct EvidenceNodeType *);			// Frees an evidence node
void cleanupEvidenceList(EvidenceListType *);				// Frees all data from an evidence list
void cleanupEvidenceListNodes(EvidenceListType *);			// CLeans up all EvidenceNodeTypes in an EvidenceListType
void removeEvidence(EvidenceListType *, EvidenceType *);		// Removes evidence from a list if it matches the input evidence



// Hunter functions
void initHunter(EvidenceClassType, char *, RoomType *, HunterType **, int);	// Initializes a HunterType
void initHunterArray(HunterArrayType *);				// Initializes a HunterArrayType
void collectEvidence(HunterType *);					// Collects all evidence from a room
int containsEvidenceType(EvidenceClassType , struct HunterType *);	// Checks if hunter's evidence array already has the evidence type we're adding
void transferEvidenceData(HunterType *, EvidenceType *);		// Transfers evidence data to the hunter 
void cleanupHunterData(HunterType *);					// Frees all data from a hunter
void cleanupHunterArray(HunterArrayType *);				// Frees all data in a hunter array
void removeHunterFromRoom(HunterType *, RoomType *);			// Removes a hunter from a room
void addHunterToRoom(HunterType *, RoomType *);				// Adds a hunter to another room

// main threading control
void getHunterNames (char *, char *, char *, char *);			// Creates the hunter's names
void typeCalculator (EvidenceListType *); 				// Calculates the type of a ghost based on hunter's evidence
void * hunterAction (void *);						// Loops through the hunter's possible actions
void * ghostAction (void *);						// Loops through the ghosts possible actions
void moveHunter (HunterType *);						// Moves hunter through the house
void shareEvidence(HunterType *);					// Shares evidence with another hunter
int endGameConditions(HunterType *);					// Determine if the hunter exits the house
void endGame(HunterType *, HunterType *, HunterType *, HunterType *);   // Determines if the hunters won