#include "functions.h"

//FUNCTION DECLERATIONS
BOOL CheckIfExists(Global*, char*);
BOOL CheckPassword(char*);
int Register(Global*);
int StudentRegister(Global*);
int WatcherRegister(Global*);
int Login(Global*);
Student* FindStudentByUN(Global*, char*);
Watcher* FindWatcherByUN(Global*, char*);
Admin* FindAdminByUN(Global*, char*);
//END OF DECLARATIONS

int Register(Global *g)
{
	int choice;                                                                                     //Action menu - choosing user type or action (student, watcher, back)
	do{
		printf("Choose a user type:\n\t[1] Student\n\t[2]Watcher\n\t[3]Back to last menu\n");
		scanf("%d", &choice);
	} while (choice != 1 && choice != 2 && choice != 3);

	if (choice == 1)                                                                                //Executing student registration
	{
		return StudentRegister(g);
	}
	else if (choice == 2)																			//Executing watcher registration
	{
		return WatcherRegister(g);
	}
	else
	{																								//Back to last menu
		return 0;
	}
}

int StudentRegister(Global *g)
{
	char stringID[31];
	int choice = 0;
	Student* newStudent = NULL;																		//Allocating memory for the new student
	newStudent = (Student*)malloc(sizeof(Student));

	if (newStudent == NULL)                                                                         //Allocation check
	{
		printf("Error allocating memory for the new student.\n");
		return 0;
	}

	newStudent->StudentID = g->StudentRunID;														//ID allocation
	(g->StudentRunID)++;

																									//Initializing username
	do{																								
		_flushall();
		printf("Enter your desired username: (Has to be less then 30 characters!)\n");
		scanf("%s", newStudent->StudentUsername);
		getchar();
		if (CheckIfUserExists(g, newStudent->StudentUsername) == TRUE)
		{
			printf("This username already exists in the system. \n Press 1 to try again, press 2 to go back to the next menu.\n");
			scanf("%d", &choice);
		}
	} while (CheckIfUserExists(g, newStudent->StudentUsername) == TRUE && choice == 1);

	if (choice != 1 && choice != 0)
	{
		printf("You did not press 1 to continue. Going back to last menu.\n");
		return 0;
	}
																									//Initializing password
	do{
		_flushall();
		printf("Enter your desired password: (Has to be less then 30 characters, and contain\n");
		printf("at least one digit, one upper case letter, one lower case letter)\n");
		scanf("%s", newStudent->StudentPassword);
		if (CheckPassword(newStudent->StudentPassword) == FALSE)
		{
			printf("This password is invalid.\n You need to try again.\n");
		}
	} while (CheckPassword(newStudent->StudentPassword) == FALSE);
																									//Initializing other attributes that require user input
	printf("Enter your email address: (Has to be less then 50 characters!)\n");
	scanf("%s", newStudent->StudentEmail);
	printf("Enter your first name: (Has to be less then 20 characters!)\n");
	scanf("%s", newStudent->StudentName);
	printf("Enter your last name: (Has to be less then 20 chatacters!)\n");
	scanf("%s", newStudent->StudentSurename);
	printf("Enter you department: (Has to be less then 20 characters!)\n");
	scanf("%s", newStudent->StudentDepartment);
	_flushall();
	printf("Enter your year in the academic facility: (Has to be 1 character!)\n");
	scanf("%c", &(newStudent->StudentYear));
	//Default initializations 
	newStudent->Group = STUDENT;
	newStudent->StudentProjectsAmount = 0;
	newStudent->ProjectIDS= NULL;
	newStudent->StudentTasksAmount = 0;

	sprintf(stringID, "%d", newStudent->StudentID); //convert id num to string
	strcpy(newStudent->StudentActivityLog, stringID); //insert this num to student log string
	strcat(newStudent->StudentActivityLog, "_SLog.txt"); //cat the rest of the file name

	strcpy(newStudent->StudentMessages, stringID); //insert id string to messages string
	strcat(newStudent->StudentMessages, "_SMess.txt"); // cat the rest of the file name

	newStudent->StudentNext = g->StudentList;
	g->StudentList = newStudent;
	return newStudent->StudentID;
}

int WatcherRegister(Global *g)
{
	Watcher* newWatcher = NULL;
	int choice = 0;
	newWatcher = (Watcher*)malloc(sizeof(Watcher));													//Allocating memory for the new watcher

	if (newWatcher == NULL)                                                                         //Allocation check
	{
		printf("Error allocating memory for the new watcher.\n");
		return 0;
	}
	newWatcher->WatcherID = g->WatcherRunID;														//Id allocation
	(g->WatcherRunID)++;

	do{																								//Initializing username
		_flushall();
		printf("Enter your desired username: (Has to be less then 30 characters!)\n");
		scanf("%s", newWatcher->WatcherUsername);
		getchar();
		if (CheckIfUserExists(g, newWatcher->WatcherUsername) == TRUE)
		{
			printf("This username already exists in the system. \n Press 1 to try again, press 2 to go back to the next menu.\n");
			scanf("%d", &choice);
		}
	} while (CheckIfUserExists(g, newWatcher->WatcherUsername) == TRUE && choice == 1);

	if (choice != 1 && choice != 0)
	{
		printf("You did not press 1 to continue. Going back to last menu.\n");
		return 0;
	}
																									//Initializing password
	do{
		_flushall();
		printf("Enter your desired password: (Has to be less then 30 characters, and contain\n");
		printf("at least one digit, one upper case letter, one lower case letter)\n");
		scanf("%s", newWatcher->WatcherPassword);
		if (CheckPassword(newWatcher->WatcherPassword) == FALSE)
		{
			printf("This password is invalid.\n You need to try again.\n");
		}
	} while (CheckPassword(newWatcher->WatcherPassword) == FALSE);
																									//Initializing other attributes that require user input
	printf("Enter your email address: (Has to be less then 50 characters!)\n");
	scanf("%s", newWatcher->WatcherEmail);
	printf("Enter your first name: (Has to be less then 20 characters!)\n");
	scanf("%s", newWatcher->WatcherName);
	printf("Enter your last name: (Has to be less then 20 chatacters!)\n");
	scanf("%s", newWatcher->WatcherSurename);

	newWatcher->Group = WATCHER;																	//Default initializations
	newWatcher->ProjectIDS = NULL;
	newWatcher->WatcherProjectsAmount = 0;
	newWatcher->WatcherReceiveChanges = FALSE;

	newWatcher->WatcherNext = g->WatchersList;
	g->WatchersList = newWatcher;
	return newWatcher->WatcherID;
}

//HELP FUNCTION for Login(Global)
//Receives a username and returns a pointer to the student with that username.
Student* FindStudentByUN(Global *g, char *username)
{
	Student *tempStud = g->StudentList;
	while (tempStud != NULL)																		//Checking if the username exists in the student list																																
	{
		if (strcmp(tempStud->StudentUsername, username) == 0)
		{
			return tempStud;
		}
		tempStud = tempStud->StudentNext;
	}
	return NULL;
}

//HELP FUNCTION for Login(Global)
//Receives a username and returns a pointer to the watcher with that username.
Watcher* FindWatcherByUN(Global *g, char *username)
{
	Watcher *tempW = g->WatchersList;
	while (tempW != NULL)																		//Checking if the username exists in the student list																																
	{
		if (strcmp(tempW->WatcherUsername, username) == 0)
		{
			return tempW;
		}
		tempW = tempW->WatcherNext;
	}
	return NULL;
}

//HELP FUNCTION for Login(Global)
//Receives a username and returns a pointer to the admin with that username.
Admin* FindAdminByUN(Global *g, char *username)
{
	Admin *tempA = g->AdminsList;
	while (tempA != NULL)																		//Checking if the username exists in the student list																																
	{
		if (strcmp(tempA->AdminUsername, username) == 0)
		{
			return tempA;
		}
		tempA = tempA->AdminNext;
	}
	return NULL;
}

int Login(Global *g)
{
	char username[81], pass[81];
	int i;
	Student *studLogin = NULL;
	Watcher *watchLogin = NULL;
	Admin *admLogin = NULL;

	printf("Please enter your username.\n");														//Entering username.
	scanf("%s", username);
																									//Finding this username in one of the databases.
	studLogin = FindStudentByUN(g, username);
	watchLogin = FindWatcherByUN(g, username);
	admLogin = FindAdminByUN(g, username);
																									
	if (studLogin == NULL && watchLogin == NULL && admLogin == NULL)								//If not fount in any database - return 0.
	{
		printf("No such user in our system.\n");
		return 0;
	}

	if (studLogin)																					//User inputs a password up to 3 times.
	{																								//If the password is correct - ID returned.
		for (i = 0; i < 3; i++)																		//If none of the attempts were successful - 0 returned.
		{
			_flushall();
			printf("Please enter you password.\n");
			scanf("%s", pass);
			if (strcmp(studLogin->StudentPassword, pass) == 0)
				return studLogin->StudentID;
		}
	}
	else if (watchLogin)
	{
		for (i = 0; i < 3; i++)
		{
			_flushall();
			printf("Please enter you password.\n");
			scanf("%s", pass);
			if (strcmp(watchLogin->WatcherPassword, pass) == 0)
				return watchLogin->WatcherID;
		}
	}
	else if (admLogin)
	{
		for (i = 0; i < 3; i++)
		{
			_flushall();
			printf("Please enter you password.\n");
			scanf("%s", pass);
			if (strcmp(admLogin->AdminPassword, pass) == 0)
				return admLogin->AdminID;
		}
	}
	printf("You have failed to enter you password correctly 3 times. Login failed.\n");
	return 0;
}