// Alexey
#include "DataBase.h"

/////// functions decalrations
void addUserToProject(Global *GlobalFile, Project *newProject);									//42
void CreateNewTask(Global *GlobalFile, Project *project, int UserID, AccessGroup group);		//53
void PrintProjectsList(Global *GlobalFile, int UserID, AccessGroup group);						//44
void printLogToFile(char *file, char msg[500]);

/////// declarations end
///////////////////////////////////// Johnatan func's

// change status of choosen task, done, ready for testing
void ChangeTaskStatus(Global* GlobalFile, Project* project, int userID, int accessGroup){
	int taskID, status;
	Task* task;
	//Getting a task from the user:

	printf("Enter Task ID:\n");
	scanf("%d", &taskID);
	//Making sure that task is in the relevant project:
	task = findTaskInProject(GlobalFile, project, taskID);
	if (!task){
		printf("Task does not belong to this project\n");
		return;
	}

	printf("Current status is: %s\n", convertStatusToString(task->TaskStatus));
	printf("Choose status to change into:\n");
	printf("Available Status:\n");
	printf("[0] New , [1] Elicitation, [2] Analysis, [3] VandV ");
	if (accessGroup != STUDENT) printf(" [4] Approved"); //Student can't change to approved
	printf("\n");
	printf("Enter an integer of your choice:\n");
	scanf("%d", &status);
	if (accessGroup == STUDENT && status == APPROVED) {
		printf("Student cannot set status to approved, please contact your watcher or admin\n");
		return;
	}
	if (status == NEW || status == ELICITATION || status == ANALYSIS || status == VandV || status == APPROVED)
		task->TaskStatus = (STATUS)status;
	else
		puts("Incorrect status identificator, status not been changed.");
}
// undone
void RemoveProject(Global* GlobalFile, Project* project, int userID, int accessGroup){
	char choice = 'O';
	printf("Are you sure you want to remove this project and all of its tasks (Y/N)?\n");

	while (choice == 'O'){
		choice = getchar();

		switch (choice){
		case 'Y':
		case 'y':
			RemoveProjectFromList(GlobalFile->ProjectsList, project->ProjectID);
			//delete // Need to implement functions that will remove this project id from all the users, from all the lists etc
			break;
		case 'N':
		case 'n':
			break;
		default: printf("No such option!\n");
			choice = 'O';
		}
	}

}

// choose student id in project and send message to him, done, ready for testing
void LeaveMessageToStudent(Global* GlobalFile, Project* project, Watcher* watcher){
	int studentID;
	Student* student;
	char* Message;
	printf("Please choose a studentID of the student you want to leave a message for (message will be shown once)\n");
	scanf("%d", &studentID);
	if (isStudentInProject(project, studentID)){
		student = FindStudent(GlobalFile->StudentList, studentID);
		if (!student) return; //If for some reason student is not in the global file, exiting function
			printf("Enter the message you want to leave (Between 5 to 30):\n");
		do
		scanf("%s", &Message);
		while (strlen(Message) < 5 || strlen(Message) > 30);
		strcpy(student->StudentMessages, Message);
	}
	// add message to student message file 
	FILE *file = fopen(student->StudentMessages, "a");
	fprintf("%s %s\n", watcher->WatcherUsername, Message);

}

void AddProjectMessage(Global* GlobalFile, Project* project, Watcher* watcher){
	char* Message = "";
	printf("Enter the message you want to leave (Between 5 to 30):\n");
	do
	scanf("%s", &Message);
	while (strlen(Message) < 5 || strlen(Message) > 30);

	strcpy(project->ProjectMessages, Message);

	// add message to project activity and Messages logs
	char log[120];
	sprintf(log,"%s added message : %s", watcher->WatcherUsername, Message);
	printLogToFile(project->ProjectMessages,log);
	printLogToFile(project->ProjectActivityLogs, log);
}

void ShowTasksByStatus(Global* GlobalFile, int studentID){
	int tasksCount = 1, i, j, status = -1, projectID, taskID;
	Student* student = FindStudent(GlobalFile->StudentList, studentID);
	Project* project;
	Task* task;
	int* projectIDs = student->ProjectIDS;

	printf("Available Status:\n");
	printf("[0] New , [1] Elicitation, [2] Analysis, [3] VandV, [4] Approved\n ");
	printf("Enter an integer of your choice:\n");
	do
	scanf("%d", &status);
	while (status < 0 || status > 4);

	//Finding all the projects this student belongs to and printing the tasks:
	for (i = 0; i < student->StudentProjectsAmount; i++){
		projectID = projectIDs[i];
		project = FindProject(GlobalFile->ProjectsList, projectID);
		for (j = 0; j < project->ProjectTasksAmount; j++){
			taskID = project->TasksIDS[j];
			task = findTaskInProject(GlobalFile, project, taskID);
			if (task->TaskStatus == status){
				printf("%d.)ID: %d , Creator: %s , Task: %s \n", tasksCount, task->TaskID, task->TaskCreatorName, task->TaskName);
				tasksCount++;
			}
		}
	}
}

int PrintTasksList(Global* GlobalFile, Project* Project){
	int i, j, status, taskID;
	Task* task;
	char* creator;
	char* taskName;

	printf("Tasks in project:\n");
	if (Project->ProjectTasksAmount == 0)
	{
		puts("No Tasks created in this project");
		return -1;		// No tasks returned value
	}

	for (j = 1, i = 0; i < Project->ProjectTasksAmount; i++){
		task = FindTask(GlobalFile->TaskList, Project->TasksIDS[i]);
		status = task->TaskStatus;
		if (status == 5) //Trash and won't show it
			continue;
		taskID = task->TaskID;
		creator = task->TaskCreatorName;
		taskName = task->TaskName;

		printf("---\n");
		printf("%d) ID: %d Creator: %s Status: %d\n", j, taskID, creator, status);
		printf("Task: %s\n\n", taskName);
		j++;
	}
	return 0;	// planned end of function
}

void PrintActivityLog(Global* GlobalFile, Project* project){

	char BUFFER[400], *fileName = project->ProjectActivityLogs;
	FILE* file = fopen(fileName, "r");
	if (!file) return;

	while (fgets(BUFFER, 400, file)) printf("%s\n", BUFFER);
	fclose(file);
}

void PrintProjectDetails(Global* GlobalFile, Project* project){
	int projectID = project->ProjectID,
		numOfTasks = project->ProjectTasksAmount,
		numOfUsers = project->ProjectUsersAmount;
	char* creator = project->ProjectCreatorName, *projectName = project->ProjectName;

	printf("Project Details:\n");
	printf("-------------------");
	printf("ID: %d , Number of tasks: %d , Number of users: %d , Creator: %s", projectID, numOfTasks, numOfUsers, creator);

	//Need to print details of student here too.
	printf("Students in project:\n");
	printf("----------------------");
	//Need to add a helper function for that
}


//////////////////////////////////// Johnatan func's end


// add message to file
void printLogToFile(char *file, char msg[500])
{
	FILE *fp = fopen(file, "a");
	fprintf(fp,"%s", msg);
	fclose(fp);
}
////////////		Project create functions
// add new project to student's array of Project ID's, done, ready for testing
void AddProjectIDToStudent(Student * Student, int ProjectID)
{
	int ProjectIDSNewSize,index;
	int* newArrayIDs;	//create new increased array

	ProjectIDSNewSize = Student->StudentProjectsAmount + 1;
	newArrayIDs = (int*)malloc(ProjectIDSNewSize * sizeof(int));

	for (index = 0; index < ProjectIDSNewSize - 1; index++)
		newArrayIDs[index] = Student->ProjectIDS[index];			// copy old values
	newArrayIDs[index] = ProjectID;					// add new project id to last index of nea array

	free(Student->ProjectIDS);											// free old array memory
	Student->ProjectIDS = newArrayIDs;									// set new array pointer to student
	Student->StudentProjectsAmount++;								// update size of projects amount to watcher
}
// add new project to watcher's array of Project ID's, done, ready for testing
void AddProjectIDToWatcher(Watcher * Watcher, int ProjectID)
{
	int ProjectIDSNewSize, index;
	int* newArrayIDs;	//create new increased array

	ProjectIDSNewSize = Watcher->WatcherProjectsAmount + 1;
	newArrayIDs = (int*)malloc(ProjectIDSNewSize * sizeof(int));

	for (index = 0; index < ProjectIDSNewSize - 1; index++)
		newArrayIDs[index] = Watcher->ProjectIDS[index];			// copy old values
	newArrayIDs[index] = ProjectID;					// add new project id to last index of nea array

	free(Watcher->ProjectIDS);											// free old array memory
	Watcher->ProjectIDS = newArrayIDs;									// set new array pointer to student
	Watcher->WatcherProjectsAmount++;								// update size of projects amount to watcher
}
// 46 - Create new project by STUDENT or WATCHER only, UNDONE, should to split
int CreateNewProject(Global* GlobalFile,int userID, AccessGroup userGroup)
{
	Student *Student;
	Watcher *Watcher;
	FILE *file = NULL;
	puts("Input Name of your project (maximum 127 characters) : ");
	char buffer[128];		// get name
	char logText[500];
	gets(buffer);
	
	// check if student or watcher and get pointer to him
	if (userGroup == STUDENT)
		Student = FindStudent(GlobalFile->StudentList, userID);

	else if (userGroup == WATCHER)
		Watcher = FindWatcher(GlobalFile->WatchersList, userID);
	else
	{
		puts("Incorrect access group received.");
		return 1;		// incorrect access group
	}

	// allocate memory
	Project *newProject = (Project*)malloc(sizeof(Project));

	newProject->ProjectID = GlobalFile->ProjectRunID;		// set runID
	GlobalFile->ProjectRunID++;								// increase runID

	strcpy(newProject->ProjectName, buffer);	// copy name

	if (userGroup == STUDENT)
		strcpy(newProject->ProjectCreatorName, Student->StudentName);	// set student name
	else if (userGroup == WATCHER)
		strcpy(newProject->ProjectCreatorName, Watcher->WatcherName);	// set watcher name

	// fill basic fields
	newProject->ProjectUsersAmount = 1;		// 1 for creator
	// add creator ID to project array
	newProject->StudentsIDS = (int*)malloc(newProject->ProjectUsersAmount*sizeof(int));
	if (userGroup == STUDENT);
	newProject->StudentsIDS[0] = userID;

	newProject->ProjectTasksAmount = 0;
	newProject->TasksIDS = NULL;
	newProject->ProgramChanges = FALSE;
	newProject->ProjectNext = NULL;

	// create file name ID_ProjectActivityLog.txt and put name to ProjectActivityLog field
	char ActivityFileName[50];		
	sprintf(ActivityFileName, "%d_ProjectActivityLog.txt", newProject->ProjectID);
	FILE *ProjActivityLog = fopen(ActivityFileName, "w");
	fclose(ProjActivityLog);
	strcpy(newProject->ProjectActivityLogs, ActivityFileName);

	// create file name ID_ProjectMessages.txt and put name to ProjectMessages field
	char MessagesFileName[50];		
	sprintf(MessagesFileName, "%d_ProjectMessages.txt", newProject->ProjectID);
	FILE *ProjMsgs = fopen(MessagesFileName, "w");
	fclose(ProjMsgs);
	strcpy(newProject->ProjectMessages, MessagesFileName);

	// add new project to list of projects Global
	AddProject(GlobalFile->ProjectsList, newProject); 

	//add id of new project to creator
	if (userGroup == STUDENT)			// add project ID to student 
		AddProjectIDToStudent(Student, newProject->ProjectID);

	else if (userGroup == WATCHER)			// add project ID to Watcher's array
		AddProjectIDToWatcher(Watcher, newProject->ProjectID);


	// print log to project activity file

	sprintf(logText, "Project [%d] created by %s\n", newProject->ProjectID, newProject->ProjectCreatorName);
	printLogToFile(newProject->ProjectActivityLogs, logText);
	//---------------------

	// print log to student activity file
	if (Student)
	{
		sprintf(logText, "Created project \" %s \" with id [%d]\n", newProject->ProjectID, newProject->ProjectCreatorName);
		printLogToFile(Student->StudentActivityLog, logText);
	}
	//---------------------

	char choice;
	printf("Project \"%s\" created!\nWant to add collaborators to this project? (y / n) :", newProject->ProjectName);
	fflush(stdin);
	choice = getchar();
	switch (choice)
	{
	case 'y':
		addUserToProject(GlobalFile, newProject);
		break;
	case 'n':
		puts("Ok!");
		break;
	default:
		puts("Incorrect Symbol inputed");
		break;
	}
	puts("1 .Return to previous menu");
	puts("2. Exit Mesimaster");
	fflush(stdin);
	choice = getchar();
	if (choice == '1')
		return 1; //returns to menu
	else if (choice == '2')
		Exit(GlobalFile);			// check correct choice
	else
		puts("Incorrect input, you will be returned to Menu");
	return 1;

}
//////////			Project create funcs end

// 42 - Add user to project  , done, ready for testing
void addUserToProject(Global *GlobalFile, Project *newProject)
{
	BOOL flag = TRUE;
	FILE *file = fopen(newProject->ProjectActivityLogs,"a");
	Student *student=NULL;
	Watcher *watcher= NULL;
	char choice;
	int   i, ProjectUsersIDNewSize, ID;
	int *UsersID;	//create new increased array


	while (flag)
	{
		puts("1. Add Student ID");
		puts("2. Add Watcher ID");
		puts("3. Return to main menu");
		fflush(stdin);
		choice = getchar();
		switch (choice)
		{
		case '1':
			fflush(stdin);
			printf("\nStudent ID :");
			scanf("%d", &ID);
			student = FindStudent(GlobalFile->StudentList, ID);
			if (student)	// student found
			{
				/// add project to student 
				AddProjectIDToStudent(student, newProject->ProjectID);
				printf("\nStudent with ID : %d was added to project!\n", ID);
				fprintf(file,"Student with ID : %d was added to project!\n", ID);

				
				// add student to project
				ProjectUsersIDNewSize = newProject->ProjectUsersAmount + 1;
				UsersID = (int*)malloc(ProjectUsersIDNewSize*sizeof(int));
				for (i = 0; i < ProjectUsersIDNewSize - 1; i++)
					UsersID[i] = newProject->StudentsIDS[i];
				UsersID[i] = ID;

				newProject->StudentsIDS = UsersID;
			}
			else
				puts("Student ID not found. Try again.");
			break;

		case '2':
			fflush(stdin);
			printf("\nWatcher ID :");
			scanf("%d", &ID);
			watcher = FindWatcher(GlobalFile->WatchersList, ID);
			if (watcher)	// watcher found
			{
				/// add project to watcher 
				AddProjectIDToWatcher(watcher, newProject->ProjectID);

				printf("\nWatcher with ID : %d was added to project!\n", ID);
				fprintf(file,"Watcher with ID : %d was added to project!\n", ID);

				// add watcher to project
				ProjectUsersIDNewSize = newProject->ProjectUsersAmount + 1;
				UsersID = (int*)malloc(ProjectUsersIDNewSize*sizeof(int));
				for (i = 0; i < ProjectUsersIDNewSize - 1; i++)
					UsersID[i] = newProject->StudentsIDS[i];
				UsersID[i] = ID;

				newProject->StudentsIDS = UsersID;
			}
			else
				puts("Watcher ID not found. Try again.");
			break;
		case '3':
			puts("Choosen to return back");
			flag = FALSE;
			break;
		default:
			puts("Incorrect choice. Try again.");
			break;
		}

	}

	fclose(file);
}

// Add id of new task to array in project
void AddTaskIDToProject(Project* project, int TaskID)
{
	int TaskIDSNewSize, index;
	int* newArrayIDs;	//create new increased array

	TaskIDSNewSize = project->ProjectTasksAmount + 1;
	newArrayIDs = (int*)malloc(TaskIDSNewSize * sizeof(int));

	for (index = 0; index < TaskIDSNewSize - 1; index++)
		newArrayIDs[index] = project->TasksIDS[index];			// copy old values
	newArrayIDs[index] = TaskID;					// add new project id to last index of nea array

	free(project->TasksIDS);											// free old array memory
	project->TasksIDS = newArrayIDs;									// set new array pointer to student


}

// 53 - create new task from project menu, done, ready for testing
void CreateNewTask(Global *GlobalFile, Project *project,int UserID,AccessGroup group)
{
	Student *student = NULL;
	Watcher *watcher = NULL;
	char log[300];
	Task *newTask = (Task*)malloc(sizeof(Task));	//new task pointer

	newTask->TaskID = GlobalFile->TaskRunID;		// set Task Run ID

	GlobalFile->TaskRunID++;						// increase run id by 1
	fflush(stdin);
	puts("Enter your task (up to 255 chars) :");
	gets(newTask->TaskName);
	if (group == STUDENT)
		student = FindStudent(GlobalFile->StudentList, UserID);
	else if (group == WATCHER)
		watcher = FindWatcher(GlobalFile->WatchersList, UserID);
	else
	{
		puts("Incorrect access group");
		return;
	}

	if (group == STUDENT && student)
	{
		strcpy(newTask->TaskCreatorName, student->StudentName);			// copy creator name to task
		student->StudentTasksAmount++;
		// add log to student
		sprintf(log, "%s created task : %s", student->StudentName, newTask->TaskName);
		printLogToFile(student->StudentActivityLog, log);
	}
	else if (group == WATCHER && watcher)
		strcpy(newTask->TaskCreatorName, watcher->WatcherName);			// copy creator name to task
	

	newTask->TaskStatus = NEW;								// initialize status
	
	// add ID of new task to array TasksIDs in project
	AddTaskIDToProject(project, newTask->TaskID);

	// add new task to global list of Tasks
	AddTask(GlobalFile->TaskList, newTask);
	printf("New Task \"%s\" was created in project \"%s\"\n",newTask->TaskName,project->ProjectName);
	// add creation of task to project log
	printLogToFile(project->ProjectActivityLogs, log);

	// switch flag for watcher subscribes
	if (project->ProgramChanges == FALSE)
		project->ProgramChanges = TRUE;
}

// print list of projects with details in which user is collaborator - for students, done, ready for testing
void PrintProjectsList(Global *GlobalFile, int UserID, AccessGroup group)
{
	int i;
	Student* student=NULL;
	Watcher *watcher = NULL;
	Project *current = NULL;
	int *ProjectsIDS;

	if (GlobalFile->ProjectsList == NULL)
	{
		puts("No Projects in database");
		return;
	}

	// find user
	if (group == STUDENT)
	{
		student = FindStudent(GlobalFile->StudentList, UserID);
		if (student->StudentProjectsAmount == 0)
		{
			puts("Student not in any project");
			return;
		}
		ProjectsIDS = student->ProjectIDS;

		int arraySize = sizeof(ProjectsIDS) / sizeof(int);
		puts("List of your projects :");
		printf("ID\tName\tUsers\tTasks");
		for (i = 0; i < arraySize; i++)
		{
			current = FindProject(GlobalFile->ProjectsList, student->ProjectIDS[i]);
			if (current)
				printf("%d\t%s\t%d\t%d",current->ProjectID,current->ProjectName,current->ProjectUsersAmount,current->ProjectTasksAmount);
		}
	}// in case of watcher
	else if (group == WATCHER)
	{
		watcher = FindWatcher(GlobalFile->WatchersList, UserID);
		ProjectsIDS = watcher->ProjectIDS;
		if (watcher->WatcherProjectsAmount == 0)
		{
			puts("Watcher not in any project");
			return;
		}

		int arraySize = sizeof(ProjectsIDS) / sizeof(int);
		puts("List of your projects :");
		printf("ID\tName\tUsers\tTasks");
		for (i = 0; i < arraySize; i++)
		{
			current = FindProject(GlobalFile->ProjectsList, watcher->ProjectIDS[i]);
			if (current)
				printf("%d\t%s\t%d\t%d", current->ProjectID, current->ProjectName, current->ProjectUsersAmount, current->ProjectTasksAmount);	
		}
	}
	else
	{
		puts("Incorrect access group");
		return;
	}
	
}

// print users list by ID array in project, done,ready fro testing
void PrintUsersByID(Global *GlobalFile, Project *project)
{
	int i;
	Student *student = NULL;
	puts("Collaborators in project");
	if (project->ProjectUsersAmount == 0)
		puts("No users added.");
	printf("Username\t\tName\t\tSurename\t\tDepartment\tEmail\t");
	for (i = 0; i < project->ProjectUsersAmount; i++)
	{
		student = FindStudent(GlobalFile->StudentList, project->StudentsIDS[i]);
		if (student)
			printf("%s\t\t%s\t%s\t%d\t%s", student->StudentUsername, student->StudentName, student->StudentSurename, student->StudentDepartment, student ->StudentEmail);
	}

}

// print list of projects with details in which watcher is watching, done, ready for testing
void PrintProjectsListWatcher(Global *GlobalFile, int UserID, AccessGroup group)
{
	int i;
	Student* student = NULL;
	Watcher *watcher = NULL;
	Project *current = NULL;
	int *ProjectsIDS;

	if (GlobalFile->ProjectsList == NULL)
	{
		puts("No Projects in database");
		return;
	}

	// in case of watcher
	if (group == WATCHER)
	{
		watcher = FindWatcher(GlobalFile->WatchersList, UserID);
		ProjectsIDS = watcher->ProjectIDS;
		if (watcher->WatcherProjectsAmount == 0)
		{
			puts("Watcher not in any project");
			return;
		}

		int arraySize = sizeof(ProjectsIDS) / sizeof(int);
		puts("List of your projects :");
		printf("ID\tName\tUsers\tTasks");
		for (i = 0; i < arraySize; i++)
		{
			current = FindProject(GlobalFile->ProjectsList, watcher->ProjectIDS[i]);
			if (current)
			{
				printf("%d\t%s\t%d\t%d", current->ProjectID, current->ProjectName, current->ProjectUsersAmount, current->ProjectTasksAmount);
				PrintUsersByID(GlobalFile, current);				
			}
		}
	}
	else
	{
		puts("Incorrect access group");
		return;
	}

}

// print activity log of student in project, receive student, done, ready for testing
void PrintStudentLog(Student* student){
	char BUFFER[400], *fileName = student->StudentActivityLog;
	FILE* file = fopen(fileName, "r");
	if (!file) return;

	while (fgets(BUFFER, 400, file)) printf("%s\n", BUFFER);
	fclose(file);
}

// receive ID and return user group, done, ready for testing
int FindAccessGroup(int ID){
	//Should be in Functions, I wrote it for the meantime here
	if (ID >= 1000 && ID <= 1999) return STUDENT;
	else if (ID >= 2000 && ID <= 2999) return ADMIN;
	else if (ID >= 3000 && ID <= 3999) return WATCHER;
	
	return BAD;
}

void UpdateDetails(Global* GlobalFile, int userID){
	//Finding the user and its' struct:
	Watcher* watcher = NULL;
	Admin* admin = NULL;
	Student* student = NULL;
	char *name, *surname, *username , input;
	int AG = FindAccessGroup(userID);
	
	switch (AG){
	case STUDENT:
		student = FindStudent(GlobalFile->StudentList, userID);
		name = student->StudentName;
		surname = student->StudentSurename;
		username = student->StudentUsername;
		break;

	case ADMIN:
		admin = FindAdmin(GlobalFile->AdminsList, userID);
		name = admin->AdminName;
		surname = admin->AdminSurename;
		username = admin->AdminUsername;
		break;

	case WATCHER:
		watcher = FindWatcher(GlobalFile->WatchersList, userID);
		name = student->StudentName;
		surname = student->StudentSurename;
		username = watcher->WatcherUsername;
		break;
	}

	printf("Do you want to change your first name?(y/n)\n");
	scanf("%c", &input);
	if (input == 'y') {
		printf("Enter new first name:\n");
		scanf("%s", &name);
		input = 'n';
	}

	printf("Do you want to change your last name?(y/n)\n");
	scanf("%c", &input);
	if (input == 'y') {
		printf("Enter new last name:\n");
		scanf("%s", &surname);
		input = 'n';
	}

	printf("Do you want to change your user name?(y/n)\n");
	scanf("%c", &input);
	if (input == 'y') {
		printf("Enter new user name:\n");
		scanf("%s", &username);
		input = 'n';
	}

}

// print global message from admin, done, ready for testing
void PrintGlobalMessages(Global *GlobalFile)
{
	if (strcmp(GlobalFile->GlobalMessages, "EMPTY") != 0)
	{
		puts("\n*** System message ***");
		puts(GlobalFile->GlobalMessages);
		puts("\n**********************");
	}
	
}

// add message by admin, done, ready for etsting
void AddGlobalMessage(Global* GlobalFile){
	char temp[250];
	printf("Enter global message (max length 255):\n");
	do
	gets(temp);
	while (strlen(temp) > 255);

	strcpy(GlobalFile->GlobalMessages, temp);
}

// Quotes funcs start, done, ready to testing
void AddNewQuote(Global* GlobalFile){
	char tempQuote[300], tempAuthor[100];
	//Creating a new quote and incrementing quoteRunID: Not sure if that's right
	int quoteID = GlobalFile->QuoteRunID++;
	Quote* newQuote = (Quote*) malloc(sizeof(Quote));
	newQuote->QuoteID = quoteID;
	printf("Enter quote (max length 256):\n");
	do
	{
		fflush(stdin);
		scanf("%s", &tempQuote);
	}
	while (strlen(tempQuote) > 255);

	strcpy(newQuote->Quote, tempQuote);

	printf("Enter quote's author (max length 30):\n");
	do
	{
		fflush(stdin);
		scanf("%s", &tempAuthor);
	}
	while (strlen(tempQuote) > 30);

	strcpy(newQuote->QuoteAuthor, tempAuthor);

	//Adding to global quote list:
	AddQuote(GlobalFile->QuotesList, newQuote);
	GlobalFile->QuoteRunID++;

}
void PrintQuotes(Global* GlobalFile){
	Quote* quote = GlobalFile->QuotesList;
	int i = 1;
	puts("Quotes in Database:");
	while (quote != NULL){

		printf("%d) \"%s\"\t,Author: %s\n", i, quote->Quote, quote->QuoteAuthor);
		quote = quote->QuoteNext;
		i++;
	}
	puts("");
}
void DeleteQuote(Global* GlobalFile)
{
	Quote *quote = NULL;
	int ID;
	fflush(stdin);
	printf("Input Quote ID to delete : ");
	scanf("%d", &ID);
	quote = FindQuote(GlobalFile->QuotesList, ID);
	if (quote)
	{
		RemoveQuoteFromList(GlobalFile->QuotesList, ID);
		printf("Quote with id : %d found and deleted.\n", ID);
	}
	else
		puts("ID not found in database");
}
void ManageQuotes(Global *GlobalFile)
{
	char choice;

	BOOL flag = TRUE;
	
	while (flag)
	{
		puts("1. Print all Quotes in database");
		puts("2. Add new Quote");
		puts("3. Remove quote by ID");
		puts("4. Back to previous menu");
		fflush(stdin);
		printf("Choice : ");
		choice = getchar();
		if (choice == '1')
			PrintQuotes(GlobalFile);
		else if (choice == '2')
			AddNewQuote(GlobalFile);
		else if (choice == '3')
			DeleteQuote(GlobalFile);
		else if (choice == '4')
			flag=FALSE;
		else
			puts("Incorrect choice");
	}

}
// quotes funcs end


// prints all users in system, for admin, done, ready for testing
void PrintUsersLists(Global* GlobalFile)
{
	puts("Admins :");
	PrintAdminsList(GlobalFile->AdminsList);
	puts("");
	puts("Students :");
	PrintStudentList(GlobalFile->StudentList);
	puts("");
	puts("Watchers :");
	PrintWatcherList(GlobalFile->WatchersList);
	puts("");
}

// print all tasks with ID's from received array, done, ready for testing
void PrintTasksByID(Task *head, int indexes[], int size, char *creator)
{
	Task *current = NULL;
	int i,CreatedTasks=0;
	if (size == 0)
	{
		puts("No Tasks in this project");
		return;
	}
	printf("ID\tStatus\t\tTask\n");
	for (i = 0; i < size; i++)
	{
		current = FindTask(head, indexes[i]);
		if (current && strcmp(current->TaskCreatorName, creator) == 0)
		{
				char *status = convertStatusToString(current->TaskStatus);
				printf("%d\t%s\t%s\n", current->TaskID, status, current->TaskName);
				CreatedTasks++;
		}	
	}
	if (CreatedTasks == 0)
		puts("User not created any task in this project");
}

// print projects by ID's array, done, ready for testing
void PrintProjectsByID(Global *GlobalFile, int indexes[],int size,char *creator)
{
	Project *head = GlobalFile->ProjectsList;
	Project *current = NULL;
	int i;
	if (size == 0)
	{
		puts("No Projects");
		return;
	}
	for (i = 0; i < size; i++)
	{
		current = FindProject(head, indexes[i]);
		if (current)
		{
			printf("ID: %d, Name: %s, Tasks : %d\n\nTasks in this Project :\n", current->ProjectID, current->ProjectName, current->ProjectTasksAmount);
			PrintTasksByID(GlobalFile->TaskList, current->TasksIDS, current->ProjectTasksAmount,creator);
		}
	}
}

// print all users in system, gives option to chooce user for more information, done, ready for testing
void ShowUserDetails(Global *GlobalFile)
{
	Student * student = NULL;
	Watcher *watcher = NULL;
	Admin *admin = NULL;
	PrintUsersLists(GlobalFile);
	
	char choice;
	int ID;
	printf("Input ID to see full details of user : ");
	fflush(stdin);
	scanf("%d",&ID);
	if (ID < 1000 || ID >=4000)
		puts("incorrect ID , correct range 1000-3999");
	else if (ID >= 1000 && ID <= 1999)
	{
		student = FindStudent(GlobalFile->StudentList, ID);
		if (student)
		{
			printf("User data for user with ID %d",ID);
			printf("\nUsername : %s\nName : %s\nSurename : %s\nEmail : %s\nDepartment :%s\n", student->StudentUsername, student->StudentName, student->StudentSurename, student->StudentEmail, student->StudentDepartment);
			puts("--------------\nCollaborator at Projects:");
			PrintProjectsByID(GlobalFile, student->ProjectIDS,student->StudentProjectsAmount,student->StudentName);
			
		}
		else
			puts("Student with this id not found");

	}
	else if (ID >= 2000 && ID <= 2999)
	{
		admin = FindAdmin(GlobalFile->AdminsList, ID);
		if (admin)
		{
			printf("User data for user with ID %d", ID);
			printf("\nUsername : %s\nPassword :%s\nName : %s\n%Surename\n\n", admin->AdminUsername, admin->AdminPassword, admin->AdminName, admin->AdminSurename);
		}
		else
			puts("Admin with this ID not found");

	}
	else if (ID >= 3000 && ID <= 3999)
	{
		watcher = FindWatcher(GlobalFile->WatchersList, ID);
		if (watcher)
		{
			printf("User data for user with ID %d\n", ID);
			printf("Username : %s\nPassword :%s\nName : %s\n%Surename\nEmail : %s\nProjects Amount : %d\n\n", watcher->WatcherUsername, watcher->WatcherPassword, watcher->WatcherName, watcher->WatcherSurename, watcher->WatcherProjectsAmount);
		}
		else
			puts("Watcher with this ID not found");
	}
		printf("Return to previous menu ( Y / N for Exit ) :");
		fflush(stdin);
	choice = getchar();
	if (choice == 'Y' || choice == 'y')
		return;
	else if (choice == 'N' || choice == 'n')
		Exit(GlobalFile);											//////// check correct option for this
	else
		puts("Incorrect choice, returning to previous menu");
}

//receive id and project, remove user ID from array of USersIDs and update array, done, ready for testing
int *RemoveUserIDFromProject(Project* project, int ID)
{
	int *UsersIDs = (int*)malloc((project->ProjectUsersAmount - 1) * sizeof(int)),i,newindex=0;
	
	for (i = 0; i < project->ProjectUsersAmount; i++,newindex++)
	{
		if (project->StudentsIDS[i] != ID)
			UsersIDs[newindex] = project->StudentsIDS[i];
		else
			newindex--;
	}

	return UsersIDs;
}

// receive user id, run over all projects, if user in project, remove it from array, and decrease amount of users in project, done, ready for testing
void RemoveUserFromProjects(Global *GlobalFile, int UserID)
{
	Project *current = GlobalFile->ProjectsList;
	int i;
	while (current)
	{
		for (i = 0; i < current->ProjectUsersAmount; i++)
		{
			if (current->StudentsIDS[i] == UserID)
			{
				current->StudentsIDS = RemoveUserIDFromProject(current, UserID);
				current->ProjectUsersAmount--;
			}
		}
		current = current->ProjectNext;
	}

}

// print all users in database for admin, admin chooce user id for delete, done, ready for testing
void DeleteUser(Global *GlobalFile)
{
	int ID;
	AccessGroup group;
	Student *student = NULL;
	Watcher *watcher = NULL;
	Admin *admin = NULL;

	puts("List of all users in system :\n-----------------------");
	PrintUsersLists(GlobalFile);
	printf("Enter ID of user you want to remove: ");
	fflush(stdin);
	scanf("%d", &ID);
	group = FindAccessGroup(ID);
	switch (group)
	{
	case WATCHER:
		watcher = FindWatcher(GlobalFile->WatchersList, ID);
		if (watcher)
		{
			RemoveWatcherFromList(GlobalFile->WatchersList, ID);
			RemoveUserFromProjects(GlobalFile, ID);
		}
		else
			puts("Watcher not found in database");
		break;
	case STUDENT:
		student = FindStudent(GlobalFile->StudentList, ID);
		if (student)
		{
			RemoveStudentFromList(GlobalFile->StudentList, ID);
			RemoveUserFromProjects(GlobalFile, ID);
		}
		else
			puts("Student not found in database");
		break;
	case ADMIN:
		admin = FindAdmin(GlobalFile->AdminsList, ID);
		if (admin)
			RemoveAdminFromList(GlobalFile->AdminsList, ID);
		else
			puts("Admin not found in database");
		break;
	}
}

// promote user to Admin, done, ready for testing
void PromoteUserToAdmin(Global *GlobalFile)
{
	puts("List of all users in system :\n-----------------------");
	PrintUsersLists(GlobalFile);
	int ID;
	Student *student = NULL;
	Watcher *watcher = NULL;
	AccessGroup group = BAD;
	
	printf("Input ID of user, you want to promoto to ADMIN group : ");
	fflush(stdin);
	scanf("%d", &ID);
	group = FindAccessGroup(ID);
	switch (group)
	{
	case BAD:
		puts("Incorrect range of ID ( 1000 - 3999 )");
		break;
	case STUDENT:
		student = FindStudent(GlobalFile->StudentList, ID);
		if (student)
			student->Group = ADMIN;
		else
			puts("Student with this id not found");
		break;

	case WATCHER:
		watcher = FindWatcher(GlobalFile->WatchersList, ID);
		if (watcher)
			watcher->Group = ADMIN;
		else
			puts("Watcher with this id not found");
		break;

	case ADMIN:
		puts("User with this ID is admin already");
	}
	
}


//// watcher notifications
// Turns on / off watcher notifications, return true if status changed, false if not changed
BOOL ShowNotifications(Global *GlobalFile, Watcher *watcher)
{
	char choice;

	printf("\nCurrent status of Subscription : ");
	if (watcher->WatcherReceiveChanges == FALSE)
		puts("OFF");
	if (watcher->WatcherReceiveChanges == TRUE)
		puts("ON");

	printf("\nChange status? ( Y/N ) : ");
	fflush(stdin);
	choice = getchar();
	if (choice == 'y' || choice == 'Y')
	{
		puts("New status : ON");
		watcher->WatcherReceiveChanges = TRUE;
		return TRUE;
	}
	else if (choice == 'n' || choice == 'N')
	{
		puts("New status : OFF");
		watcher->WatcherReceiveChanges = FALSE;
		return TRUE;
	}
	puts("Incorrect choice, status not been changed");
		return FALSE;
	
	

}

// print changes in project to watcher
void PrintProjectChanges(Global *GlobalFile, Project* project, Watcher *watcher)
{
	if (watcher->WatcherReceiveChanges == TRUE)
	{
		puts("Last changes in this project : ");
		PrintActivityLog(GlobalFile, project);
	}

}
//// watcher notifications end

// show messages from watcher to student, done, ready for testing
void ShowMessagesToStudent(Global * Global, Student *student)
{

	FILE *file = fopen(student->StudentMessages, "r");
	char username[31], message[31];
	if (file)
	{
		puts("You have one or more messages from watchers");
		while (!feof(file))
		{
			fscanf("%s %s", &username, &message);
			printf("User : %s, Message :%s", &username, &message);
		}
	}
	fclose(file);
	// once messages been printed to student, delete old file by creating new empty one
	file = fopen(student->StudentMessages, "w");
	fclose(file);
}


// wtite  function to print project messages to project team! 