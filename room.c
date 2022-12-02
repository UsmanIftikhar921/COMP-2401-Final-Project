#include "defs.h"

/*
  Function:  initRoomList
  Purpose:   initialize a list of rooms by setting it's head and tail to null, and it's size to 0
       in:   a pointer to a RoomListType
   return:   a pointer to a RoomListType that has been initialized
*/
void initRoomList(RoomListType * roomList){
	roomList->head = NULL;
	roomList->tail = NULL;
	roomList->size = 0;
}

/*
  Function:  initRoom
  Purpose:   initialize a room by setting it's fields to the corresponding values provided
       in:   a double pointer to a room
       in:   a pointer to the name of the room
   return:   a pointer to a RoomType with initialized contents
*/
void initRoom(char * name, RoomType ** room){
	//Assign memory for the room data
	*room = calloc(1, sizeof(RoomType));
	
	//Initialize the room's Name
	strcpy((*room) -> name, name);
	
	//Initialize The Evidence List, Room List and Hunter Array
	initEvidenceList((*room) -> evidence);
	initRoomList((*room) -> attached);
	initHunterArray((*room) -> hunters);
}

/*
  Function:  addRoom
  Purpose:   adds a room to the room list
       in:   a pointer to a RoomListType
       in:   a pointer to a RoomType
   return:   a pointer to a RoomListType with a room element inside it
*/
void addRoom(RoomListType * list, RoomType * room){	
	//A temporary node to store the information of the new tail to be added
	RoomNodeType * newNode;
	
	//Assign memory to the new node
	newNode = calloc(1, sizeof(RoomNodeType));
	
	//The data of the new node is the room data
	newNode -> data = room;
	
	//The new tail does not have any nodes after it, so make sure to mark it as so
	newNode -> next = NULL;
	
	// If list is empty:
	if (list -> head == NULL) {
		list -> head = newNode;
		list -> tail = newNode;
    	}
    	
    	// If list is not empty
    	else {
		list -> tail -> next = newNode;
		list -> tail = newNode;
   	}
}

/*
  Function:  connectRooms
  Purpose:   connects two rooms together by adding the opposing room to each other's connected rooms list
       in:   a pointer to a RoomType for the first room
       in:   a pointer to a RoomType for the second room
   return:   pointers to two rooms that are connected to each other
*/
void connectRooms(RoomType * room1, RoomType * room2){
	// Add Room 2 to Room 1's attached roomList
	addRoom(room1->attached, room2);
	
	// Add Room 1 to Room 2's attached roomList
	addRoom(room2->attached, room1);
}

/*
  Function:  cleanupRoom
  Purpose:   clears all of the contents of the room
       in:   a pointer to a RoomType
   return:   a pointer to an empty RoomType
*/
void cleanupRoom(RoomType * room){
	// Free Evidence and Room Linked List
	cleanupEvidenceList(room -> evidence);
	cleanupRoomList(room -> attached);
	// Free hunter array
	cleanupHunterArray(room -> hunters);
}

/*
  Function:  cleanupRoomList
  Purpose:   clears all of the contents of the room list
       in:   a pointer to a RoomListType
   return:   a pointer to an empty RoomListType
*/
void cleanupRoomList(RoomListType * roomList){
	// Record the size of the room
	int roomListSize = roomList -> size;
	
	// Make two temporary nodes to iterate through the linked list
	RoomNodeType * currNode = roomList -> head;
	RoomNodeType * nextNode;
	
	// Clean up every room in the room list
	for(int i = 0; i < roomListSize; i++){
		nextNode = currNode -> next;
		cleanupRoom(currNode -> data);
		free(currNode);
		currNode = nextNode;
	}
}

/*
  Function:  randRoom
  Purpose:   get a random room from the room list
       in:   a pointer to a RoomListType
       in:   an integer that determines whether or not the ghost can move inside the van
   return:   a pointer to a room that has been randomly chosen
*/
RoomType * randRoom(RoomListType * roomList, int spawnGhostCheck) {
    if (spawnGhostCheck == C_FALSE) {
        int selectedRoom = randInt(0, roomList -> size - 1);
    } else {
        int selectedRoom = randInt(1, roomList -> size - 1);
    }

    RoomNodeType currRoom = roomList -> head;

    for (int i = 0; i < roomList -> size; i++) {
        if (i == selected) {
            return currRoom -> roomData;
        } else {
            currRoom = currRoom -> next;
        }
    }
}





